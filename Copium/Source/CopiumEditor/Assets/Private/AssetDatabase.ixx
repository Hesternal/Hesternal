export module CopiumEditor.Assets.AssetDatabase;

import CopiumEngine.Assets.Model;
import CopiumEngine.Assets.Texture;
import CopiumEngine.Core.Object;

import <filesystem>;
import <memory>;
import <string>;
import <unordered_map>;


export namespace Copium
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
        static void Init(const std::filesystem::path& projectPath);
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

        static inline std::unordered_map<std::string, std::shared_ptr<Texture>>    m_textures;
        static inline std::unordered_map<std::string, std::shared_ptr<ModelScene>> m_models;
    };

} // export namespace Copium
