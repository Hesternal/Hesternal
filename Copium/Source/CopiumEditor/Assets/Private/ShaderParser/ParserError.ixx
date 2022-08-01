export module CopiumEditor.Assets.ShaderImporter.ParserError;

import CopiumEngine.Core.CoreTypes;

import <string>;
import <utility>;


export namespace Copium
{

    class ParserError final
    {
    public:
        ParserError(std::string&& message)
            : Message(std::move(message))
        {
        }

    public:
        const std::string Message;
    };

} // export namespace Copium
