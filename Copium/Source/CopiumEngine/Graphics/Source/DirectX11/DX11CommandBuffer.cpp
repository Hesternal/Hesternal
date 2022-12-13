module;

#include "Copium/Core/Debug.hpp"

#include <d3d11_4.h>

// NOTE(v.matushkin): <SAL Warnings> May be it will be fixed once we get 'import std;'
COP_WARNING_PUSH
COP_WARNING_DISABLE_MSVC(4005) // warning C4005: macro redefinition
COP_WARNING_DISABLE_MSVC(5106) // warning C5106: macro redefined with different parameter names
module CopiumEngine.Graphics.DX11GraphicsDevice;

import CopiumEngine.Core.Platform;
import CopiumEngine.Graphics.DX11Conversion;
COP_WARNING_POP


namespace
{
    using namespace Copium;


    static constexpr float32 k_ViewportMinDepth = 0.0f;
    static constexpr float32 k_ViewportMaxDepth = 1.0f;

} // namespace


namespace Copium
{

    DX11CommandBuffer::DX11CommandBuffer(ID3D11DeviceContext4* deviceContext, DX11GraphicsDevice* graphicsDevice)
        : m_deviceContext(deviceContext)
        , m_graphicsDevice(graphicsDevice)
    {
#if COP_ENABLE_GRAPHICS_API_DEBUG
        m_deviceContext->QueryInterface(&m_annotation);
        m_makeAnnotationCalls = m_annotation->GetStatus();
#endif
    }

    DX11CommandBuffer::~DX11CommandBuffer()
    {
#if COP_ENABLE_GRAPHICS_API_DEBUG
        m_annotation->Release();
#endif
    }


    void DX11CommandBuffer::BeginRenderPass(RenderPassHandle renderPassHandle)
    {
        const DX11RenderPass& dx11RenderPass = m_graphicsDevice->_GetRenderPass(renderPassHandle);

        //- Clear RenderTargets
        //-- Color
        for (uint8 i = 0; i < dx11RenderPass.NumClearColorAttachments; i++)
        {
            const DX11RenderTexture& dx11RenderTexture = m_graphicsDevice->_GetRenderTexture(dx11RenderPass.ClearColorAttachments[i]);

            m_deviceContext->ClearRenderTargetView(dx11RenderTexture.RTV, dx11RenderTexture.ClearValue.Color.Value);
        }
        //-- DepthStencil
        if (dx11RenderPass.ClearDepthStencilAttachment != RenderTextureHandle::Invalid)
        {
            const DX11RenderTexture& dx11RenderTexture = m_graphicsDevice->_GetRenderTexture(dx11RenderPass.ClearDepthStencilAttachment);
            const uint32 depthStencilClearFlags = dx11_RenderTextureTypeToClearFlags(dx11RenderTexture.Type);
            const ClearDepthStencilValue clearDepthStencilValue = dx11RenderTexture.ClearValue.DepthStencil;

            m_deviceContext->ClearDepthStencilView(
                dx11RenderTexture.DSV,
                depthStencilClearFlags,
                clearDepthStencilValue.Depth,
                clearDepthStencilValue.Stencil
            );
        }

        //- Set RenderTargets
        {
            ID3D11RenderTargetView* d3dSubpassRTVs[8]; // Actual type -> ID3D11RenderTargetView1
            ID3D11DepthStencilView* d3dSubpassDSV;

            for (uint8 i = 0; i < dx11RenderPass.Subpass.NumColorAttachments; i++)
            {
                d3dSubpassRTVs[i] = m_graphicsDevice->_GetRenderTexture(dx11RenderPass.Subpass.ColorAttachments[i]).RTV;
            }

            if (dx11RenderPass.Subpass.DepthStencilAttachment != RenderTextureHandle::Invalid)
            {
                d3dSubpassDSV = m_graphicsDevice->_GetRenderTexture(dx11RenderPass.Subpass.DepthStencilAttachment).DSV;
            }
            else
            {
                d3dSubpassDSV = nullptr;
            }

            m_deviceContext->OMSetRenderTargets(dx11RenderPass.Subpass.NumColorAttachments, d3dSubpassRTVs, d3dSubpassDSV);
        }
    }


    void DX11CommandBuffer::SetViewport(const Rect& viewportRect)
    {
        const D3D11_VIEWPORT d3dViewport = {
            .TopLeftX = viewportRect.X,
            .TopLeftY = viewportRect.Y,
            .Width    = viewportRect.Width,
            .Height   = viewportRect.Height,
            .MinDepth = k_ViewportMinDepth,
            .MaxDepth = k_ViewportMaxDepth,
        };
        m_deviceContext->RSSetViewports(1, &d3dViewport);
    }

