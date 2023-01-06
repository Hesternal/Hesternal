module;

#include "Hesternal/Core/Debug.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

module HesternalEditor.Assets.ModelImporter;

import Hesternal.Core;
import Hesternal.Math;

import HesternalEngine.Assets.Material;
import HesternalEngine.Assets.Mesh;
import HesternalEngine.Assets.Model;
import HesternalEngine.Assets.Texture;
import HesternalEngine.Assets.Shader;
import HesternalEngine.Graphics;
import HesternalEngine.Memory;

import HesternalEditor.Assets.AssetDatabase;

import <filesystem>;
import <memory>;
import <queue>;
import <string>;
import <string_view>;
import <unordered_map>;
import <unordered_set>;
import <utility>;

namespace fs = std::filesystem;


namespace AssimpConstants
{
    using namespace Hesternal;


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
    using namespace Hesternal;


    [[nodiscard]] static std::string ToString(const aiString& assimpString)
    {
        return std::string(assimpString.C_Str(), assimpString.length);
    }

    [[nodiscard]] static std::string_view ToStringView(const aiString& assimpString) noexcept
    {
        return std::string_view(assimpString.C_Str(), assimpString.length);
    }


    class MaterialCreationContext final
    {
    public:
        MaterialCreationContext(const std::string& modelPath, const aiScene* assimpScene)
            : m_modelDirPath(fs::path(modelPath).parent_path())
            , m_assimpMaterials(std::span<const aiMaterial* const>(assimpScene->mMaterials, assimpScene->mNumMaterials))
        {}

        [[nodiscard]] std::vector<std::shared_ptr<Material>>& GetMaterials() noexcept { return m_modelMaterials; }

        void Log() const
        {
#if COP_ENABLE_LOGGING
            const uint64 numNotLoadedAssimpMaterials = m_assimpMaterials.size() - m_assimpToModelMaterialIndex.size();
            if (numNotLoadedAssimpMaterials != 0)
            {
                COP_LOG_WARN("Not loaded assimp materials: {:d}", numNotLoadedAssimpMaterials);
            }
#endif
        }

        [[nodiscard]] uint32 AddMaterial(uint32 assimpMaterialIndex)
        {
            const uint32 newModelMaterialIndex = static_cast<uint32>(m_modelMaterials.size());

            const auto result = m_assimpToModelMaterialIndex.emplace(assimpMaterialIndex, newModelMaterialIndex);
            if (result.second == true)
            {
                m_modelMaterials.push_back(_CreateMaterial(assimpMaterialIndex));
            }

            return result.first->second;
        }

    private:
        [[nodiscard]] std::shared_ptr<Material> _CreateMaterial(uint32 assimpMaterialIndex)
        {
            const aiMaterial& assimpMaterial = *m_assimpMaterials[assimpMaterialIndex];
            const std::string_view materialName = ToStringView(assimpMaterial.GetName());

            auto material = std::make_shared<Material>(std::string(materialName), Graphics::GetDefaultShader());

            //- Get BaseColorMap
            const uint32 diffuseTexturesCount = assimpMaterial.GetTextureCount(aiTextureType::aiTextureType_DIFFUSE);
            if (diffuseTexturesCount > 0)
            {
                const std::string texturePath = _GetAssimpMaterialTexturePath(assimpMaterial, aiTextureType::aiTextureType_DIFFUSE);
                material->SetBaseColorMap(AssetDatabase::LoadAsset<Texture>(texturePath));
            
                if (diffuseTexturesCount != 1)
                {
                    COP_LOG_WARN("Material has {:d} BaseColorMaps. [{:s}]", diffuseTexturesCount, materialName);
                }
            }
            else
            {
                COP_LOG_WARN("Material doesn't have BaseColorMap, using default Black texture. [{:s}]", materialName);
                material->SetBaseColorMap(Graphics::GetBlackTexture());
            }

            //- Get NormalMap
            const uint32 normalTexturesCount = assimpMaterial.GetTextureCount(aiTextureType::aiTextureType_NORMALS);
            if (normalTexturesCount > 0)
            {
                const std::string texturePath = _GetAssimpMaterialTexturePath(assimpMaterial, aiTextureType::aiTextureType_NORMALS);
                material->SetNormalMap(AssetDatabase::LoadAsset<Texture>(texturePath));
            
                if (normalTexturesCount != 1)
                {
                    COP_LOG_WARN("Material has {:d} NormalMaps. [{:s}]", diffuseTexturesCount, materialName);
                }
            }
            else
            {
                COP_LOG_WARN("Material doesn't have NormalMap, using default Normal texture. [{:s}]", materialName);
                material->SetNormalMap(Graphics::GetNormalTexture());
            }

            return material;
        }

