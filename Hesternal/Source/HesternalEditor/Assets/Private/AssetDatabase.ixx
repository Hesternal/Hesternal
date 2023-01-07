export module HesternalEditor.Assets.AssetDatabase;

import HesternalEngine.Assets.Model;
import HesternalEngine.Assets.Shader;
import HesternalEngine.Assets.Texture;
import HesternalEngine.Core.Object;

import <filesystem>;
import <memory>;
import <string>;
import <unordered_map>;


export namespace Hesternal
{

    class AssetDatabase final
    {
        // struct AssetInfo final
        // {
        //     std::string Path;
        //     Guid        Guid;
        //     ClassID     Type;
        // };

    public:
        static void Init(const std::filesystem::path& projectPath, const std::filesystem::path& shaderDirPath);
        static void Shutdown();

        // /// Returns zero Guid if assetPath is invalid
        // [[nodiscard]] static Guid AssetPathToGuid(const std::string& assetPath);
        // /// Returns empty string if assetGuid is invalid
        // [[nodiscard]] static std::string AssetGuidToPath(const Guid& assetGuid);

        // TODO(v.matushkin): ModelScene doesn't inherit Object so had to remove concept, bring it back
        template<typename T>
        [[nodiscard]] static std::shared_ptr<T> LoadAsset(const std::string& assetPath);

    private:
        // static inline std::unordered_map<std::string, AssetInfo> m_pathToAssetInfo;
        // static inline std::unordered_map<Guid, std::string>      m_guidToPath;
        // static inline std::unordered_map<Guid, std::string> m_guidToArtifactPath;

        static inline std::filesystem::path m_shaderDirPath;

        static inline std::unordered_map<std::string, std::shared_ptr<ModelScene>> m_models;
        static inline std::unordered_map<std::string, std::shared_ptr<Shader>>     m_shaders;
        static inline std::unordered_map<std::string, std::shared_ptr<Texture>>    m_textures;
    };

} // export namespace Hesternal
