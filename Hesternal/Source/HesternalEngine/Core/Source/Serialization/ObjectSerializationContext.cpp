module HesternalEngine.Core.Serialization.ObjectSerializationContext;

import HesternalEngine.Core.Object;


namespace Hesternal
{

    ObjectSerializationContext::ObjectSerializationContext(bool initAfterDeserialize)
        : m_initAfterDeserialize(initAfterDeserialize)
    {
    }

    void ObjectSerializationContext::AddObject(Object* object)
    {
        m_objects.push_back(object);
    }

    void ObjectSerializationContext::EndConvertion()
    {
        if (m_initAfterDeserialize == false)
        {
            return;
        }

        for (Object* object : m_objects)
        {
            object->OnAfterDeserealize();
        }
    }

} // namespace Hesternal
