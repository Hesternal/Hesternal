using System;

namespace Copium.BuildTool.Core;


// TODO(v.matushkin): And why did I made this class?
internal sealed class CbtException : Exception
{
    public CbtException(string message)
        : base(message)
    {
    }
}
