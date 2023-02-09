module;

#include "Hesternal/Core/Debug.hpp"

module HesternalEditor.Assets.AssetDatabase;

import HesternalEngine.Core.Object;

import HesternalEditor.Assets.ModelImporter;
import HesternalEditor.Assets.ShaderImporter;
import HesternalEditor.Assets.TextureImporter;

import <filesystem>;
import <vector>;
import <utility>;

namespace fs = std::filesystem;


namespace Hesternal
{

    void AssetDatabase::Init(const std::filesystem::path& projectPath, const std::filesystem::path& shaderDirPath)
    {
        fs::current_path(projectPath);

        // TODO(v.matushkin): I shouldn't handle shader paths like this, but there is no other way right now.
        m_shaderDirPath = shaderDirPath / "dx";
        m_shaderIncludeDirPath = m_shaderDirPath / "include";

        // std::vector<fs::path> assetsToImport;

        //- Get assets Guid/Path
        // for (const fs::directory_entry& dirEntry : fs::recursive_directory_iterator("Assets"))
        // {
        //     if (fs::is_regular_file(dirEntry))
        //     {
        //         fs::path assetPath = dirEntry.path();
        //         std::string assetPathStr = assetPath.string();
        //         std::string assetExtension = assetPath.extension().string();
        //         HS_LOG_INFO(assetPathStr);
        // 
        //         Guid assetGuid = Guid::Generate();
        // 
        //         m_guidToPath.emplace(assetGuid, assetPathStr);
        // 
        //         AssetInfo assetInfo = {
        //             .Path = std::move(assetPathStr),
        //             .Guid = assetGuid,
        //         };
        // 
        //         if (assetExtension == ".tga")
        //         {
        //             assetInfo.Type = ClassID::Texture2D;
        //         }
        //         else if (assetExtension == ".obj")
        //         {
        //             assetInfo.Type = ClassID::Mesh;
        //         }
        // 
        //         m_pathToAssetInfo.emplace(std::move(assetPathStr), std::move(assetInfo));
        // 
        //         // assetsToImport.push_back(std::move(assetPath));
        //     }
        // }

        // const std::string artifactsDirPath = "Cache/Artifacts/";

        //- Import assets
        // for (const fs::path& assetPath : assetsToImport)
        // {
        //     std::string assetPathStr = assetPath.string();
        //     std::string assetName = assetPath.filename().string();
        //     std::string assetExtension = assetPath.extension().string();
        // 
        //     if (assetExtension == ".tga")
        //     {
        //         Texture textureDesc = TextureImporter::Import(assetPathStr);
        // 
        //         Guid assetGuid = m_pathToGuid.find(assetPathStr)->second;
        //         std::string artifactPath = artifactsDirPath + assetName;
        //         {
        //             FileBinarySerializer serializer(artifactPath);
        //             serializer << textureDesc;
        //         }
        // 
        //         m_guidToArtifactPath.emplace(assetGuid, std::move(artifactPath));
        //     }
        //     else if (assetExtension == ".obj")
        //     {
        //         ModelAsset modelAsset = ModelImporter::Import(assetPathStr);
        // 
        //         Guid assetGuid = m_pathToGuid.find(assetPathStr)->second;
        //         std::string artifactPath = artifactsDirPath + assetName;
        //         {
        //             FileBinarySerializer serializer(artifactPath);
        //             serializer << modelAsset;
        //         }
        // 
        //         m_guidToArtifactPath.emplace(assetGuid, std::move(artifactPath));
        //     }
        // }
    }

    void AssetDatabase::Shutdown()
    {
        m_models.clear();
        m_shaders.clear();
        m_textures.clear();
    }


    // Guid AssetDatabase::AssetPathToGuid(const std::string& assetPath)
    // {
    //     if (auto mapIterator = m_pathToAssetInfo.find(assetPath); mapIterator != m_pathToAssetInfo.end())
    //     {
    //         return mapIterator->second.Guid;
    //     }
    // 
    //     // TODO(v.matushkin): Need something like Guid.Zero();
    //     return Guid{ 0, 0, 0, 0 };
    // }
    // 
    // std::string AssetDatabase::AssetGuidToPath(const Guid& assetGuid)
    // {
    //     if (auto mapIterator = m_guidToPath.find(assetGuid); mapIterator != m_guidToPath.end())
    //     {
    //         return mapIterator->second;
    //     }
    // 
    //     return std::string();
    // }


    template<>
    std::shared_ptr<ModelScene> AssetDatabase::LoadAsset(const std::string& assetPath)
    {
        auto mapIterator = m_models.find(assetPath);
        if (mapIterator == m_models.end())
        {
            mapIterator = m_models.emplace(assetPath, std::make_shared<ModelScene>(ModelImporter::Import(assetPath))).first;
        }

        return mapIterator->second;
    }

    template<>
    std::shared_ptr<Shader> AssetDatabase::LoadAsset(const std::string& assetPath)
    {
        auto mapIterator = m_shaders.find(assetPath);
        if (mapIterator == m_shaders.end())
        {
            const std::string actualShaderPath = (m_shaderDirPath / assetPath).string();
            mapIterator = m_shaders.emplace(assetPath, std::make_shared<Shader>(ShaderImporter::Import(&m_shaderIncludeDirPath, actualShaderPath))).first;
        }

        return mapIterator->second;
    }

    template<>
    std::shared_ptr<Texture> AssetDatabase::LoadAsset(const std::string& assetPath)
    {
        auto mapIterator = m_textures.find(assetPath);
        if (mapIterator == m_textures.end())
        {
            mapIterator = m_textures.emplace(assetPath, std::make_shared<Texture>(TextureImporter::Import(assetPath))).first;
        }

        return mapIterator->second;
    }

} // namespace Hesternal
