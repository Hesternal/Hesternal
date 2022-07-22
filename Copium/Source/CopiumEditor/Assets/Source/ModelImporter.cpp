module;

#include "Engine/Core/Debug.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <memory>
#include <string>

module CopiumEditor.Assets.ModelImporter;

import CopiumEngine.Assets.NativeAsset;
import CopiumEngine.Core.CoreTypes;


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


namespace Copium
{

    ModelAsset ModelImporter::Import(const std::string& meshPath)
    {
        Assimp::Importer assimpImporter;
        // TODO(v.matushkin): Learn more about aiPostProcessSteps
        const aiScene* assimpScene = assimpImporter.ReadFile(
            meshPath,
            aiPostProcessSteps::aiProcess_Triangulate
            | aiPostProcessSteps::aiProcess_GenNormals
            // | aiPostProcessSteps::aiProcess_FlipUVs          // Instead of stbi_set_flip_vertically_on_load(true); ?
            // | aiPostProcessSteps::aiProcess_FlipWindingOrder // Default is counter clockwise
        );

        // TODO(v.matushkin): This will break when asserts turned off
        if (assimpScene == nullptr)
        {
            COP_LOG_CRITICAL(
                "Got an error while loading Mesh"
                "\t\nPath: {0}"
                "\t\nAssimp error message: {1}",
                meshPath, assimpImporter.GetErrorString());
            COP_ASSERT(false);
        }

        ModelAsset modelAsset;

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

            MeshDesc meshDesc = {
                .Name        = std::string(assimpMesh->mName.C_Str(), assimpMesh->mName.length),
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

            modelAsset.Meshes.push_back(std::move(meshDesc));
        }

        return modelAsset;
    }

} // namespace Copium
