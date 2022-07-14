export module CopiumEngine.Assets.Shader;

import CopiumEngine.Graphics.GraphicsTypes;


namespace Copium
{
    class AssetManager;
}


export namespace Copium
{

    class Shader final
    {
        friend AssetManager;

    public:
        ~Shader();

        [[nodiscard]] ShaderHandle GetHandle() const { return m_shaderHandle; }

    private:
        Shader(ShaderDesc&& shaderDesc);

    private:
        ShaderDesc   m_shaderDesc;
        ShaderHandle m_shaderHandle;
    };

} // export namespace Copium
