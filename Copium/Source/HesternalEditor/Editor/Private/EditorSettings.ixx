export module HesternalEditor.Editor.EditorSettings;

import Hesternal.Core;

import <string>;


export namespace Hesternal
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

} // export namespace Hesternal
