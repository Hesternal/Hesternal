export module HesternalEditor.Editor.GraphicsComponentGui;

import HesternalEngine.ECS.Components;

import HesternalEditor.GUI.ComponentGui;
import HesternalEditor.GUI.EditorGui;


export namespace Hesternal
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


    class DirectionalLightGui : public ComponentGui
    {
    public:
        DirectionalLightGui(DirectionalLight* directionalLight)
            : ComponentGui("DirectionalLight")
            , m_directionalLight(directionalLight)
        {
        }

        ~DirectionalLightGui() = default;

        DirectionalLightGui(DirectionalLightGui&& other) noexcept = default;
        DirectionalLightGui& operator=(DirectionalLightGui&& other) noexcept = default;

    protected:
        void OnComponentGui() override
        {
            EditorGui::ColorField("Color", m_directionalLight->Color, false);
        }

    private:
        DirectionalLight* m_directionalLight;
    };


    class PointLightGui : public ComponentGui
    {
    public:
        PointLightGui(PointLight* pointLight)
            : ComponentGui("PointLight")
            , m_pointLight(pointLight)
        {
        }

        ~PointLightGui() = default;

        PointLightGui(PointLightGui&& other) noexcept = default;
        PointLightGui& operator=(PointLightGui&& other) noexcept = default;

    protected:
        void OnComponentGui() override
        {
            EditorGui::ColorField("Color", m_pointLight->Color, false);
            EditorGui::FloatField("Intensity", m_pointLight->Intensity);
            EditorGui::FloatField("Range", m_pointLight->Range);
        }

    private:
        PointLight* m_pointLight;
    };

    class SpotLightGui : public ComponentGui
    {
    public:
        SpotLightGui(SpotLight* spotLight)
            : ComponentGui("SpotLight")
            , m_spotLight(spotLight)
        {
        }

        ~SpotLightGui() = default;

        SpotLightGui(SpotLightGui&& other) noexcept = default;
        SpotLightGui& operator=(SpotLightGui&& other) noexcept = default;

    protected:
        void OnComponentGui() override
        {
            EditorGui::ColorField("Color", m_spotLight->Color, false);
            EditorGui::FloatField("Intensity", m_spotLight->Intensity);
            EditorGui::FloatField("Range", m_spotLight->Range);
            EditorGui::FloatField("SpotAngle", m_spotLight->SpotAngle);
            EditorGui::FloatField("InnerSpotPercent", m_spotLight->InnerSpotPercent);
        }

    private:
        SpotLight* m_spotLight;
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
            EditorGui::Label("MetallicMap", material->GetMetallicMap()->GetName());
            EditorGui::Label("RoughnessMap", material->GetRoughnessMap()->GetName());
            EditorGui::Label("NormalMap", material->GetNormalMap()->GetName());
            EditorGui::Unindent();
        }

    private:
        RenderMesh* m_renderMesh;
    };

} // export namespace Hesternal