        [[nodiscard]] std::string _GetAssimpMaterialTexturePath(const aiMaterial& material, aiTextureType textureType)
        {
            aiString texturePath;
            // NOTE(v.matushkin): Useless method call without ASSERT
            [[maybe_unused]] const aiReturn error = material.GetTexture(textureType, 0, &texturePath);
            COP_ASSERT_MSG(error == aiReturn::aiReturn_SUCCESS, error == aiReturn_FAILURE ? "aiReturn_FAILURE" : "aiReturn_OUTOFMEMORY");
    
            // NOTE(v.matushkin): This dances with pathes is so fucking dumb
            std::string projectDirRelativePath = (m_modelDirPath / ToString(texturePath)).string();
    
            return projectDirRelativePath;
        }

    private:
        fs::path                               m_modelDirPath;
        std::span<const aiMaterial* const>     m_assimpMaterials;
        std::vector<std::shared_ptr<Material>> m_modelMaterials;
        std::unordered_map<uint32, uint32>     m_assimpToModelMaterialIndex;
    };


    class MeshCreationContext final
    {
    public:
        MeshCreationContext(const std::string& modelPath, const aiScene* assimpScene)
            : m_materialCreationContext(modelPath, assimpScene)
            , m_assimpMeshes(std::span<const aiMesh* const>(assimpScene->mMeshes, assimpScene->mNumMeshes))
        {}

        [[nodiscard]] std::vector<ModelMesh>& GetMeshes() noexcept { return m_modelMeshes; }
        [[nodiscard]] std::vector<std::shared_ptr<Material>>& GetMaterials() noexcept { return m_materialCreationContext.GetMaterials(); }

        void Log() const
        {
#if COP_ENABLE_LOGGING
            const uint64 numNotLoadedAssimpMeshes = m_assimpMeshes.size() - m_usedMeshIndices.size();
            if (numNotLoadedAssimpMeshes != 0)
            {
                COP_LOG_WARN("Not loaded assimp meshes: {:d}", numNotLoadedAssimpMeshes);
            }
            m_materialCreationContext.Log();
#endif
        }

        void AddMesh(const aiNode& assimpNode, ModelNode& modelNode)
        {
            if (assimpNode.mNumMeshes != 0)
            {
                const std::span<const uint32> subMeshIndices(assimpNode.mMeshes, assimpNode.mNumMeshes);

#if COP_ENABLE_LOGGING
                // Check that there is no mesh instancing or different nodes using the same mesh (which is kinda the same thing).
                //   Because I'm not handling this cases right now.
                for (const uint32 subMeshIndex : subMeshIndices)
                {
                    if (m_usedMeshIndices.insert(subMeshIndex).second == false)
                    {
                        COP_ASSERT_MSG(false, "Mesh was already used");
                    }
                }
#endif

                modelNode.MeshIndex = static_cast<uint32>(m_modelMeshes.size());
                // NOTE(v.matushkin): Assuming that modelNode already has a Name set
                m_modelMeshes.push_back(_CreateMesh(subMeshIndices, modelNode.Name));
            }
        }

