using System.ComponentModel;


// NOTE(v.matushkin): Hack for property { init; }
namespace System.Runtime.CompilerServices
{
    [EditorBrowsable(EditorBrowsableState.Never)]
    internal class IsExternalInit
    {
    }
}