    void DX11CommandBuffer::SetScissorRect(const RectInt& scissorRect)
    {
        const D3D11_RECT d3dScissorRect = {
            .left   = scissorRect.X,
            .top    = scissorRect.Y,
            .right  = scissorRect.GetMaxX(),
            .bottom = scissorRect.GetMaxY(),
        };
        m_deviceContext->RSSetScissorRects(1, &d3dScissorRect);
    }


    void DX11CommandBuffer::BindShader(ShaderHandle shaderHandle)
    {
        const DX11Shader& dx11Shader = m_graphicsDevice->_GetShader(shaderHandle);

        m_deviceContext->IASetInputLayout(dx11Shader.InputLayout);
        m_deviceContext->VSSetShader(dx11Shader.VertexShader, nullptr, 0);
        m_deviceContext->RSSetState(dx11Shader.RasterizerState);
        m_deviceContext->PSSetShader(dx11Shader.PixelShader, nullptr, 0);
        // TODO(v.matushkin): StencilRef ?
        m_deviceContext->OMSetDepthStencilState(dx11Shader.DepthStencilState, D3D11_DEFAULT_STENCIL_REFERENCE);
        // TODO(v.matushkin): BlendFactor, SampleMask ? BlendFactor = nullptr -> {1,1,1,1}
        m_deviceContext->OMSetBlendState(dx11Shader.BlendState, nullptr, D3D11_DEFAULT_SAMPLE_MASK);
    }

    void DX11CommandBuffer::BindVertexBuffer(GraphicsBufferHandle vertexBufferHandle, uint32 stride, uint32 offset)
    {
        const DX11GraphicsBuffer& dx11VertexBuffer = m_graphicsDevice->_GetGraphicsBuffer(vertexBufferHandle);
        m_deviceContext->IASetVertexBuffers(0, 1, &dx11VertexBuffer.Buffer, &stride, &offset);
    }

    void DX11CommandBuffer::BindIndexBuffer(GraphicsBufferHandle indexBufferHandle, IndexFormat indexFormat)
    {
        const DX11GraphicsBuffer& dx11IndexBuffer = m_graphicsDevice->_GetGraphicsBuffer(indexBufferHandle);
        m_deviceContext->IASetIndexBuffer(dx11IndexBuffer.Buffer, dx11_IndexFormat(indexFormat), 0);
    }

    void DX11CommandBuffer::BindConstantBuffer(GraphicsBufferHandle constantBufferHandle, uint32 slot)
    {
        const DX11GraphicsBuffer& dx11GraphicsBuffer = m_graphicsDevice->_GetGraphicsBuffer(constantBufferHandle);
        m_deviceContext->VSSetConstantBuffers(slot, 1, &dx11GraphicsBuffer.Buffer);
    }

    void DX11CommandBuffer::BindConstantBuffer(GraphicsBufferHandle constantBufferHandle, uint32 slot, uint32 elementIndex, uint32 elementSize)
    {
        const DX11GraphicsBuffer& dx11GraphicsBuffer = m_graphicsDevice->_GetGraphicsBuffer(constantBufferHandle);
        const uint32 numConstants = elementSize / 16;
        const uint32 firstConstant = elementIndex * numConstants;
        m_deviceContext->VSSetConstantBuffers1(slot, 1, &dx11GraphicsBuffer.Buffer, &firstConstant, &numConstants);
    }

    void DX11CommandBuffer::BindTexture(TextureHandle textureHandle, uint32 slot)
    {
        DX11Texture2D& dx11Texture = m_graphicsDevice->_GetTexture(textureHandle);
        ID3D11ShaderResourceView* d3dTextureSRV = dx11Texture.SRV;
        m_deviceContext->PSSetShaderResources(slot, 1, &d3dTextureSRV);
        m_deviceContext->PSSetSamplers(slot, 1, &dx11Texture.Sampler);
    }

