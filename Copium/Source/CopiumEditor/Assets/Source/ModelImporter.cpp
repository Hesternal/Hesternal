module;

#include "Engine/Core/Debug.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

module CopiumEditor.Assets.ModelImporter;

import CopiumEngine.Assets.Mesh;
import CopiumEngine.Assets.Material;
import CopiumEngine.Assets.Texture;
import CopiumEngine.Assets.Shader;
import CopiumEngine.Core.CoreTypes;
import CopiumEngine.ECS.Entity;
import CopiumEngine.Graphics;

import CopiumEditor.Assets.AssetDatabase;

import <filesystem>;
import <memory>;
import <string>;

namespace fs = std::filesystem;


namespace AssimpConstants
{
    using namespace Copium;

    // Index
    static constexpr int32 IndexTypeSize = sizeof(uint32);
    static constexpr int32 IndicesPerFace = 3;
    static constexpr int32 IndexSize = IndicesPerFace * IndexTypeSize;
    // Position
    static constexpr int32 PositionDimension = 3;
    static constexpr int32 PositionTypeSize = sizeof(float32);
    static constexpr int32 PositionSize = PositionDimension * PositionTypeSize;
    // Normal
    static constexpr int32 NormalDimension = 3;
    static constexpr int32 NormalTypeSize = sizeof(float32);
    static constexpr int32 NormalSize = NormalDimension * NormalTypeSize;
    // TexCoord0
    static constexpr int32 UV0Dimension = 2;
    static constexpr int32 UV0TypeSize = sizeof(float32);
    static constexpr int32 UV0Size = UV0Dimension * UV0TypeSize;

#define MEMBER_SIZEOF(type, member) sizeof(((type *)0)->member)
    static_assert(MEMBER_SIZEOF(aiVector3D, x) == PositionTypeSize);
    static_assert(MEMBER_SIZEOF(aiVector3D, x) == NormalTypeSize);
#undef MEMBER_SIZEOF

#define ARRAY_MEMBER_SIZEOF(type, member) sizeof(((type *)0)->member[0])
    static_assert(ARRAY_MEMBER_SIZEOF(aiFace, mIndices)  == IndexTypeSize);
    static_assert(ARRAY_MEMBER_SIZEOF(aiMesh, mVertices) == PositionSize);
    static_assert(ARRAY_MEMBER_SIZEOF(aiMesh, mNormals)  == NormalSize);
#undef ARRAY_MEMBER_SIZEOF
};


namespace
{

    using namespace Copium;


    [[nodiscard]] static std::string GetAssimpMaterialTexturePath(const fs::path& modelDirPath, const aiMaterial* material, aiTextureType textureType)
    {
        aiString texturePath;
        const aiReturn error = material->GetTexture(textureType, 0, &texturePath);
        COP_ASSERT_MSG(error == aiReturn::aiReturn_SUCCESS, error == aiReturn_FAILURE ? "aiReturn_FAILURE" : "aiReturn_OUTOFMEMORY");

        // NOTE(v.matushkin): This dances with pathes is so fucking dumb
        std::string projectDirRelativePath = (modelDirPath / std::string(texturePath.C_Str(), texturePath.length)).string();

        return projectDirRelativePath;
    }

    [[nodiscard]] static std::vector<std::shared_ptr<Material>> GetAssimpMaterials(const std::string& modelPath, const aiScene* assimpScene)
    {
        COP_ASSERT_MSG(assimpScene->HasMaterials(), "Mesh without materials");

        const fs::path modelDirPath = fs::path(modelPath).parent_path();

        std::vector<std::shared_ptr<Material>> materials;
        const uint32 numMaterials = assimpScene->mNumMaterials;

        const std::shared_ptr<Shader>& defaultShader = Graphics::GetDefaultShader();
        const std::shared_ptr<Texture>& blackTexture = Graphics::GetBlackTexture();
        const std::shared_ptr<Texture>& normalTexture = Graphics::GetNormalTexture();

        for (uint32 materialIndex = 0; materialIndex < numMaterials; materialIndex++)
        {
            const aiMaterial* const assimpMaterial = assimpScene->mMaterials[materialIndex];
            const char* const assimpMaterialName = assimpMaterial->GetName().C_Str();

            auto material = std::make_shared<Material>(std::string(assimpMaterialName, assimpMaterial->GetName().length), defaultShader);

            // Get Material BaseColorMap
            const uint32 diffuseTexturesCount = assimpMaterial->GetTextureCount(aiTextureType::aiTextureType_DIFFUSE);
            if (diffuseTexturesCount > 0)
            {
                const std::string texturePath = GetAssimpMaterialTexturePath(modelDirPath, assimpMaterial, aiTextureType::aiTextureType_DIFFUSE);
                material->SetBaseColorMap(AssetDatabase::LoadAsset<Texture>(texturePath));

                if (diffuseTexturesCount != 1)
                {
                    COP_LOG_WARN("Material: {}, has {} BaseColor textures", assimpMaterialName, diffuseTexturesCount);
                }
            }
            else
            {
                COP_LOG_WARN("Material: {}, has 0 baseColor textures, using default Black texture", assimpMaterialName);
                material->SetBaseColorMap(blackTexture);
            }
            // Get Material NormalMap
            const uint32 normalTexturesCount = assimpMaterial->GetTextureCount(aiTextureType::aiTextureType_NORMALS);
            if (normalTexturesCount > 0)
            {
                const std::string texturePath = GetAssimpMaterialTexturePath(modelDirPath, assimpMaterial, aiTextureType::aiTextureType_NORMALS);
                material->SetNormalMap(AssetDatabase::LoadAsset<Texture>(texturePath));

                if (normalTexturesCount != 1)
                {
                    COP_LOG_WARN("Material: {}, has {} Normal textures", assimpMaterialName, diffuseTexturesCount);
                }
            }
            else
            {
                COP_LOG_WARN("Material: {}, has 0 normal textures, using default Normal texture", assimpMaterialName);
                material->SetNormalMap(normalTexture);
            }

            materials.push_back(std::move(material));
        }

        return materials;
    }

}


