module;

#include <vector>

export module CopiumEngine.Core.Serialization.ObjectSerializationContext;

import CopiumEngine.Core.ForwardDeclaration;


export namespace Copium
{

    class ObjectSerializationContext final
    {
    public:
        ObjectSerializationContext(bool initAfterDeserialize);

        void AddObject(Object* object);
        void EndConvertion();

    private:
        std::vector<Object*> m_objects;

        const bool m_initAfterDeserialize;
    };

} // export namespace Copium
