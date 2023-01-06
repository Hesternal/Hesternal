module CopiumEngine.Core.Serialization.ObjectSerializationContext;

import CopiumEngine.Core.Object;


namespace Copium
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

} // namespace Copium
