export module HesternalEditor.Assets.ShaderImporter.ParserError;

import Hesternal.Core;

import <string>;
import <utility>;


export namespace Hesternal
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

} // export namespace Hesternal