    void DX11CommandBuffer::BindTexture(RenderTextureHandle renderTextureHandle, uint32 slot)
    {
        DX11RenderTexture& dx11RenderTexture = m_graphicsDevice->_GetRenderTexture(renderTextureHandle);
        ID3D11ShaderResourceView* d3dRenderTextureSRV = dx11RenderTexture.SRV;
        COP_ASSERT_MSG(d3dRenderTextureSRV != nullptr, "Trying to access nullptr DX11RenderTexture.SRV");

        ID3D11SamplerState* d3dRenderTextureSampler = m_graphicsDevice->GetRenderTextureSampler();

        m_deviceContext->PSSetShaderResources(slot, 1, &d3dRenderTextureSRV);
        m_deviceContext->PSSetSamplers(slot, 1, &d3dRenderTextureSampler);
    }

    void DX11CommandBuffer::BindMaterial(TextureHandle baseColorTextureHandle, TextureHandle normalTextureHandle)
    {
        DX11Texture2D& dx11BaseColorTexture = m_graphicsDevice->_GetTexture(baseColorTextureHandle);
        DX11Texture2D& dx11NormalTexture = m_graphicsDevice->_GetTexture(normalTextureHandle);

        ID3D11ShaderResourceView* materialTextures[] = { dx11BaseColorTexture.SRV, dx11NormalTexture.SRV };
        ID3D11SamplerState* textureSamplers[] = { dx11BaseColorTexture.Sampler, dx11NormalTexture.Sampler };

        m_deviceContext->PSSetShaderResources(0, 2, materialTextures);
        m_deviceContext->PSSetSamplers(0, 2, textureSamplers);
    }


    void DX11CommandBuffer::DrawIndexed(uint32 indexCount, uint32 firstIndex, uint32 vertexOffset)
    {
        m_deviceContext->DrawIndexed(indexCount, firstIndex, vertexOffset);
    }

    void DX11CommandBuffer::DrawMesh(MeshHandle meshHandle)
    {
        const DX11Mesh& dx11Mesh = m_graphicsDevice->_GetMesh(meshHandle);

        m_deviceContext->IASetIndexBuffer(dx11Mesh.Index, dx11Mesh.IndexFormat, 0);
        m_deviceContext->IASetVertexBuffers(0, 3, dx11Mesh.VertexBuffers, dx11Mesh.VertexStrides, dx11Mesh.VertexOffsets);

        m_deviceContext->DrawIndexed(dx11Mesh.IndexCount, 0, 0);
    }

    void DX11CommandBuffer::DrawProcedural(uint32 vertexCount)
    {
        m_deviceContext->Draw(vertexCount, 0);
    }


    void DX11CommandBuffer::CopyBuffer(GraphicsBufferHandle srcGraphicsBufferHandle, GraphicsBufferHandle dstGraphicsBufferHandle)
    {
        ID3D11Buffer* const d3dSrcBuffer = m_graphicsDevice->_GetGraphicsBuffer(srcGraphicsBufferHandle).Buffer;
        ID3D11Buffer* const d3dDstBuffer = m_graphicsDevice->_GetGraphicsBuffer(dstGraphicsBufferHandle).Buffer;
        m_deviceContext->CopyResource(d3dDstBuffer, d3dSrcBuffer);
    }

    void* DX11CommandBuffer::MapBuffer(GraphicsBufferHandle graphicsBufferHandle)
    {
        ID3D11Buffer* const d3dBuffer = m_graphicsDevice->_GetGraphicsBuffer(graphicsBufferHandle).Buffer;
        D3D11_MAPPED_SUBRESOURCE d3dMappedSubresource = {
            .pData      = nullptr,
            .RowPitch   = 0,
            .DepthPitch = 0,
        };
        // NOTE(v.matushkin): There is only one flag: D3D11_MAP_FLAG_DO_NOT_WAIT
        m_deviceContext->Map(d3dBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedSubresource);

        return d3dMappedSubresource.pData;
    }

    void DX11CommandBuffer::UnmapBuffer(GraphicsBufferHandle graphicsBufferHandle)
    {
        ID3D11Buffer* const d3dBuffer = m_graphicsDevice->_GetGraphicsBuffer(graphicsBufferHandle).Buffer;
        m_deviceContext->Unmap(d3dBuffer, 0);
    }


#if COP_ENABLE_GRAPHICS_API_DEBUG
    void DX11CommandBuffer::BeginSample(std::string_view name)
    {
        if (m_makeAnnotationCalls)
        {
            const std::wstring wideName = Platform::ToWideString(name);
            m_annotation->BeginEvent(wideName.data());
        }
    }

    void DX11CommandBuffer::EndSample()
    {
        if (m_makeAnnotationCalls)
        {
            m_annotation->EndEvent();
        }
    }
#endif

} // namespace Copium
