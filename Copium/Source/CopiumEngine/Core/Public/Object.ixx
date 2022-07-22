module;

#include <string>
#include <type_traits>

export module CopiumEngine.Core.Object;


export namespace Copium
{

    class Object
    {
    public:
        Object() = default;
        virtual ~Object() = default;

        Object(const Object&) = delete;
        Object& operator=(const Object&) = delete;

        [[nodiscard]] virtual std::string GetName() const = 0;
    };


    template<typename T>
    concept CObject = std::derived_from<T, Object>;

} // export namespace Copium
