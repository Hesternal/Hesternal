module;

#include <memory>
#include <string>
#include <utility>

export module CopiumEngine.Assets.Material;

import CopiumEngine.Assets.Shader;
import CopiumEngine.Assets.Texture;
import CopiumEngine.Core.Object;
import CopiumEngine.Graphics.GraphicsTypes;


export namespace Copium
{

    class Material final : public Object
    {
    public:
        Material(std::string&& materialName, std::shared_ptr<Shader> shader);
        ~Material();

        Material(Material&& other) noexcept = default;
        Material& operator=(Material&& other) noexcept = default;

        //< Object Interface
        [[nodiscard]] std::string GetName() const override { return m_materialName; }
        //> Object Interface

        [[nodiscard]] const std::shared_ptr<Shader>& GetShader() const { return m_shader; }

        [[nodiscard]] const std::shared_ptr<Texture>& GetBaseColorMap() const { return m_baseColorMap; }
        [[nodiscard]] const std::shared_ptr<Texture>& GetNormalMap() const { return m_normalMap; }

        void SetBaseColorMap(std::shared_ptr<Texture> baseColorMap) { m_baseColorMap = std::move(baseColorMap); }
        void SetNormalMap(std::shared_ptr<Texture> normalMap) { m_normalMap = std::move(normalMap); }

    private:
        std::string              m_materialName;
        std::shared_ptr<Shader>  m_shader;

        std::shared_ptr<Texture> m_baseColorMap;
        std::shared_ptr<Texture> m_normalMap;
    };

} // export namespace Copium
