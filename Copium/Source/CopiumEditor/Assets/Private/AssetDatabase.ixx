module;

#include <filesystem>
#include <string>
#include <unordered_map>

export module CopiumEditor.Assets.AssetDatabase;

import CopiumEngine.Core.Object;
import CopiumEngine.Core.Misc.Guid;


export namespace Copium
{

    class AssetDatabase final
    {
    public:
        static void Init(const std::filesystem::path& projectPath);

        /// Returns zero Guid if assetPath is invalid
        [[nodiscard]] static Guid AssetPathToGuid(const std::string& assetPath);
        /// Returns empty string if assetGuid is invalid
        [[nodiscard]] static std::string AssetGuidToPath(const Guid& assetGuid);

    private:
        static inline std::unordered_map<Guid, std::string> m_guidToPath;
        static inline std::unordered_map<std::string, Guid> m_pathToGuid;

        static inline std::unordered_map<Guid, std::string> m_guidToArtifactPath;
    };

} // export namespace Copium
