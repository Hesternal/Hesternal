export module CopiumEngine.Core.Platform;


export namespace Copium
{
    struct Guid;
}


// NOTE(v.matushkin): Module and namespace naming is shit
export namespace Copium::Platform
{

    void GenerateGuid(Guid& guid);

} // export namespace Copium
