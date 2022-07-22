module;

#include <string>

export module CopiumEngine.Assets.Mesh;

import CopiumEngine.Core.Object;
import CopiumEngine.Graphics.GraphicsTypes;


export namespace Copium
{

    class Mesh final : public Object
    {
    public:
        Mesh(MeshDesc&& meshDesc);
        ~Mesh();

        Mesh(Mesh&& other) noexcept = default;
        Mesh& operator=(Mesh&& other) noexcept = default;

        //< Object Interface
        [[nodiscard]] std::string GetName() const override { return m_meshDesc.Name; }
        //> Object Interface

        [[nodiscard]] MeshHandle GetHandle() const { return m_meshHandle; }

    private:
        MeshDesc   m_meshDesc;
        MeshHandle m_meshHandle;
    };

} // export namespace Copium
