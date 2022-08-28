export module CopiumEngine.Core.Platform;

import CopiumEngine.Core.ForwardDeclaration;

import <string>;
import <string_view>;


// NOTE(v.matushkin): Module and namespace naming is shit
export namespace Copium::Platform
{

    void GenerateGuid(Guid& guid);

    [[nodiscard]] std::wstring ToWideString(std::string_view str);

} // export namespace Copium
