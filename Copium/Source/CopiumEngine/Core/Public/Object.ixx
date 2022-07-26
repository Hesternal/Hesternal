module;

#include "Engine/Core/Defines.hpp"
// #include "EngineCht/Object.cht.hpp"

#include <string>
#include <type_traits>

export module CopiumEngine.Core.Object;

import CopiumEngine.Core.ClassID;
import CopiumEngine.Core.CoreTypes;
import CopiumEngine.Core.ForwardDeclaration;


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


    template<typename T>
    concept CObject = std::derived_from<T, Object>;

} // export namespace Copium
