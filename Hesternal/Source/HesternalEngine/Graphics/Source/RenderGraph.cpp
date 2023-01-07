module;

#include "Hesternal/Core/Debug.hpp"

module HesternalEngine.Graphics.RenderGraph;

import <utility>;


namespace Hesternal
{

    RenderGraph::RenderGraph(SwapchainHandle swapchainHandle)
        : m_scheduleCurrentPass(RenderGraphPassID::Invalid)
        , m_graphicsDevice(nullptr)
        , m_swapchainHandle(swapchainHandle)
        , m_isLastRenderPass(false)
    {
    }


    void RenderGraph::AddRenderPass(std::unique_ptr<IRenderPass>&& renderPass)
    {
        const std::string& renderPassName = renderPass->GetName();

        const RenderGraphPassID renderPassID = static_cast<RenderGraphPassID>(m_renderPassesInfo.size());
        m_renderPassNameToId.emplace(renderPassName, renderPassID);

        IRenderPass* renderPassPtr = renderPass.get();

        m_renderPassesInfo.push_back(RenderGraphPassInfo{
            .Pass          = std::move(renderPass),
            .CreateTexture = RenderGraphTextureID::Invalid,
            .ReadTexture   = RenderGraphTextureID::Invalid,
        });

        m_scheduleCurrentPass = renderPassID;
        renderPassPtr->OnSchedule(*this);
        m_scheduleCurrentPass = RenderGraphPassID::Invalid;
    }


    void RenderGraph::Build(IGraphicsDevice* graphicsDevice)
    {
        m_graphicsDevice = graphicsDevice;

        const uint64 lastRenderPassIndex = m_renderPassesInfo.size() - 1;

        for (uint64 i = 0; i < lastRenderPassIndex; i++)
        {
            const RenderGraphPassInfo& renderPassInfo = m_renderPassesInfo[i];
            renderPassInfo.Pass->OnCreate(*this);
            m_renderPasses.push_back(renderPassInfo.Pass.get());
        }

        m_isLastRenderPass = true;
        {
            const RenderGraphPassInfo& renderPassInfo = m_renderPassesInfo[lastRenderPassIndex];
            renderPassInfo.Pass->OnCreate(*this);
            m_renderPasses.push_back(renderPassInfo.Pass.get());
        }
        m_isLastRenderPass = false;

        m_graphicsDevice = nullptr;
    }

    void RenderGraph::Execute(RenderContext& renderContext)
    {
        for (IRenderPass* const renderPass : m_renderPasses)
        {
            renderPass->OnRender(renderContext);
        }
    }


    RenderGraphTextureID RenderGraph::ScheduleCreate(const std::string& renderTextureName)
    {
        const RenderGraphTextureID renderTextureID = _GetRenderTextureID(renderTextureName);

        RenderGraphTextureInfo& textureInfo = m_renderTexturesInfo[std::to_underlying(renderTextureID)];
        COP_ASSERT_MSG(textureInfo.CreatePass == RenderGraphPassID::Invalid, "Trying to create RenderTexture with the same name twice");
        textureInfo.CreatePass = m_scheduleCurrentPass;

        RenderGraphPassInfo& passInfo = m_renderPassesInfo[std::to_underlying(m_scheduleCurrentPass)];
        passInfo.CreateTexture = renderTextureID;

        return renderTextureID;
    }

    RenderGraphTextureID RenderGraph::ScheduleRead(const std::string& renderTextureName)
    {
        const RenderGraphTextureID renderTextureID = _GetRenderTextureID(renderTextureName);

        RenderGraphTextureInfo& textureInfo = m_renderTexturesInfo[std::to_underlying(renderTextureID)];
        COP_ASSERT_MSG(textureInfo.ReadPass == RenderGraphPassID::Invalid, "Only one RenderTexture read access is supported");
        textureInfo.ReadPass = m_scheduleCurrentPass;

        RenderGraphPassInfo& passInfo = m_renderPassesInfo[std::to_underlying(m_scheduleCurrentPass)];
        COP_ASSERT_MSG(passInfo.ReadTexture == RenderGraphTextureID::Invalid, "RenderPass can read only one RenderTexture");
        passInfo.ReadTexture = renderTextureID;

        return renderTextureID;
    }

    RenderGraphTextureID RenderGraph::_GetRenderTextureID(const std::string& renderTextureName)
    {
        RenderGraphTextureID renderTextureID;

        if (const auto it = m_renderTextureNameToId.find(renderTextureName); it != m_renderTextureNameToId.end())
        {
            renderTextureID = it->second;
        }
        else
        {
            renderTextureID = static_cast<RenderGraphTextureID>(m_renderTexturesInfo.size());

            m_renderTexturesInfo.push_back(RenderGraphTextureInfo{
                .Name       = renderTextureName,
                .Handle     = RenderTextureHandle::Invalid,
                .CreatePass = RenderGraphPassID::Invalid,
                .ReadPass   = RenderGraphPassID::Invalid,
            });

            m_renderTextureNameToId.emplace(renderTextureName, renderTextureID);
        }

        return renderTextureID;
    }


    RenderTextureHandle RenderGraph::GetRenderTexture(RenderGraphTextureID renderTextureId) const
    {
        return m_renderTexturesInfo[std::to_underlying(renderTextureId)].Handle;
    }


    RenderTextureHandle RenderGraph::CreateRenderTexture(
        RenderGraphTextureID renderTextureId, uint32 width, uint32 height, RenderTextureFormat format, const RenderTextureClearValue& clearValue
    )
    {
        // TODO(v.matushkin): Inject copy RenderPass if requested RenderTexture is different than the swapchain
        if (m_isLastRenderPass)
        {
            return m_graphicsDevice->GetSwapchainRenderTexture(m_swapchainHandle);
        }

        RenderGraphTextureInfo& renderTextureInfo = m_renderTexturesInfo[std::to_underlying(renderTextureId)];

        const RenderTextureUsage renderTextureUsage = renderTextureInfo.ReadPass == RenderGraphPassID::Invalid
                                                    ? RenderTextureUsage::Default
                                                    : RenderTextureUsage::ShaderRead;

        RenderTextureDesc renderTextureDesc = {
            .Name       = renderTextureInfo.Name,
            .ClearValue = clearValue,
            .Width      = width,
            .Height     = height,
            .Format     = format,
            .Usage      = renderTextureUsage,
        };

        renderTextureInfo.Handle = m_graphicsDevice->CreateRenderTexture(renderTextureDesc);

        return renderTextureInfo.Handle;
    }

    RenderPassHandle RenderGraph::CreateRenderPass(const RenderPassDesc& renderPassDesc)
    {
        return m_graphicsDevice->CreateRenderPass(renderPassDesc);
    }

} // namespace Hesternal