    private:
        [[nodiscard]] ModelMesh _CreateMesh(std::span<const uint32> subMeshIndices, const std::string& meshName)
        {
            ModelMesh modelMesh;

            uint32 indexBufferSize = 0;
            uint32 vertexBufferSize = 0;

            MeshDesc meshDesc = {
                .Name     = meshName,
                .Position = VertexAttributeDesc::Position(),
                .Normal   = VertexAttributeDesc::Normal(),
                .UV0      = VertexAttributeDesc::UV0(),
            };

            for (const uint32 subMeshIndex : subMeshIndices)
            {
                const aiMesh& assimpMesh = *m_assimpMeshes[subMeshIndex];
                const uint32 subMeshNumVertices = assimpMesh.mNumVertices;

                // TODO(v.matushkin): Skip meshes that I can't handle right now.
                [[maybe_unused]] const bool isMeshValid = _IsMeshValid(assimpMesh);
                // COP_ASSERT(IsMeshValid(assimpMesh));

                modelMesh.MaterialIndices.push_back(m_materialCreationContext.AddMaterial(assimpMesh.mMaterialIndex));

                indexBufferSize = Memory::GetAlignedSize(indexBufferSize, sizeof(uint32));

                SubMeshDesc subMeshDesc;
                subMeshDesc.IndexBufferOffset = indexBufferSize;
                subMeshDesc.VertexCount = subMeshNumVertices;

                //- Index
                {
                    const uint32 subMeshNumFaces = assimpMesh.mNumFaces;
                    const uint32 subMeshNumIndices = subMeshNumFaces * AssimpConstants::IndicesPerFace;

                    subMeshDesc.IndexCount = subMeshNumIndices;

                    if (subMeshNumVertices > UInt16::Max)
                    {
                        subMeshDesc.IndexFormat = IndexFormat::UInt32;
                        indexBufferSize += subMeshDesc.IndexCount * sizeof(uint32);
                        //indexBufferSize = Memory::GetAlignedSize(indexBufferSize, sizeof(uint32));
                    }
                    else
                    {
                        subMeshDesc.IndexFormat = IndexFormat::UInt16;
                        indexBufferSize += subMeshDesc.IndexCount * sizeof(uint16);
                    }
                }
                //- Vertex
                {
                    const uint32 positionDataSize = subMeshNumVertices * AssimpConstants::PositionSize;
                    const uint32 normalDataSize   = subMeshNumVertices * AssimpConstants::NormalSize;
                    const uint32 uv0DataSize      = subMeshNumVertices * AssimpConstants::UV0Size;
                    const uint32 subMeshvertexDataSize = positionDataSize + normalDataSize + uv0DataSize;

                    subMeshDesc.PositionBufferOffset = vertexBufferSize;
                    subMeshDesc.NormalBufferOffset = subMeshDesc.PositionBufferOffset + positionDataSize;
                    subMeshDesc.UV0BufferOffset = subMeshDesc.NormalBufferOffset + normalDataSize;

                    vertexBufferSize += subMeshvertexDataSize;
                }

                meshDesc.SubMeshes.push_back(subMeshDesc);
            }

            meshDesc.IndexData.resize(indexBufferSize);
            meshDesc.VertexData.resize(vertexBufferSize);

            _CopyIndexData(subMeshIndices, meshDesc);
            _CopyVertexData(subMeshIndices, meshDesc);

            modelMesh.Mesh = std::make_shared<Mesh>(std::move(meshDesc));

            return modelMesh;
        }

