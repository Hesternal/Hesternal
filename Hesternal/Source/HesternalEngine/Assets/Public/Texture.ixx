export module HesternalEngine.Assets.Texture;

import HesternalEngine.Core.Object;
import HesternalEngine.Graphics.GraphicsTypes;

import <string>;


export namespace Hesternal
{

    class Texture final : public Object
    {
    public:
        Texture(TextureDesc&& textureDesc, const SamplerDesc& samplerDesc);
        ~Texture();

        // Default constructor for serialization
        Texture();
        // This is for asset import
        Texture(DoNotInitialize, TextureDesc&& textureDesc);

        Texture(Texture&& other) noexcept;
        Texture& operator=(Texture&& other) noexcept;

        //< Object Interface Public
        [[nodiscard]] const std::string& GetName() const override { return m_textureDesc.Name; }
        //> Object Interface Public

        [[nodiscard]] TextureHandle GetHandle() const { return m_textureHandle; }

    private:
        //< Object Interface Protected
        [[nodiscard]] ClassID GetClassID() const override { return ClassID::Texture2D; }
        void Convert(IBinaryConverter& bc) override;
        void OnAfterDeserealize() override;
        //> Object Interface Protected

        void _InitGpuResource();

    private:
        TextureDesc   m_textureDesc;
        SamplerDesc   m_samplerDesc;
        TextureHandle m_textureHandle;
    };

} // export namespace Hesternal
