export module HesternalEngine.Assets.Shader;

import HesternalEngine.Core.Object;
import HesternalEngine.Graphics.GraphicsTypes;

import <string>;


export namespace Hesternal
{

    class Shader final : public Object
    {
    public:
        Shader(ShaderDesc&& shaderDesc);
        ~Shader();

        Shader(Shader&& other) noexcept;
        Shader& operator=(Shader&& other) noexcept;

        //< Object Interface Public
        [[nodiscard]] const std::string& GetName() const override { return m_shaderDesc.Name; }
        //> Object Interface Public

        [[nodiscard]] ShaderHandle GetHandle() const { return m_shaderHandle; }

    private:
        //< Object Interface Protected
        [[nodiscard]] ClassID GetClassID() const override { return ClassID::Shader; }
        void Convert(IBinaryConverter& bc) override;
        void OnAfterDeserealize() override;
        //> Object Interface Protected

        void _InitGpuResource();

    private:
        ShaderDesc   m_shaderDesc;
        ShaderHandle m_shaderHandle;
    };

} // export namespace Hesternal
