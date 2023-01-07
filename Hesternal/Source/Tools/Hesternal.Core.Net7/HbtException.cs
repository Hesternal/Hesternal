using System;

namespace Hesternal.Core.Net7;


// TODO(v.matushkin): And why did I made this class?
public sealed class HbtException : Exception
{
    public HbtException(string message)
        : base(message)
    {
    }
}
