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

        // Default constructor for serialization
        Mesh();
        // This is for asset import
        Mesh(DoNotInitialize, MeshDesc&& textureDesc);

        Mesh(Mesh&& other) noexcept;
        Mesh& operator=(Mesh&& other) noexcept;

        //< Object Interface Public
        [[nodiscard]] const std::string& GetName() const override { return m_meshDesc.Name; }
        //> Object Interface Public

        [[nodiscard]] MeshHandle GetHandle() const { return m_meshHandle; }

    private:
        //< Object Interface Protected
        [[nodiscard]] ClassID GetClassID() const override { return ClassID::Mesh; }
        void Convert(IBinaryConverter& bc) override;
        void OnAfterDeserealize() override;
        //> Object Interface Protected

        void _InitGpuResource();

    private:
        MeshDesc   m_meshDesc;
        MeshHandle m_meshHandle;
    };

} // export namespace Copium
