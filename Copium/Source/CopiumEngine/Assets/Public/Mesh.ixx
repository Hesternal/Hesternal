export module CopiumEngine.Assets.Mesh;

import CopiumEngine.Graphics.GraphicsTypes;


export namespace Copium
{

    class Mesh final
    {
    public:
        Mesh(MeshDesc&& meshDesc);
        ~Mesh();

        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;
        Mesh(Mesh&& other) noexcept = default;
        Mesh& operator=(Mesh&& other) noexcept = default;

        [[nodiscard]] MeshHandle GetHandle() const { return m_meshHandle; }

    private:
        MeshDesc   m_meshDesc;
        MeshHandle m_meshHandle;
    };

} // export namespace Copium
