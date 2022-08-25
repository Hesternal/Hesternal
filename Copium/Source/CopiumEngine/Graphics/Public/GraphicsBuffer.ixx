export module CopiumEngine.Graphics.GraphicsBuffer;

import CopiumEngine.Core.CoreTypes;
import CopiumEngine.Graphics.GraphicsTypes;

import <memory>;
import <span>;


// TODO(v.matushkin): Right now Map/Unmap works with CPU memory and then I copy it in the IGraphicsDevice
//  also with Map/Unmap. I should make GraphicsBuffer directly call IGraphicsDevice Map/Unmap.


export namespace Copium
{

    class GraphicsBuffer final
    {
    public:
        GraphicsBuffer(const GraphicsBufferDesc& graphicsBufferDesc);
        GraphicsBuffer(const GraphicsBufferDesc& graphicsBufferDesc, std::span<const uint8> initialData);
        ~GraphicsBuffer();

        GraphicsBuffer(const GraphicsBuffer& other) = delete;
        GraphicsBuffer& operator=(const GraphicsBuffer& other) = delete;
        GraphicsBuffer(GraphicsBuffer&& other) noexcept;
        GraphicsBuffer& operator=(GraphicsBuffer&& other) noexcept;

        [[nodiscard]] uint32 GetElementCount() const noexcept { return m_graphicsBufferDesc.ElementCount; }
        [[nodiscard]] uint32 GetElementSize() const noexcept { return m_graphicsBufferDesc.ElementSize; }
        [[nodiscard]] GraphicsBufferUsage GetUsage() const noexcept { return m_graphicsBufferDesc.Usage; }
        [[nodiscard]] GraphicsBufferHandle GetHandle() const noexcept { return m_graphicsBufferHandle; }

        void SetData(std::span<const uint8> data);
        [[nodiscard]] std::span<uint8> Map();
        void Unmap();

    private:
        void _UpdateGpuResource() const;

    private:
        GraphicsBufferDesc       m_graphicsBufferDesc;
        std::unique_ptr<uint8[]> m_graphicsBufferData;
        uint32                   m_sizeInBytes;
        GraphicsBufferHandle     m_graphicsBufferHandle;
    };

} // export namespace Copium
