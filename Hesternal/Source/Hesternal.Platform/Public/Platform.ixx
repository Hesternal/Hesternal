export module Hesternal.Platform;

export import :Guid;
export import :File;

import <string>;
import <string_view>;


export namespace Hesternal::Platform
{

    [[nodiscard]] std::wstring ToWideString(std::string_view str);

} // export namespace Hesternal::Platform