namespace Copium
{

    Entity ModelImporter::Import(const std::string& modelPath)
    {
        Assimp::Importer assimpImporter;
        // TODO(v.matushkin): Learn more about aiPostProcessSteps
        const aiScene* assimpScene = assimpImporter.ReadFile(
            modelPath,
            aiPostProcessSteps::aiProcess_Triangulate
            | aiPostProcessSteps::aiProcess_GenNormals
            // | aiPostProcessSteps::aiProcess_GenUVCoords
            // | aiPostProcessSteps::aiProcess_FlipUVs          // Instead of stbi_set_flip_vertically_on_load(true); ?
            // | aiPostProcessSteps::aiProcess_FlipWindingOrder // Default is counter clockwise
            // | aiPostProcessSteps::aiProcess_MakeLeftHanded
            // Optimization
            // | aiPostProcessSteps::aiProcess_JoinIdenticalVertices
            // | aiPostProcessSteps::aiProcess_ImproveCacheLocality
            // Validation
            // | aiPostProcessSteps::aiProcess_FindDegenerates
            // | aiPostProcessSteps::aiProcess_FindInvalidData
            // | aiPostProcessSteps::aiProcess_ValidateDataStructure
        );

        // TODO(v.matushkin): This will break when asserts turned off
        if (assimpScene == nullptr)
        {
            COP_LOG_CRITICAL(
                "Got an error while loading Mesh"
                "\t\nPath: {0}"
                "\t\nAssimp error message: {1}",
                modelPath, assimpImporter.GetErrorString());
            COP_ASSERT(false);
        }

        Entity parentEntity;
        const std::vector<std::shared_ptr<Material>> materials = GetAssimpMaterials(modelPath, assimpScene);

        std::vector<uint32> indexData;
        std::vector<float32> uv0Data;
        indexData.reserve(100);
        uv0Data.reserve(100);

        for (uint32 meshIndex = 0; meshIndex < assimpScene->mNumMeshes; meshIndex++)
        {
            const aiMesh* const assimpMesh = assimpScene->mMeshes[meshIndex];

            COP_ASSERT(assimpMesh->HasFaces());
            COP_ASSERT(assimpMesh->HasPositions());
            COP_ASSERT(assimpMesh->HasNormals());

            // COP_ASSERT(assimpMesh->HasTextureCoords(0));
            if (assimpMesh->HasTextureCoords(0) == false)
            {
                continue;
            }

            const uint32 numFaces    = assimpMesh->mNumFaces;
            const uint32 numVertices = assimpMesh->mNumVertices;
            std::string meshName(assimpMesh->mName.C_Str(), assimpMesh->mName.length);

            MeshDesc meshDesc = {
                .Name        = meshName,
                .Position    = VertexAttributeDesc::Position(),
                .Normal      = VertexAttributeDesc::Normal(),
                .UV0         = VertexAttributeDesc::UV0(),
                .IndexFormat = IndexFormat::UInt32,
                .IndexCount  = numFaces * AssimpConstants::IndicesPerFace,
                .VertexCount = numVertices,
            };

            //- IndexData
            indexData.clear();
            for (uint32 faceIndex = 0; faceIndex < numFaces; faceIndex++)
            {
                const aiFace& face = assimpMesh->mFaces[faceIndex];

                COP_ASSERT(face.mNumIndices == 3);

                indexData.push_back(face.mIndices[0]);
                indexData.push_back(face.mIndices[1]);
                indexData.push_back(face.mIndices[2]);
            }

            const uint32 indexDataSize = numFaces * AssimpConstants::IndexSize;
            meshDesc.IndexData.resize(indexDataSize);
            std::memcpy(meshDesc.IndexData.data(), indexData.data(), indexDataSize);

            //- VertexData
            const uint32 positionDataSize = numVertices * AssimpConstants::PositionSize;
            const uint32 normalDataSize   = numVertices * AssimpConstants::NormalSize;
            const uint32 uv0DataSize      = numVertices * AssimpConstants::UV0Size;
            const uint32 vertexDataSize = positionDataSize + normalDataSize + uv0DataSize;

            meshDesc.VertexData.resize(vertexDataSize);
            uint8* vertexDataPtr = meshDesc.VertexData.data();

            //-- Position
            std::memcpy(vertexDataPtr, assimpMesh->mVertices, positionDataSize);
            vertexDataPtr += positionDataSize;

            //-- Normal
            std::memcpy(vertexDataPtr, assimpMesh->mNormals, normalDataSize);
            vertexDataPtr += normalDataSize;

            //-- UV0
            const aiVector3D* assimpUV0Data = assimpMesh->mTextureCoords[0];

            // Transform (3 * float32) -> (2 * float32)
            uv0Data.clear();
            for (uint32 uv0Index = 0; uv0Index < numVertices; uv0Index++)
            {
                const aiVector3D vertexUV0 = assimpUV0Data[uv0Index];
                uv0Data.push_back(vertexUV0.x);
                uv0Data.push_back(vertexUV0.y);
            }

            std::memcpy(vertexDataPtr, uv0Data.data(), uv0DataSize);

            Entity childEntity(std::move(meshName));
            childEntity.SetMesh(std::make_shared<Mesh>(std::move(meshDesc)));
            childEntity.SetMaterial(materials[assimpMesh->mMaterialIndex]);

            parentEntity.AddChild(std::move(childEntity));
        }

        return parentEntity;
    }

} // namespace Copium
