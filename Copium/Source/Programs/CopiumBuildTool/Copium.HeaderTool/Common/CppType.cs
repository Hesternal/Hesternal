using System;
using System.Collections.Generic;

namespace Copium.HeaderTool
{
    internal class CppType
    {
        private static readonly HashSet<string> s_FundamentalCppTypes = new HashSet<string>()
        {
            "bool",
            "char",
            "int8",
            "int16",
            "int32",
            "int64",
            "uint8",
            "uint16",
            "uint32",
            "uint64",
            "float32",
            "float64",
        };

        // NOTE(v.matushkin): Naming is kinda meh
        public readonly string FullType;
        // NOTE(v.matushkin): TypeQualifier and BaseType can be string_view(ReadOnlySpan) but I don't know how to do it
        // NOTE(v.matushkin): Since I'm targeting netstandard 2.0 now there is no Spans anyway
        public readonly string TypeQualifier;
        public readonly string BaseType;


        /// <summary>
        /// 'bool', 'char', 'int16' etc.
        /// </summary>
        public static bool IsFundamentalType(string type) => s_FundamentalCppTypes.Contains(type);

        /// <param name="type">Can be either 'type' or 'qualifier::type'</param>
        /// <exception cref="ArgumentException"></exception>
        public CppType(string type)
        {
            // Check for type qualifier
            int index = type.LastIndexOf(':');

            if (index != -1)
            {
                if (index == 0 || index == type.Length - 1 || type[index - 1] != ':')
                {
                    throw new ArgumentException($"Got some weird type: {type}");
                }

                TypeQualifier = type.Substring(0, index - 1);
                BaseType = type.Substring(index + 1);
            }
            else
            {
                TypeQualifier = null;
                BaseType = type;
            }

            FullType = type;
        }

        public CppType(string typeQualifier, string baseType)
        {
            if (string.IsNullOrEmpty(TypeQualifier))
            {
                FullType = baseType;
                TypeQualifier = null;
                BaseType = baseType;
            }
            else
            {
                FullType = typeQualifier + "::" + baseType;
                TypeQualifier = typeQualifier;
                BaseType = baseType;
            }
        }
    }


    internal sealed class CppEnumType : CppType
    {
        public readonly string UnderlyingType;


        public CppEnumType(string baseType, string underlyingType)
            : base(baseType)
        {
            UnderlyingType = underlyingType;
        }

        public CppEnumType(string parentType, string baseType, string underlyingType)
            : base(parentType, baseType)
        {
            UnderlyingType = underlyingType;
        }
    }
}
