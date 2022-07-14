module;

#include <memory>

export module CopiumEngine.Graphics;

export import CopiumEngine.Graphics.GraphicsTypes;
export import CopiumEngine.Graphics.IGraphicsDevice;


export namespace Copium
{

    class Graphics final
    {
    public:
        static void Init();
        static void Shutdown();

        [[nodiscard]] static IGraphicsDevice* GetGraphicsDevice() { return m_graphicsDevice.get(); }

    private:
        static inline std::unique_ptr<IGraphicsDevice> m_graphicsDevice;
    };

} // export namespace Copium
