export module HesternalEngine.Graphics.GraphicsBuffer;

import Hesternal.Core;

import HesternalEngine.Graphics.GraphicsTypes;

import <memory>;
import <span>;


// TODO(v.matushkin): Right now Map/Unmap works with CPU memory and then I copy it in the IGraphicsDevice
//   also with Map/Unmap. I should make GraphicsBuffer directly call IGraphicsDevice Map/Unmap.
// NOTE(v.matushkin): Removed CPU side data copy, so there is no way to recreate GraphicsBuffer on device lost.
//   Although I'm far from solving this kind of problems anyway.


export namespace Hesternal
{

    class GraphicsBuffer final
    {
    public:
        GraphicsBuffer(const GraphicsBufferDesc& graphicsBufferDesc, std::span<const uint8> initialData = {});
        ~GraphicsBuffer();

        GraphicsBuffer(const GraphicsBuffer& other) = delete;
        GraphicsBuffer& operator=(const GraphicsBuffer& other) = delete;
        GraphicsBuffer(GraphicsBuffer&& other) noexcept;
        GraphicsBuffer& operator=(GraphicsBuffer&& other) noexcept;

        [[nodiscard]] uint32               GetElementCount() const noexcept { return m_graphicsBufferDesc.ElementCount; }
        [[nodiscard]] uint32               GetElementSize()  const noexcept { return m_graphicsBufferDesc.ElementSize; }
        [[nodiscard]] GraphicsBufferUsage  GetUsage()        const noexcept { return m_graphicsBufferDesc.Usage; }
        [[nodiscard]] uint32               GetBufferSize()   const noexcept { return m_sizeInBytes; }
        [[nodiscard]] GraphicsBufferHandle GetHandle()       const noexcept { return m_graphicsBufferHandle; }

    private:
        GraphicsBufferDesc   m_graphicsBufferDesc;
        uint32               m_sizeInBytes;
        GraphicsBufferHandle m_graphicsBufferHandle;
    };

} // export namespace Hesternal