        void _CopyIndexData(std::span<const uint32> subMeshIndices, MeshDesc& meshDesc)
        {
            static const auto CopyData = []<typename T>(const aiFace* assimpMeshFaces, uint32 assimpMeshNumFaces, uint8* subMeshIndexDataPtr)
            {
                for (uint32 j = 0; j < assimpMeshNumFaces; j++)
                {
                    const aiFace assimpMeshFace = assimpMeshFaces[j];
                    const T indices[3] = {
                        static_cast<T>(assimpMeshFace.mIndices[0]),
                        static_cast<T>(assimpMeshFace.mIndices[1]),
                        static_cast<T>(assimpMeshFace.mIndices[2]),
                    };

                    std::memcpy(subMeshIndexDataPtr, indices, sizeof(indices));
                    subMeshIndexDataPtr += sizeof(indices);
                }
            };

            const uint32 numSubMeshes = static_cast<uint32>(subMeshIndices.size());
            uint8* const indexDataPtr = meshDesc.IndexData.data();

            for (uint32 i = 0; i < numSubMeshes; i++)
            {
                const aiMesh& assimpMesh = *m_assimpMeshes[subMeshIndices[i]];
                const SubMeshDesc& subMeshDesc = meshDesc.SubMeshes[i];

                const uint32 assimpMeshNumFaces = assimpMesh.mNumFaces;
                const aiFace* const assimpMeshFaces = assimpMesh.mFaces;
                uint8* subMeshIndexDataPtr = indexDataPtr + subMeshDesc.IndexBufferOffset;

                if (subMeshDesc.IndexFormat == IndexFormat::UInt32)
                {
                    CopyData.template operator()<uint32>(assimpMeshFaces, assimpMeshNumFaces, subMeshIndexDataPtr);
                }
                else
                {
                    CopyData.template operator()<uint16>(assimpMeshFaces, assimpMeshNumFaces, subMeshIndexDataPtr);
                }
            }
        }

        void _CopyVertexData(std::span<const uint32> subMeshIndices, MeshDesc& meshDesc)
        {
            const uint32 numSubMeshes = static_cast<uint32>(subMeshIndices.size());
            uint8* const vertexDataPtr = meshDesc.VertexData.data();

            for (uint32 i = 0; i < numSubMeshes; i++)
            {
                const aiMesh* const assimpMesh = m_assimpMeshes[subMeshIndices[i]];
                const SubMeshDesc& subMeshDesc = meshDesc.SubMeshes[i];

                const uint32 numVertices = assimpMesh->mNumVertices;
                const uint32 positionDataSize = numVertices * AssimpConstants::PositionSize;
                const uint32 normalDataSize   = numVertices * AssimpConstants::NormalSize;

                uint8* subMeshIndexDataPtr = vertexDataPtr + subMeshDesc.PositionBufferOffset;

                //- Position
                std::memcpy(subMeshIndexDataPtr, assimpMesh->mVertices, positionDataSize);
                subMeshIndexDataPtr += positionDataSize;
                //- Normal
                std::memcpy(subMeshIndexDataPtr, assimpMesh->mNormals, normalDataSize);
                subMeshIndexDataPtr += normalDataSize;
                //- UV0
                _CopyUVChannel(std::span<const aiVector3D>(assimpMesh->mTextureCoords[0], numVertices), subMeshIndexDataPtr);
            }
        }

        [[nodiscard]] static bool _IsMeshValid(const aiMesh& assimpMesh)
        {
            bool valid = true;

#if COP_ENABLE_LOGGING
            const std::string_view meshName = ToStringView(assimpMesh.mName);
#endif

            if (assimpMesh.mPrimitiveTypes != aiPrimitiveType::aiPrimitiveType_TRIANGLE)
            {
                COP_LOG_WARN("Mesh topology is not triangle. [{:s}]", meshName);
                valid = false;
            }
            if (assimpMesh.HasFaces() == false)
            {
                COP_LOG_WARN("Mesh doesn't have Faces. [{:s}]", meshName);
                valid = false;
            }
            if (assimpMesh.HasPositions() == false)
            {
                COP_LOG_WARN("Mesh doesn't have Positions. [{:s}]", meshName);
                valid = false;
            }
            if (assimpMesh.HasNormals() == false)
            {
                COP_LOG_WARN("Mesh doesn't have Normals. [{:s}]", meshName);
                valid = false;
            }
            if (assimpMesh.HasTextureCoords(0) == false)
            {
                COP_LOG_WARN("Mesh doesn't have UV0. [{:s}]", meshName);
                valid = false;
            }

            return valid;
        }

