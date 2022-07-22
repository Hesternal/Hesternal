module;

#include <string>

export module CopiumEngine.Assets.Shader;

import CopiumEngine.Core.Object;
import CopiumEngine.Graphics.GraphicsTypes;


namespace Copium
{
    class AssetManager;
}


export namespace Copium
{

    class Shader final : public Object
    {
        friend AssetManager;

    public:
        ~Shader();

        //< Object Interface
        [[nodiscard]] std::string GetName() const override { return m_shaderDesc.Name; }
        //> Object Interface

        [[nodiscard]] ShaderHandle GetHandle() const { return m_shaderHandle; }

    private:
        Shader(ShaderDesc&& shaderDesc);

    private:
        ShaderDesc   m_shaderDesc;
        ShaderHandle m_shaderHandle;
    };

} // export namespace Copium
