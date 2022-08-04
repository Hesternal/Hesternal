export module CopiumEditor.Assets.ParsedShaderDesc;

import CopiumEngine.Graphics.GraphicsTypes;

import <string>;


export namespace Copium
{

    struct ParsedShaderDesc final
    {
        std::string           Name;
        RasterizerStateDesc   RasterizerStateDesc;
        DepthStencilStateDesc DepthStencilStateDesc;
        BlendStateDesc        BlendStateDesc;
        std::string           VertexSource;
        std::string           FragmentSource;
    };

} // export namespace Copium
