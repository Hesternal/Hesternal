export module HesternalEditor.Assets.ParsedShaderDesc;

import HesternalEngine.Graphics.GraphicsTypes;

import <string>;


export namespace Hesternal
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

} // export namespace Hesternal
