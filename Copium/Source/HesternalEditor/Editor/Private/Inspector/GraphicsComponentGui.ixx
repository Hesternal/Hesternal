export module CopiumEditor.Editor.GraphicsComponentGui;

import CopiumEngine.ECS.Components;

import CopiumEditor.GUI.ComponentGui;
import CopiumEditor.GUI.EditorGui;


export namespace Copium
{

    class CameraGui : public ComponentGui
    {
    public:
        CameraGui(Camera* camera)
            : ComponentGui("Camera")
            , m_camera(camera)
        {
        }

        ~CameraGui() = default;

        CameraGui(CameraGui&& other) noexcept = default;
        CameraGui& operator=(CameraGui&& other) noexcept = default;

    protected:
        void OnComponentGui() override
        {
            EditorGui::MatrixField(m_camera->Projection);
        }

    private:
        Camera* m_camera;
    };


    class RenderMeshGui : public ComponentGui
    {
    public:
        RenderMeshGui(RenderMesh* renderMesh)
            : ComponentGui("RenderMesh")
            , m_renderMesh(renderMesh)
        {
        }

        ~RenderMeshGui() = default;

        RenderMeshGui(RenderMeshGui&& other) noexcept = default;
        RenderMeshGui& operator=(RenderMeshGui&& other) noexcept = default;

    protected:
        void OnComponentGui() override
        {
            EditorGui::Label("Mesh", m_renderMesh->Mesh->GetName());

            const Material* material = m_renderMesh->Material.get();
            EditorGui::Label("Material", material->GetName());
            EditorGui::Indent();
            EditorGui::Label("BaseColorMap", material->GetBaseColorMap()->GetName());
            EditorGui::Label("NormalMap", material->GetNormalMap()->GetName());
            EditorGui::Unindent();
        }

    private:
        RenderMesh* m_renderMesh;
    };

} // export namespace Copium
