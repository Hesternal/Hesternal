using System;

namespace Copium.Core.Net7;


// TODO(v.matushkin): And why did I made this class?
public sealed class CbtException : Exception
{
    public CbtException(string message)
        : base(message)
    {
    }
}
