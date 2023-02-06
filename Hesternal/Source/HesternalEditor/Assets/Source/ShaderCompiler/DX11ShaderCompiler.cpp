module;

#include "Hesternal/Core/Debug.hpp"

#include <d3d11_4.h>
#include <d3dcompiler.h>

// NOTE(v.matushkin): <SAL Warnings> May be it will be fixed once we get import std;
HS_WARNING_PUSH
HS_WARNING_DISABLE_MSVC(4005) // warning C4005: macro redefinition
HS_WARNING_DISABLE_MSVC(5106) // warning C5106: macro redefined with different parameter names
module HesternalEditor.Assets.DX11ShaderCompiler;
HS_WARNING_POP

import Hesternal.Core;

import HesternalEngine.Core.Platform.File;

import <memory>;
import <string_view>;
import <unordered_map>;


namespace
{
    using namespace Hesternal;


    static constexpr const char* k_ShaderEntryPoint   = "main";
    static constexpr const char* k_VertexShaderTarget = "vs_5_0";
    static constexpr const char* k_PixelShaderTarget  = "ps_5_0";


    // NOTE(v.matushkin): I think this class will not handle cases like this:
    //   |-- includeDir
    //   |   |-- includes
    //   |   |   |-- IncludeA.hlsl
    //   |   |   |-- IncludeB.hlsl -> #include "IncludeA.hlsl"          not gonna work because it assumes that the current dir is includeDir
    //   |   |-- Shader.shader     -> #include "includes/IncludeB.hlsl" works because shaderDir + includeDir/IncludeB.hlsl will work
    //
    //   At the same time it's probably better to enforce that every include path should be relative to some root dir, just like I do it in C++

    class IncludeHandler final : public ID3DInclude
    {
    public:
        // NOTE(v.matushkin): Passing as a pointer to remove redunandant copy, very safe
        IncludeHandler(const std::filesystem::path* includeDir)
            : m_includeDir(includeDir)
        {}

        HRESULT __stdcall Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, [[maybe_unused]] LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes) noexcept override
        {
            // D3D_INCLUDE_LOCAL  -> "include.hlsl"
            // D3D_INCLUDE_SYSTEM -> <include.hlsl>

            // HS_LOG_TRACE("    DX11 Include: {:s}", pFileName);

            if (IncludeType != D3D_INCLUDE_TYPE::D3D_INCLUDE_LOCAL)
            {
                HS_LOG_ERROR("Can handle only local shader includes at the moment");
                return E_FAIL;
            }

            std::string includeName(pFileName);
            std::string* includeContent = nullptr;

            if (const auto it = m_includes.find(includeName); it != m_includes.end())
            {
                includeContent = &it->second;
                // HS_LOG_TRACE("      found in cache");
            }
            else
            {
                const auto includeFilePath = (*m_includeDir) / pFileName;
                if (std::filesystem::exists(includeFilePath))
                {
                    const auto emplaceIt = m_includes.emplace(std::move(includeName), File::ReadAllText(includeFilePath.string())).first;
                    includeContent = &emplaceIt->second;
                    // HS_LOG_TRACE("      loaded from file");
                }
                else
                {
                    HS_LOG_ERROR("Couldn't find include file: {:s}", includeName);
                    return E_FAIL;
                }
            }

            // NOTE(v.matushkin): This can't crash right?
            *ppData = includeContent->c_str();
            *pBytes = static_cast<uint32>(includeContent->length());

            return S_OK;
        }

        HRESULT __stdcall Close([[maybe_unused]] LPCVOID pData) noexcept override
        {
            // NOTE(v.matushkin): No need to do anything? In theory I should free pData here, but includes file content will be freed when we done anyway.
            return S_OK;
        }

    private:
        const std::filesystem::path* m_includeDir;
        std::unordered_map<std::string, std::string> m_includes; // Key - include name, Value - include content
    };


    // NOTE(v.matushkin): Probably would be better if this function was a method of DX11ShaderCopmiler and then no need to pass IncludeHandler.
    //   But then I need to include d3d headers in module interface.
    [[nodiscard]] static ShaderBlob dx11_Compile(IncludeHandler* includeHandler, std::string_view programSource, const char* programName, const char* programTarget)
    {
        // TODO(v.matushkin): Add some kind of std::unique_ptr aka ScopedComPtr for these, no need for reference counter
        ID3DBlob* d3dShaderProgramBlob;
        ID3DBlob* d3dShaderProgramErrorBlob;

        // TODO(v.matushkin): D3DCOMPILE_OPTIMIZATION_LEVEL*
        constexpr uint32 d3dCompileFlags = D3DCOMPILE_ENABLE_STRICTNESS
                                         | D3DCOMPILE_WARNINGS_ARE_ERRORS;

        // NOTE(v.matushkin): D3DCompile2?
        const HRESULT d3dCompileResult = D3DCompile(
            programSource.data(),
            programSource.length(),
            programName,           // NOTE(v.matushkin): Can be nullptr, don't know where/how it will be used
            nullptr,               // Defines
            includeHandler,
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
            HS_ASSERT_MSG(d3dShaderProgramErrorBlob != nullptr, "Could this ever happen?");
            HS_LOG_ERROR("Shader program compilation error:\n{}", (char*)d3dShaderProgramErrorBlob->GetBufferPointer());
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

    void DX11ShaderCompiler::Compile(const std::filesystem::path* includeDir, const ParsedShaderDesc& parsedShaderDesc, ShaderDesc& outShaderDesc)
    {
        // HS_LOG_TRACE("DX11ShaderCompiler::Compile for {:s}", parsedShaderDesc.Name);

        IncludeHandler includeHandler(includeDir);

        // HS_LOG_TRACE("  Compiling Vertex");
        outShaderDesc.VertexBlob = dx11_Compile(&includeHandler, parsedShaderDesc.VertexSource, parsedShaderDesc.Name.data(), k_VertexShaderTarget);
        // HS_LOG_TRACE("  Compiling Pixel");
        outShaderDesc.FragmentBlob = dx11_Compile(&includeHandler, parsedShaderDesc.FragmentSource, parsedShaderDesc.Name.data(), k_PixelShaderTarget);
    }

} // namespace Hesternal
