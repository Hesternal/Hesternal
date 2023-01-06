export module CopiumEngine.Graphics.RenderGraph;

import Copium.Core;
import CopiumEngine.Graphics.GraphicsTypes;
import CopiumEngine.Graphics.IGraphicsDevice;
import CopiumEngine.Graphics.RenderContext;

import <memory>;
import <string>;
import <unordered_map>;
import <utility>;
import <vector>;


export namespace Copium
{

    class RenderGraph;


    enum class RenderGraphTextureID : uint8 { Invalid = 255 };


    // NOTE(v.matushkin): Don't know how to move it to its own module
    struct IRenderPass
    {
        virtual ~IRenderPass() = default;

        IRenderPass(const IRenderPass& other) = delete;
        IRenderPass& operator=(const IRenderPass& other) = delete;
        IRenderPass(IRenderPass&& other) noexcept = default;
        IRenderPass& operator=(IRenderPass&& other) noexcept = default;

        [[nodiscard]] const std::string& GetName() { return m_renderPassName; }

        virtual void OnSchedule(RenderGraph& renderGraph) = 0;
        virtual void OnCreate(RenderGraph& renderGraph) = 0;
        virtual void OnRender(RenderContext& renderContext) = 0;

    protected:
        IRenderPass(std::string&& renderPassName) : m_renderPassName(std::move(renderPassName)) {}

    private:
        const std::string m_renderPassName;
    };


    class RenderGraph final
    {
        friend IRenderPass;


        enum class RenderGraphPassID : uint8 { Invalid = 255 };


        struct RenderGraphTextureInfo final
        {
            std::string         Name;
            RenderTextureHandle Handle;
            RenderGraphPassID   CreatePass;
            RenderGraphPassID   ReadPass;
        };

        struct RenderGraphPassInfo final
        {
            std::unique_ptr<IRenderPass> Pass;
            RenderGraphTextureID         CreateTexture;
            RenderGraphTextureID         ReadTexture;
        };


        using RenderPassNameToIdMap    = std::unordered_map<std::string, RenderGraphPassID>;
        using RenderTextureNameToIdMap = std::unordered_map<std::string, RenderGraphTextureID>;

        using RenderPassInfo    = std::vector<RenderGraphPassInfo>;
        using RenderTextureInfo = std::vector<RenderGraphTextureInfo>;

    public:
        RenderGraph(SwapchainHandle swapchainHandle);
        ~RenderGraph() = default;

        RenderGraph(const RenderGraph&) = delete;
        RenderGraph& operator=(const RenderGraph&) = delete;
        RenderGraph(RenderGraph&& other) noexcept = default;
        RenderGraph& operator=(RenderGraph&& other) noexcept = default;

        void AddRenderPass(std::unique_ptr<IRenderPass>&& renderPass);

        void Build(IGraphicsDevice* graphicsDevice);
        void Execute(RenderContext& renderContext);

        //< IRenderPass OnSchedule
        [[nodiscard]] RenderGraphTextureID ScheduleCreate(const std::string& renderTextureName);
        [[nodiscard]] RenderGraphTextureID ScheduleRead(const std::string& renderTextureName);
        //> IRenderPass OnSchedule

        //< IRenderPass OnCreate
        [[nodiscard]] RenderTextureHandle GetRenderTexture(RenderGraphTextureID renderTextureId) const;

        [[nodiscard]] RenderTextureHandle CreateRenderTexture(
            RenderGraphTextureID renderTextureId, uint32 width, uint32 height, RenderTextureFormat format, const RenderTextureClearValue& clearValue
        );
        [[nodiscard]] RenderPassHandle CreateRenderPass(const RenderPassDesc& renderPassDesc);
        //> IRenderPass OnCreate

    private:
        //< IRenderPass OnSchedule
        [[nodiscard]] RenderGraphTextureID _GetRenderTextureID(const std::string& renderTextureName);
        //> IRenderPass OnSchedule

    private:
        std::vector<IRenderPass*>  m_renderPasses; // RenderPasses to execute

        RenderPassNameToIdMap      m_renderPassNameToId;
        RenderTextureNameToIdMap   m_renderTextureNameToId;

        RenderPassInfo             m_renderPassesInfo;
        RenderTextureInfo          m_renderTexturesInfo;

        //< IRenderPass OnSchedule
        RenderGraphPassID          m_scheduleCurrentPass;
        //> IRenderPass OnSchedule
        //< IRenderPass OnCreate
        IGraphicsDevice*           m_graphicsDevice;
        SwapchainHandle            m_swapchainHandle;
        bool                       m_isLastRenderPass;
        //> IRenderPass OnCreate
    };

} // export namespace Copium
