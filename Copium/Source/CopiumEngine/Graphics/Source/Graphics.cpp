module;

#include <memory>

module CopiumEngine.Graphics;

import CopiumEngine.Graphics.DX11GraphicsDevice;


namespace Copium
{

    void Graphics::Init()
    {
        m_graphicsDevice = std::make_unique<DX11GraphicsDevice>();
    }

    void Graphics::Shutdown()
    {
        m_graphicsDevice.reset();
    }

} // namespace Copium
