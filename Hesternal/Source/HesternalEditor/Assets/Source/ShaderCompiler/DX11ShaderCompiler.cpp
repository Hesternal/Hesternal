module;

#include "Hesternal/Core/Debug.hpp"

#include <d3d11_4.h>
#include <d3dcompiler.h>

// NOTE(v.matushkin): <SAL Warnings> May be it will be fixed once we get import std;
COP_WARNING_PUSH
COP_WARNING_DISABLE_MSVC(4005) // warning C4005: macro redefinition
COP_WARNING_DISABLE_MSVC(5106) // warning C5106: macro redefined with different parameter names
module HesternalEditor.Assets.DX11ShaderCompiler;
COP_WARNING_POP

import Hesternal.Core;

import <memory>;
import <string_view>;


namespace
{

    using namespace Hesternal;


    static constexpr const char* k_ShaderEntryPoint   = "main";
    static constexpr const char* k_VertexShaderTarget = "vs_5_0";
    static constexpr const char* k_PixelShaderTarget  = "ps_5_0";


    [[nodiscard]] static ShaderBlob dx11_Compile(std::string_view programSource, const char* programName, const char* programTarget)
    {
        ID3DBlob* d3dShaderProgramBlob;
        ID3DBlob* d3dShaderProgramErrorBlob;
        
        // TODO(v.matushkin): D3DCOMPILE_OPTIMIZATION_LEVEL*
        constexpr uint32 d3dCompileFlags = D3DCOMPILE_ENABLE_STRICTNESS
                                         | D3DCOMPILE_WARNINGS_ARE_ERRORS;

        const HRESULT d3dCompileResult = D3DCompile(
            programSource.data(),
            programSource.length(),
            programName,           // NOTE(v.matushkin): Can be nullptr, don't know where/how it will be used
            nullptr,               // Defines
            nullptr,               // Includes
            k_ShaderEntryPoint,
            programTarget,
            d3dCompileFlags,
            0,
            &d3dShaderProgramBlob,
            &d3dShaderProgramErrorBlob
        );

        if (FAILED(d3dCompileResult))
        {
            // TODO(v.matushkin): For D3DX11CompileFromFile, errorBlob == nullptr means shader file wasn't found,
            //  so with D3DCompile this cannot happen?
            COP_ASSERT_MSG(d3dShaderProgramErrorBlob != nullptr, "Could this ever happen?");
            COP_LOG_ERROR("Shader program compilation error:\n{}", (char*)d3dShaderProgramErrorBlob->GetBufferPointer());
            d3dShaderProgramErrorBlob->Release();
        }

        ShaderBlob shaderBlob = {
            .Size = d3dShaderProgramBlob->GetBufferSize(),
            .Data = std::make_unique<uint8[]>(d3dShaderProgramBlob->GetBufferSize()),
        };

        std::memcpy(shaderBlob.Data.get(), d3dShaderProgramBlob->GetBufferPointer(), shaderBlob.Size);
        d3dShaderProgramBlob->Release();

        return shaderBlob;
    }

} // namespace


namespace Hesternal
{

    void DX11ShaderCompiler::Compile(const ParsedShaderDesc& parsedShaderDesc, ShaderDesc& shaderDesc)
    {
        shaderDesc.VertexBlob = dx11_Compile(parsedShaderDesc.VertexSource, parsedShaderDesc.Name.data(), k_VertexShaderTarget);
        shaderDesc.FragmentBlob = dx11_Compile(parsedShaderDesc.FragmentSource, parsedShaderDesc.Name.data(), k_PixelShaderTarget);
    }

} // namespace Hesternal
