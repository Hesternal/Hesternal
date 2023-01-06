export module CopiumEditor.Editor.EditorSettings;

import Copium.Core;

import <string>;


export namespace Copium
{

    struct EditorSettings final
    {
        std::wstring WindowTitle;
        uint16       WindowWidth;
        uint16       WindowHeight;


        [[nodiscard]] static EditorSettings& Get()
        {
            static EditorSettings settings;
            return settings;
        }
    };

} // export namespace Copium
