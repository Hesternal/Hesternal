module;

#include "Copium/Core/Debug.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

module CopiumEditor.Assets.ModelImporter;

import Copium.Core;
import Copium.Math;

import CopiumEngine.Assets.Material;
import CopiumEngine.Assets.Mesh;
import CopiumEngine.Assets.Model;
import CopiumEngine.Assets.Texture;
import CopiumEngine.Assets.Shader;
import CopiumEngine.Graphics;

import CopiumEditor.Assets.AssetDatabase;

import <filesystem>;
import <limits>;
import <memory>;
import <queue>;
import <string>;
import <string_view>;
import <utility>;

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

} // namespace AssimpConstants


namespace
{
    using namespace Copium;


    [[nodiscard]] static std::string GetAssimpMaterialTexturePath(const fs::path& modelDirPath, const aiMaterial* material, aiTextureType textureType)
    {
        aiString texturePath;
        // NOTE(v.matushkin): Useless method call without ASSERT
        [[maybe_unused]] const aiReturn error = material->GetTexture(textureType, 0, &texturePath);
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

        if (numIndices > Math::UInt16::Max)
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

        if (numIndices > Math::UInt16::Max)
        {
            for (uint32 i = 0; i < numFaces; i++)
            {
                const aiFace assimpFace = assimpFaces[i];
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
                const aiFace assimpFace = assimpFaces[i];
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

    static void CopyUV0Data(const aiMesh* assimpMesh, uint8* vertexDataPtr)
    {
        const uint32 numVertices = assimpMesh->mNumVertices;
        const aiVector3D* const assimpUV0Data = assimpMesh->mTextureCoords[0];

        for (uint32 i = 0; i < numVertices; i++)
        {
            const aiVector3D vertexUV0 = assimpUV0Data[i];
            const float32 uv0[2] = { vertexUV0.x, vertexUV0.y };

            std::memcpy(vertexDataPtr, uv0, sizeof(uv0));
            vertexDataPtr += sizeof(uv0);
        }
    }

    [[nodiscard]] static std::vector<ModelMesh> GetAssimpMeshes(const aiMesh* const* assimpMeshes, uint32 numMeshes)
    {
        std::vector<ModelMesh> meshes;
        // NOTE(v.matushkin): Can be reserved for more than needed if I encounter broken/unsupported meshes
        meshes.reserve(numMeshes);

        for (uint32 i = 0; i < numMeshes; i++)
        {
            const aiMesh* const assimpMesh = assimpMeshes[i];

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

            const uint32 numVertices = assimpMesh->mNumVertices;
            const uint32 positionDataSize = numVertices * AssimpConstants::PositionSize;
            const uint32 normalDataSize   = numVertices * AssimpConstants::NormalSize;
            const uint32 uv0DataSize      = numVertices * AssimpConstants::UV0Size;
            const uint32 vertexDataSize = positionDataSize + normalDataSize + uv0DataSize;

            MeshDesc meshDesc = {
                .Name        = std::move(meshName),
                .Position    = VertexAttributeDesc::Position(),
                .Normal      = VertexAttributeDesc::Normal(),
                .UV0         = VertexAttributeDesc::UV0(),
                .IndexCount  = assimpMesh->mNumFaces * AssimpConstants::IndicesPerFace,
                .VertexCount = numVertices,
            };

            //- IndexData
            meshDesc.IndexFormat = CopyIndexData(assimpMesh, meshDesc.IndexData);

            //- VertexData
            meshDesc.VertexData.resize(vertexDataSize);
            uint8* vertexDataPtr = meshDesc.VertexData.data();

            //-- Position
            std::memcpy(vertexDataPtr, assimpMesh->mVertices, positionDataSize);
            vertexDataPtr += positionDataSize;
            //-- Normal
            std::memcpy(vertexDataPtr, assimpMesh->mNormals, normalDataSize);
            vertexDataPtr += normalDataSize;
            //-- UV0
            CopyUV0Data(assimpMesh, vertexDataPtr);

            meshes.push_back(ModelMesh{
                .Mesh          = std::make_shared<Mesh>(std::move(meshDesc)),
                .MaterialIndex = assimpMesh->mMaterialIndex,
            });
        }

        return meshes;
    }


    [[nodiscard]] static std::unique_ptr<ModelNode> GetAssimpModelHierarchy(const aiNode* assimpRootNode)
    {
        auto modelRootNode = std::make_unique<ModelNode>(ModelNode{
            .Name = std::string(assimpRootNode->mName.C_Str(), assimpRootNode->mName.length),
        });

        const uint32 rootNumMeshes = assimpRootNode->mNumMeshes;
        if (rootNumMeshes != 0)
        {
            COP_LOG_WARN("Assimp root node has {:d} meshes", rootNumMeshes);
        }
        for (uint32 i = 0; i < rootNumMeshes; i++)
        {
            modelRootNode->MeshIndices.push_back(assimpRootNode->mMeshes[i]);
        }

        // TODO(v.matushkin): Sort nodes by name?
        std::queue<std::pair<ModelNode*, const aiNode*>> assimpParentNodeQueue;

        if (assimpRootNode->mNumChildren == 0)
        {
            COP_LOG_WARN("Assimp root node has 0 children");
        }
        else
        {
            assimpParentNodeQueue.push(std::make_pair(modelRootNode.get(), assimpRootNode));
        }

        while (assimpParentNodeQueue.empty() == false)
        {
            const auto [modelParentNode, assimpParentNode] = assimpParentNodeQueue.front();
            assimpParentNodeQueue.pop();

            const uint32 parentNumChildren = assimpParentNode->mNumChildren;

            for (uint32 i = 0; i < parentNumChildren; i++)
            {
                const aiNode* const assimpChildrenNode = assimpParentNode->mChildren[i];
                const uint32 childNumChildren = assimpChildrenNode->mNumChildren;
                const uint32 childNumMeshes = assimpChildrenNode->mNumMeshes;

                std::string_view childrenNodeName(assimpChildrenNode->mName.C_Str(), assimpChildrenNode->mName.length);

                if (assimpChildrenNode->mMetaData != nullptr)
                {
                    COP_LOG_WARN("Node has metadata. [{:s}] | NumProperties: {:d}", childrenNodeName, assimpChildrenNode->mMetaData->mNumProperties);
                }

                if (childNumChildren == 0 && childNumMeshes == 0)
                {
                    continue;
                }

                auto modelChildrenNode = std::make_unique<ModelNode>(ModelNode{ .Name = std::string(childrenNodeName) });

                if (childNumMeshes > 1)
                {
                    COP_LOG_WARN("Assimp node has {:d} meshes. [{:s}]", childNumMeshes, childrenNodeName);
                }
                for (uint32 j = 0; j < childNumMeshes; j++)
                {
                    modelChildrenNode->MeshIndices.push_back(assimpChildrenNode->mMeshes[j]);
                }

                if (childNumChildren != 0)
                {
                    assimpParentNodeQueue.emplace(modelChildrenNode.get(), assimpChildrenNode);
                }

                modelParentNode->Children.push_back(std::move(modelChildrenNode));
            }
        }

        return modelRootNode;
    }

} // namespace


namespace Copium
{

    ModelScene ModelImporter::Import(const std::string& modelPath)
    {
        Assimp::Importer assimpImporter;

        // TODO(v.matushkin): Learn more about aiPostProcessSteps
        const uint32 assimpPostProcessFlags =
            aiPostProcessSteps::aiProcess_Triangulate
            | aiPostProcessSteps::aiProcess_GenNormals
            // | aiPostProcessSteps::aiProcess_GenUVCoords
            //- Left/Right handed
            // | aiPostProcessSteps::aiProcess_FlipUVs          // Instead of stbi_set_flip_vertically_on_load(true); ?
            // | aiPostProcessSteps::aiProcess_FlipWindingOrder // Default is counter clockwise
            | aiPostProcessSteps::aiProcess_MakeLeftHanded
            // NOTE(v.matushkin): Why does it use an aiProcess_FlipWindingOrder flag? If it says that default is CCW and you need to use CCW in left-handed?
            // | aiProcess_ConvertToLeftHanded
            //- Optimization
            // | aiPostProcessSteps::aiProcess_JoinIdenticalVertices
            // | aiPostProcessSteps::aiProcess_ImproveCacheLocality
            //- Validation
            // | aiPostProcessSteps::aiProcess_FindDegenerates
            // | aiPostProcessSteps::aiProcess_FindInvalidData
            // | aiPostProcessSteps::aiProcess_ValidateDataStructure
            ;
        COP_ASSERT_MSG(assimpImporter.ValidateFlags(assimpPostProcessFlags), "Invalid set of Assimp PostProcess flags");

        const aiScene* const assimpScene = assimpImporter.ReadFile(modelPath, assimpPostProcessFlags);

        // TODO(v.matushkin): This will break when asserts turned off
        if (assimpScene == nullptr)
        {
            COP_LOG_CRITICAL(
                "Got an error while loading Mesh"
                "\t\nPath: {:s}"
                "\t\nAssimp error message: {:s}",
                modelPath, assimpImporter.GetErrorString());
            COP_ASSERT(false);
        }

        if (assimpScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
        {
            COP_ASSERT_MSG(false, "AI_SCENE_FLAGS_INCOMPLETE");
        }
        if (assimpScene->mFlags & AI_SCENE_FLAGS_VALIDATION_WARNING)
        {
            COP_LOG_WARN("AI_SCENE_FLAGS_VALIDATION_WARNING");
        }

        COP_LOG_INFO("Meshes: {:d} | Materials: {:d} | Embeded Textures: {:d}",
            assimpScene->mNumMeshes, assimpScene->mNumMaterials, assimpScene->mNumTextures);

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

        ModelScene modelScene = {
            .Materials = GetAssimpMaterials(modelPath, assimpScene),
            .Meshes    = GetAssimpMeshes(assimpScene->mMeshes, assimpScene->mNumMeshes),
            .RootNode  = GetAssimpModelHierarchy(assimpScene->mRootNode),
        };

        return modelScene;
    }

} // namespace Copium
