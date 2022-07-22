module;

#include "Engine/Core/Debug.hpp"

#include <filesystem>
#include <utility>
#include <vector>

module CopiumEditor.Assets.AssetDatabase;

import CopiumEngine.Assets.NativeAsset;
import CopiumEngine.Core.Serialization.FileBinaryConverter;
import CopiumEngine.Graphics.GraphicsTypes;

import CopiumEditor.Assets.ModelImporter;
import CopiumEditor.Assets.TextureImporter;

namespace fs = std::filesystem;


namespace Copium
{

    void AssetDatabase::Init(const std::filesystem::path& projectPath)
    {
        fs::current_path(projectPath);

        std::vector<fs::path> assetsToImport;

        //- Get assets Guid/Path
        for (const fs::directory_entry& dirEntry : fs::recursive_directory_iterator("Assets"))
        {
            if (fs::is_regular_file(dirEntry))
            {
                fs::path assetPath = dirEntry.path();

                std::string assetPathStr = assetPath.string();
                COP_LOG_INFO(assetPathStr);

                Guid assetGuid = Guid::Generate();
                m_guidToPath.emplace(assetGuid, assetPathStr);
                m_pathToGuid.emplace(std::move(assetPathStr), assetGuid);

                assetsToImport.push_back(std::move(assetPath));
            }
        }

        const std::string artifactsDirPath = "Cache/Artifacts/";

        //- Import assets
        for (const fs::path& assetPath : assetsToImport)
        {
            std::string assetPathStr = assetPath.string();
            std::string assetName = assetPath.filename().string();
            std::string assetExtension = assetPath.extension().string();

            if (assetExtension == ".tga")
            {
                TextureDesc textureDesc = TextureImporter::Import(assetPathStr);

                Guid assetGuid = m_pathToGuid.find(assetPathStr)->second;
                std::string artifactPath = artifactsDirPath + assetName;
                {
                    FileBinarySerializer serializer(artifactPath);
                    serializer << textureDesc;
                }

                m_guidToArtifactPath.emplace(assetGuid, std::move(artifactPath));
            }
            else if (assetExtension == ".obj")
            {
                ModelAsset modelAsset = ModelImporter::Import(assetPathStr);

                Guid assetGuid = m_pathToGuid.find(assetPathStr)->second;
                std::string artifactPath = artifactsDirPath + assetName;
                {
                    FileBinarySerializer serializer(artifactPath);
                    serializer << modelAsset;
                }

                m_guidToArtifactPath.emplace(assetGuid, std::move(artifactPath));
            }
        }
    }


    Guid AssetDatabase::AssetPathToGuid(const std::string& assetPath)
    {
        if (auto mapIterator = m_pathToGuid.find(assetPath); mapIterator != m_pathToGuid.end())
        {
            return mapIterator->second;
        }

        // TODO(v.matushkin): Need something like Guid.Zero();
        return Guid{ 0, 0, 0, 0 };
    }

    std::string AssetDatabase::AssetGuidToPath(const Guid& assetGuid)
    {
        if (auto mapIterator = m_guidToPath.find(assetGuid); mapIterator != m_guidToPath.end())
        {
            return mapIterator->second;
        }

        return std::string();
    }

} // namespace Copium
