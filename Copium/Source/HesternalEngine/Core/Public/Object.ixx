module;

#include "Copium/Core/Defines.hpp"
// #include "EngineCht/Object.cht.hpp"

export module CopiumEngine.Core.Object;

import Copium.Core;
import CopiumEngine.Core.ClassID;
import CopiumEngine.Core.ForwardDeclaration;

// import <concepts>;
import <string>;


export namespace Copium
{

    enum class DoNotInitialize : uint8 {};


    // CHT_CLASS(abstract)
    class Object
    {
        // CHT_GENERATED_BODY()

        friend IBinaryConverter; // Convert()
        friend ObjectSerializationContext; // OnAfterDeserealize()

    public:
        Object() = default;
        virtual ~Object() = default;

        Object(const Object&) = delete;
        Object& operator=(const Object&) = delete;

        [[nodiscard]] virtual const std::string& GetName() const = 0;

    protected:
        [[nodiscard]] virtual ClassID GetClassID() const = 0;
        virtual void Convert(IBinaryConverter& bc) = 0;
        virtual void OnAfterDeserealize() = 0;
    };


    // NOTE(v.matushkin):
    //  CopiumEngine\Core\Public\Object.ixx(47,5): fatal  error C1001: Internal compiler error.
    //  compiler file 'D:\a\_work\1\s\src\vctools\Compiler\CxxFE\sl\p1\c\module\unit.h', line 816)
    // template<typename T>
    // concept CObject = std::derived_from<T, Object>;

} // export namespace Copium
