export module HesternalEngine.Core.Serialization.ObjectSerializationContext;

import HesternalEngine.Core.ForwardDeclaration;

import <vector>;


export namespace Hesternal
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

} // export namespace Hesternal
