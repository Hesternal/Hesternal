export module HesternalEngine.Core.Platform;

import HesternalEngine.Core.ForwardDeclaration;

import <string>;
import <string_view>;


// NOTE(v.matushkin): Module and namespace naming is shit
export namespace Hesternal::Platform
{

    void GenerateGuid(Guid& guid);

    [[nodiscard]] std::wstring ToWideString(std::string_view str);

} // export namespace Hesternal