        static void _CopyUVChannel(std::span<const aiVector3D> uvChannel, uint8* vertexDataPtr)
        {
            for (const aiVector3D vertexUV : uvChannel)
            {
                const float32 uv[2] = { vertexUV.x, vertexUV.y };

                std::memcpy(vertexDataPtr, uv, sizeof(uv));
                vertexDataPtr += sizeof(uv);
            }
        }

    private:
        MaterialCreationContext        m_materialCreationContext;
        std::span<const aiMesh* const> m_assimpMeshes;
        std::vector<ModelMesh>         m_modelMeshes;
#if COP_ENABLE_LOGGING
        std::unordered_set<uint32>     m_usedMeshIndices;
#endif
    };


    [[nodiscard]] static std::unique_ptr<ModelNode> GetAssimpModelHierarchy(const aiNode& assimpRootNode, MeshCreationContext& meshCreationContext)
    {
        static const auto DecomposeNodeTransform = [](const aiNode& assimpNode, ModelNode& modelNode)
        {
            // TODO(v.matushkin): Add Decompose method to Hesternal.Math?
            // NOTE(v.matushkin): Fix negative zeroes?
            aiVector3D position;
            aiQuaternion rotation;
            aiVector3D scale;
            assimpNode.mTransformation.Decompose(scale, rotation, position);

            if ((Math::IsNearlyEqual(scale.x, scale.y) && Math::IsNearlyEqual(scale.x, scale.z)) == false)
            {
                COP_LOG_WARN("Node has a non-uniform scale.");
            }

            modelNode.Position = Float3(position.x, position.y, position.z);
            modelNode.Rotation = Quaternion(rotation.x, rotation.y, rotation.z, rotation.w);
            modelNode.Scale = scale.x;
        };

        auto modelRootNode = std::make_unique<ModelNode>(ModelNode{ .Name = ToString(assimpRootNode.mName) });
        DecomposeNodeTransform(assimpRootNode, *modelRootNode);
        meshCreationContext.AddMesh(assimpRootNode, *modelRootNode);

        // TODO(v.matushkin): Sort nodes by name?
        std::queue<std::pair<ModelNode*, const aiNode*>> assimpParentNodeQueue;

        if (assimpRootNode.mNumChildren == 0)
        {
            COP_LOG_WARN("Assimp root node has 0 children");
        }
        else
        {
            assimpParentNodeQueue.push(std::make_pair(modelRootNode.get(), &assimpRootNode));
        }

        while (assimpParentNodeQueue.empty() == false)
        {
            const auto [modelParentNode, assimpParentNode] = assimpParentNodeQueue.front();
            assimpParentNodeQueue.pop();

            const uint32 parentNumChildren = assimpParentNode->mNumChildren;

            for (uint32 i = 0; i < parentNumChildren; i++)
            {
                const aiNode& assimpChildrenNode = *(assimpParentNode->mChildren[i]);
                const uint32 childNumChildren = assimpChildrenNode.mNumChildren;
                const uint32 childNumMeshes = assimpChildrenNode.mNumMeshes;
                const std::string_view childrenNodeName = ToStringView(assimpChildrenNode.mName);

                if (childNumChildren == 0 && childNumMeshes == 0)
                {
                    COP_LOG_WARN("Skipping a node without children and meshes. [{:s}]", childrenNodeName);
                    continue;
                }
                // if (assimpChildrenNode->mMetaData != nullptr)
                // {
                //     COP_LOG_WARN("Node has metadata. [{:s}] | NumProperties: {:d}", childrenNodeName, assimpChildrenNode->mMetaData->mNumProperties);
                // }

                auto modelChildrenNode = std::make_unique<ModelNode>(ModelNode{ .Name = std::string(childrenNodeName) });
                DecomposeNodeTransform(assimpChildrenNode, *modelChildrenNode);
                meshCreationContext.AddMesh(assimpChildrenNode, *modelChildrenNode);

                if (childNumChildren != 0)
                {
                    assimpParentNodeQueue.emplace(modelChildrenNode.get(), &assimpChildrenNode);
                }

                modelParentNode->Children.push_back(std::move(modelChildrenNode));
            }
        }

        return modelRootNode;
    }

} // namespace


