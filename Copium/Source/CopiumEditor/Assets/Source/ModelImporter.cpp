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
import <limits>;
import <memory>;
import <string>;

namespace fs = std::filesystem;


namespace AssimpConstants
{
    using namespace Copium;

    // Index
    static constexpr int32 IndicesPerFace = 3;
    // Position
    static constexpr int32 PositionDimension = 3;
    static constexpr int32 PositionTypeSize = sizeof(float32);
    static constexpr int32 PositionSize = PositionDimension * PositionTypeSize;
    // Normal
    static constexpr int32 NormalDimension = 3;
    static constexpr int32 NormalTypeSize = sizeof(float32);
    static constexpr int32 NormalSize = NormalDimension * NormalTypeSize;
    // UV0
    static constexpr int32 UV0Dimension = 2;
    static constexpr int32 UV0TypeSize = sizeof(float32);
    static constexpr int32 UV0Size = UV0Dimension * UV0TypeSize;

#define MEMBER_SIZEOF(type, member) sizeof(((type *)0)->member)
    static_assert(MEMBER_SIZEOF(aiVector3D, x) == PositionTypeSize);
    static_assert(MEMBER_SIZEOF(aiVector3D, x) == NormalTypeSize);
#undef MEMBER_SIZEOF

#define ARRAY_MEMBER_SIZEOF(type, member) sizeof(((type *)0)->member[0])
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
                    COP_LOG_WARN("Material has {:d} BaseColorMaps. [{:s}]", diffuseTexturesCount, assimpMaterialName);
                }
            }
            else
            {
                COP_LOG_WARN("Material doesn't have BaseColorMap, using default Black texture. [{:s}]", assimpMaterialName);
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
                    COP_LOG_WARN("Material has {:d} NormalMaps. [{:s}]", diffuseTexturesCount, assimpMaterialName);
                }
            }
            else
            {
                COP_LOG_WARN("Material doesn't have NormalMap, using default Normal texture. [{:s}]", assimpMaterialName);
                material->SetNormalMap(normalTexture);
            }

            materials.push_back(std::move(material));
        }

        return materials;
    }


    [[nodiscard]] static IndexFormat CopyIndexData(const aiMesh* assimpMesh, std::vector<uint8>& indexData)
    {
        const uint32 numFaces = assimpMesh->mNumFaces;
        const uint32 numIndices = numFaces * AssimpConstants::IndicesPerFace;
       
        const aiFace* const assimpFaces = assimpMesh->mFaces;

        uint32 indexDataSize;
        IndexFormat indexFormat;

        if (numIndices > std::numeric_limits<uint16>::max())
        {
            indexDataSize = numIndices * sizeof(uint32);
            indexFormat = IndexFormat::UInt32;
        }
        else
        {
            indexDataSize = numIndices * sizeof(uint16);
            indexFormat = IndexFormat::UInt16;
        }

        indexData.resize(indexDataSize);
        uint8* indexDataPtr = indexData.data();

        if (numIndices > std::numeric_limits<uint16>::max())
        {
            for (uint32 i = 0; i < numFaces; i++)
            {
                const aiFace& assimpFace = assimpFaces[i];
                const uint32 indices[3] = {
                    assimpFace.mIndices[0],
                    assimpFace.mIndices[1],
                    assimpFace.mIndices[2],
                };

                std::memcpy(indexDataPtr, indices, sizeof(indices));
                indexDataPtr += sizeof(indices);
            }
        }
        else
        {
            for (uint32 i = 0; i < numFaces; i++)
            {
                const aiFace& assimpFace = assimpFaces[i];
                const uint16 indices[3] = {
                    static_cast<uint16>(assimpFace.mIndices[0]),
                    static_cast<uint16>(assimpFace.mIndices[1]),
                    static_cast<uint16>(assimpFace.mIndices[2]),
                };

                std::memcpy(indexDataPtr, indices, sizeof(indices));
                indexDataPtr += sizeof(indices);
            }
        }

        return indexFormat;
    }

    [[nodiscard]] static void CopyUV0Data(const aiMesh* assimpMesh, uint8* vertexDataPtr)
    {
        const uint32 numVertices = assimpMesh->mNumVertices;
        const aiVector3D* const assimpUV0Data = assimpMesh->mTextureCoords[0];

        for (uint32 uv0Index = 0; uv0Index < numVertices; uv0Index++)
        {
            const aiVector3D vertexUV0 = assimpUV0Data[uv0Index];
            const float32 uv0[2] = { vertexUV0.x, vertexUV0.y };

            std::memcpy(vertexDataPtr, uv0, sizeof(uv0));
            vertexDataPtr += sizeof(uv0);
        }
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

        if (assimpScene->HasAnimations())
        {
            COP_LOG_WARN("Model has {:d} animations", assimpScene->mNumAnimations);
        }
        if (assimpScene->HasCameras())
        {
            COP_LOG_WARN("Model has {:d} cameras", assimpScene->mNumCameras);
        }
        if (assimpScene->HasLights())
        {
            COP_LOG_WARN("Model has {:d} lights", assimpScene->mNumLights);
        }

        const aiNode* const* const rootChildren = assimpScene->mRootNode->mChildren;
        const uint32 rootNumChildren = assimpScene->mRootNode->mNumChildren;

        for (uint32 i = 0; i < rootNumChildren; ++i)
        {
            if (rootChildren[i]->mNumChildren != 0)
            {
                COP_LOG_WARN("Model has more complex hierarchy than I support");
                break;
            }
        }

        const aiMesh* const* const assimpMeshes = assimpScene->mMeshes;
        const uint32 numMeshes = assimpScene->mNumMeshes;

        COP_LOG_INFO("Meshes: {:d} | Materials: {:d} | Embeded Textures: {:d}",
            numMeshes, assimpScene->mNumMaterials, assimpScene->mNumTextures);

        Entity parentEntity;
        const std::vector<std::shared_ptr<Material>> materials = GetAssimpMaterials(modelPath, assimpScene);

        for (uint32 meshIndex = 0; meshIndex < numMeshes; meshIndex++)
        {
            const aiMesh* const assimpMesh = assimpMeshes[meshIndex];

            COP_ASSERT(assimpMesh->HasFaces());
            COP_ASSERT(assimpMesh->HasPositions());

            std::string meshName(assimpMesh->mName.C_Str(), assimpMesh->mName.length);

            if (assimpMesh->HasNormals() == false)
            {
                COP_LOG_WARN("Mesh doesn't have Normals. [{:s}]", meshName);
                continue;
            }
            if (assimpMesh->HasTextureCoords(0) == false)
            {
                COP_LOG_WARN("Mesh doesn't have UV0. [{:s}]", meshName);
                continue;
            }

            const uint32 numFaces    = assimpMesh->mNumFaces;
            const uint32 numVertices = assimpMesh->mNumVertices;

            MeshDesc meshDesc = {
                .Name        = meshName,
                .Position    = VertexAttributeDesc::Position(),
                .Normal      = VertexAttributeDesc::Normal(),
                .UV0         = VertexAttributeDesc::UV0(),
                .IndexCount  = numFaces * AssimpConstants::IndicesPerFace,
                .VertexCount = numVertices,
            };
            meshDesc.IndexFormat = CopyIndexData(assimpMesh, meshDesc.IndexData);

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

            CopyUV0Data(assimpMesh, vertexDataPtr);

            Entity childEntity(std::move(meshName));
            childEntity.SetMesh(std::make_shared<Mesh>(std::move(meshDesc)));
            childEntity.SetMaterial(materials[assimpMesh->mMaterialIndex]);

            parentEntity.AddChild(std::move(childEntity));
        }

        return parentEntity;
    }

} // namespace Copium
