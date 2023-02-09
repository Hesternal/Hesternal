export module HesternalEditor.Editor.GraphicsComponentGui;

import Hesternal.Core;

import HesternalEngine.ECS.Components;

import HesternalEditor.GUI.ComponentGui;
import HesternalEditor.GUI.EditorGui;


namespace
{
    using namespace Hesternal;


    inline constexpr float32 k_PunctualLightIntensityMin = 0.0f;
    inline constexpr float32 k_PunctualLightRangeMin     = 0.001f;
    inline constexpr float32 k_SpotLightOuterAngleMin    = 1.0f;
    inline constexpr float32 k_SpotLightOuterAngleMax    = 179.0f;
    inline constexpr float32 k_SpotLightInnerPercentMin  = 1.0f;
    inline constexpr float32 k_SpotLightInnerPercentMax  = 100.0f;

} // namespace


export namespace Hesternal
{

    class CameraGui final : public ComponentGui
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
            EditorGui::FieldMatrix(m_camera->Projection);
        }

    private:
        Camera* m_camera;
    };


    class DirectionalLightGui final : public ComponentGui
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
            EditorGui::FieldColor("Color", m_directionalLight->Color, false);
        }

    private:
        DirectionalLight* m_directionalLight;
    };


    class PointLightGui final : public ComponentGui
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
            EditorGui::FieldColor("Color", m_pointLight->Color, false);
            EditorGui::FieldScalar("Intensity", m_pointLight->Intensity, k_PunctualLightIntensityMin);
            EditorGui::FieldScalar("Range", m_pointLight->Range, k_PunctualLightRangeMin);
        }

    private:
        PointLight* m_pointLight;
    };

    class SpotLightGui final : public ComponentGui
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
            EditorGui::FieldColor("Color", m_spotLight->Color, false);
            EditorGui::FieldScalar("Intensity", m_spotLight->Intensity, k_PunctualLightIntensityMin);
            EditorGui::FieldScalar("Range", m_spotLight->Range, k_PunctualLightRangeMin);
            EditorGui::SliderScalar("Outer Angle", m_spotLight->SpotAngle, k_SpotLightOuterAngleMin, k_SpotLightOuterAngleMax);
            EditorGui::SliderScalar("Inner Angle (%)", m_spotLight->InnerSpotPercent, k_SpotLightInnerPercentMin, k_SpotLightInnerPercentMax);
        }

    private:
        SpotLight* m_spotLight;
    };


    class RenderMeshGui final : public ComponentGui
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
