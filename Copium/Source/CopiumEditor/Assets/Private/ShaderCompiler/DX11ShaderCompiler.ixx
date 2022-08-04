export module CopiumEditor.Assets.DX11ShaderCompiler;

import CopiumEngine.Graphics.GraphicsTypes;

import CopiumEditor.Assets.ParsedShaderDesc;


export namespace Copium
{

    class DX11ShaderCompiler final
    {
    public:
        DX11ShaderCompiler() = default;
        ~DX11ShaderCompiler() = default;

        DX11ShaderCompiler(const DX11ShaderCompiler&) = delete;
        DX11ShaderCompiler& operator=(const DX11ShaderCompiler&) = delete;

        DX11ShaderCompiler(DX11ShaderCompiler&& other) noexcept = default;
        DX11ShaderCompiler& operator=(DX11ShaderCompiler&& other) noexcept = default;

        void Compile(const ParsedShaderDesc& parsedShaderDesc, ShaderDesc& shaderDesc);
    };

} // export namespace Copium