namespace Hesternal
{

    ModelScene ModelImporter::Import(const std::string& modelPath)
    {
        Assimp::Importer assimpImporter;

        //- Load scene
        const aiScene* const assimpScene = assimpImporter.ReadFile(modelPath, 0);
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

        //- NOTE(v.matushkin): Hack for Sponza model
        const ai_real assimpModelGlobalScale = assimpImporter.GetPropertyFloat(AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, FloatingType<ai_real>::Infinity);
        if (assimpModelGlobalScale == FloatingType<ai_real>::Infinity)
        {
            COP_LOG_DEBUG("Applying global scale");
            assimpImporter.SetPropertyFloat(AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, static_cast<ai_real>(0.01));
        }

        //- Apply PostProcessing
        // TODO(v.matushkin): Learn more about aiPostProcessSteps
        constexpr uint32 assimpPostProcessFlags = 0
            | aiPostProcessSteps::aiProcess_Triangulate
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
            // | aiPostProcessSteps::aiProcess_OptimizeGraph
            //- Validation
            // | aiPostProcessSteps::aiProcess_FindDegenerates
            // | aiPostProcessSteps::aiProcess_FindInvalidData
            // | aiPostProcessSteps::aiProcess_ValidateDataStructure
            //
            | aiPostProcessSteps::aiProcess_GlobalScale
            ;
        COP_ASSERT_MSG(assimpImporter.ValidateFlags(assimpPostProcessFlags), "Invalid set of Assimp PostProcess flags");
        // This returns the same pointer, and can only be nullptr if aiProcess_ValidateDataStructure was used
        assimpImporter.ApplyPostProcessing(assimpPostProcessFlags);

        if (assimpScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
        {
            COP_ASSERT_MSG(false, "AI_SCENE_FLAGS_INCOMPLETE");
        }
        // This flag can be set only if aiProcess_ValidateDataStructure was used
        if (assimpScene->mFlags & AI_SCENE_FLAGS_VALIDATION_WARNING)
        {
            COP_LOG_WARN("AI_SCENE_FLAGS_VALIDATION_WARNING");
        }

        if (assimpScene->HasMaterials() == false)
        {
            COP_LOG_WARN("Model has no materials");
        }

        COP_LOG_TRACE("Meshes: {:d} | Materials: {:d} | Embeded Textures: {:d}",
            assimpScene->mNumMeshes, assimpScene->mNumMaterials, assimpScene->mNumTextures);

        if (assimpScene->HasAnimations())
        {
            COP_LOG_TRACE("Model has {:d} animations", assimpScene->mNumAnimations);
        }
        if (assimpScene->HasCameras())
        {
            COP_LOG_TRACE("Model has {:d} cameras", assimpScene->mNumCameras);
        }
        if (assimpScene->HasLights())
        {
            COP_LOG_TRACE("Model has {:d} lights", assimpScene->mNumLights);
        }

        MeshCreationContext meshCreationContext(modelPath, assimpScene);
        std::unique_ptr<ModelNode> rootNode = GetAssimpModelHierarchy(*assimpScene->mRootNode, meshCreationContext);

        meshCreationContext.Log();

        return ModelScene{
            .Materials = std::move(meshCreationContext.GetMaterials()),
            .Meshes    = std::move(meshCreationContext.GetMeshes()),
            .RootNode  = std::move(rootNode),
        };
    }

} // namespace Hesternal
