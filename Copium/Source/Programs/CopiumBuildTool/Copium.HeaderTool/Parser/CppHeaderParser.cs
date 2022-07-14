using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

using Copium.Core;

namespace Copium.HeaderTool.Parser
{
    internal sealed class CppHeaderParser : BaseParser
    {
        private static class CppIdentifier
        {
            public const string Class     = "class";
            public const string Const     = "const";
            public const string Constexpr = "constexpr";
            public const string Enum      = "enum";
            public const string Export    = "export";
            public const string Inline    = "inline";
            public const string Module    = "module";
            public const string Namespace = "namespace";
            public const string Static    = "static";
            public const string Struct    = "struct";
        }

        private static class ChtIdentifier
        {
            public const string Class    = "CHT_CLASS";
            public const string Enum     = "CHT_ENUM";
            public const string Property = "CHT_PROPERTY";
            public const string Struct   = "CHT_STRUCT";

            public const string GeneratedBody = "CHT_GENERATED_BODY";
        }


        private readonly CppTypeScope m_cppTypeScope = new CppTypeScope();
        private readonly ParsingContext m_parsingContext = new ParsingContext();
        // For reuse
        private readonly StringBuilder m_stringBuilder = new StringBuilder();

        // CppHeaderDesc fields
        private readonly FileInfo m_headerFile;
        private string m_module;
        private readonly List<CppEnumDesc> m_cppEnums = new List<CppEnumDesc>();
        private readonly List<CppStructDesc> m_cppStructs = new List<CppStructDesc>();


        public static CppHeaderDesc Parse(FileInfo headerFile)
        {
            string headerContent = File.ReadAllText(headerFile.FullName);

            var cppHeaderParser = new CppHeaderParser(headerFile, headerContent);
            return cppHeaderParser._ParseHeader();
        }


        private CppHeaderParser(FileInfo headerFile, string headerContent)
            : base(headerFile.FullName, headerContent)
        {
            m_headerFile = headerFile;
        }


        private CppHeaderDesc _ParseHeader()
        {
            _ParseFileHeader();
            if (bEndOfStream)
            {
                return null;
            }

            _ParseFileBody();

            foreach (CppStructDesc cppStructDesc in m_cppStructs)
            {
                if (cppStructDesc.Properties.Count == 0)
                {
                    throw ParserError.Message(
                        m_filePath, cppStructDesc.GeneratedBodyMacroLine, 0, "Found CHT_STRUCT without any CHT_PROPERTY, which is useless right now.");
                }
            }

            return m_cppEnums.Count != 0 || m_cppStructs.Count != 0
                ? new CppHeaderDesc(m_headerFile, m_module, m_cppEnums, m_cppStructs)
                : null;
        }

        private void _ParseFileHeader()
        {
            Token token = PeekToken();

            // NOTE(v.matushkin): Idk if I should handle this, but the whole header could be commented out or may be it's empty
            //  May be just return null in _ParseHeader() ?
            if (token.IsEndOfFile())
            {
                return;
            }

            while (true)
            {
                token = NextToken();

                if (token.IsEndOfFile())
                {
                    throw ParserError.Message(m_filePath, token, "Couldn't find module declaration");
                }

                if (token.IsIdentifier(CppIdentifier.Export))
                {
                    Token moduleToken = NextToken();
                    if (moduleToken.IsIdentifier(CppIdentifier.Module) == false)
                    {
                        throw ParserError.Message(m_filePath, moduleToken, "First 'export' should be used for module declaration");
                    }

                    m_module = _ParseModuleDeclaration();
                    break;
                }
            }
        }

        private void _ParseFileBody()
        {
            while (true)
            {
                Token token = NextToken();

                if (token.IsEndOfFile())
                {
                    if (m_parsingContext.IsNoneContext == false)
                    {
                        throw ParserError.UnexpectedEndOfFile(m_filePath, token);
                    }
                    break;
                }

                if (token.IsIdentifier())
                {
                    // ChtIdentifiers
                    if (token.IsValue(ChtIdentifier.Class))
                    {
                        _ParseMacroClass();
                    }
                    else if (token.IsValue(ChtIdentifier.Enum))
                    {
                        _ParseMacroEnum(token.Line);
                    }
                    else if (token.IsValue(ChtIdentifier.Property))
                    {
                        if (m_parsingContext.TryGetCppStructDesc(out CppStructDesc cppStructDesc))
                        {
                            cppStructDesc.Properties.Add(_ParseMacroProperty());
                        }
                        else
                        {
                            throw ParserError.Message(m_filePath, token, "CHT_PROPERTY macro can only be used inside CHT_STRUCT or CHT_CLASS");
                        }
                    }
                    else if (token.IsValue(ChtIdentifier.Struct))
                    {
                        _ParseMacroStruct();
                    }
                    // CppIdentifiers
                    else if (token.IsValue(CppIdentifier.Class))
                    {
                        _ParseClass();
                    }
                    else if (token.IsValue(CppIdentifier.Enum))
                    {
                        _ParseEnum();
                    }
                    else if (token.IsValue(CppIdentifier.Namespace))
                    {
                        _ParseNamespace();
                    }
                    else if (token.IsValue(CppIdentifier.Struct))
                    {
                        _ParseStruct();
                    }
                }
                else if (token.IsSymbol())
                {
                    if (token.IsValue('{'))
                    {
                        _ParseDeclaration();
                    }
                    else if (token.IsValue('}'))
                    {
                        if (m_parsingContext.IsNoneContext)
                        {
                            throw ParserError.Message(m_filePath, token, "Unexpected '}' symbol");
                        }

                        m_parsingContext.PopContext();
                        m_cppTypeScope.Pop();
                    }
                }
            }
        }


        private void _ParseMacroClass()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// <para>Parse enum name, enum underlying type and enum variables</para>
        /// Should be called after <see cref="ChtIdentifier.Enum"/> macro was parsed
        /// </summary>
        private void _ParseMacroEnum(int enumMacroLine)
        {
            ExpectSymbol('(');
            ExpectSymbol(')');

            ExpectIdentifier(CppIdentifier.Enum);
            ExpectIdentifier(CppIdentifier.Class);

            Token enumNameToken = ExpectIdentifier();

            ExpectSymbol(':'); // TODO(v.matushkin): Type shouldn't be necessary

            Token enumUnderlyingTypeToken = ExpectIdentifier();

            ExpectSymbol('{');

            var cppEnumDesc = new CppEnumDesc(new CppEnumType(enumNameToken.Value, enumUnderlyingTypeToken.Value), enumMacroLine);

            long prevEnumVariableValue = -1;
            Token enumVariableNameToken = ExpectIdentifier();

            for (; enumVariableNameToken.IsIdentifier(); enumVariableNameToken = NextToken())
            {
                string enumVariableValueStr;
                long enumVariableValue;

                if (PeekToken().IsSymbol('='))
                {
                    AdvanceToken();
                    Token enumVariableValueToken = ExpectNumber();
                    enumVariableValueStr = enumVariableValueToken.Value;
                    enumVariableValue = enumVariableValueToken.GetNumber();
                }
                else
                {
                    enumVariableValue = prevEnumVariableValue + 1;
                    enumVariableValueStr = enumVariableValue.ToString();
                }
                prevEnumVariableValue = enumVariableValue;

                ExpectSymbol(',');

                cppEnumDesc.Variables.Add(new CppEnumDesc.CppEnumVariable(enumVariableNameToken.Value, enumVariableValueStr));
            }

            if (enumVariableNameToken.IsSymbol('}') == false)
            {
                throw ParserError.Message(m_filePath, enumVariableNameToken, "Expected '}' at the end of the enum definition");
            }

            ExpectSymbol(';');

            m_cppEnums.Add(cppEnumDesc);
        }

        /// <summary>
        /// <para>Parse property type and name</para>
        /// Should be called after <see cref="ChtIdentifier.Property"/> macro was parsed
        /// </summary>
        private CppPropertyDesc _ParseMacroProperty()
        {
            ExpectSymbol('(');
            ExpectSymbol(')');

            CppType propertyType = _ParseVariableType();
            string propertyName = ExpectIdentifier().Value;

            ExpectSymbol(';');

            return new CppPropertyDesc(propertyName, propertyType);
        }

        /// <summary>
        /// <para>
        ///   Parse struct name and type qualifier(optional)<br/>
        ///   Adds it to <see cref="m_cppTypeScope"/> and <see cref="m_parsingContext"/>
        /// </para>
        /// Should be called after <see cref="ChtIdentifier.Struct"/> macro was parsed
        /// </summary>
        private void _ParseMacroStruct()
        {
            ExpectSymbol('(');
            ExpectSymbol(')');
            ExpectIdentifier(CppIdentifier.Struct);

            Token structNameToken = ExpectIdentifier();

            ExpectSymbol('{');

            int generatedBodyMacroLine = ExpectIdentifier(ChtIdentifier.GeneratedBody).Line;
            ExpectSymbol('(');
            ExpectSymbol(')');

            var structCppType = new CppType(m_cppTypeScope.GetCurrent(), structNameToken.Value);
            var cppStructDesc = new CppStructDesc(structCppType, generatedBodyMacroLine);
            m_cppStructs.Add(cppStructDesc);

            m_parsingContext.PushContext(cppStructDesc);
            m_cppTypeScope.Push(structCppType.BaseType);
        }


        private void _ParseClass()
        {
            Token classNameToken = ExpectIdentifier();

            Token nextToken = PeekToken();
            if (nextToken.IsSymbol(';'))
            {
                AdvanceToken();
                return;
            }
            if (nextToken.IsSymbol('{') == false)
            {
                if (nextToken.IsIdentifier("final"))
                {
                    AdvanceToken();
                    nextToken = NextToken();
                }

                if (nextToken.IsSymbol(':'))
                {
                    // TODO(v.matushkin): Support nested class type and 'virtual'
                    ExpectIdentifier(); // inheritance access-specifier
                    ExpectIdentifier(); // inheritance class

                    nextToken = NextToken();
                }

                if (nextToken.IsSymbol('{') == false)
                {
                    throw ParserError.Message(m_filePath, nextToken, "Expected \"class-inheritance(optional) '{'\"");
                }
            }
            else
            {
                AdvanceToken();
            }

            m_parsingContext.PushContext_Class();
            m_cppTypeScope.Push(classNameToken.Value);
        }

        private void _ParseDeclaration()
        {
            int openBrackets = 1;

            while (true)
            {
                Token token = NextToken();

                if (token.IsEndOfFile())
                {
                    throw ParserError.UnexpectedEndOfFile(m_filePath, token);
                }

                if (token.IsSymbol())
                {
                    if (token.IsValue('{'))
                    {
                        ++openBrackets;
                    }
                    else if (token.IsValue('}'))
                    {
                        --openBrackets;
                        if (openBrackets == 0)
                        {
                            break;
                        }
                    }
                }
            }
        }

        /// <summary>
        /// Parse enum and its variables
        /// </summary>
        private void _ParseEnum()
        {
            ExpectIdentifier(CppIdentifier.Class);
            //- Enum name
            ExpectIdentifier();
            //- Enum type(optional)
            if (PeekToken().IsSymbol(':'))
            {
                AdvanceToken();
                ExpectIdentifier();
            }
            //- Enum variables
            ExpectSymbol('{');
            for (Token token = NextToken(); token.IsSymbol('}') == false; token = NextToken())
            {
                if (token.IsEndOfFile())
                {
                    throw ParserError.UnexpectedEndOfFile(m_filePath, token);
                }
            }
            ExpectSymbol(';');
        }

        // TODO(v.matushkin): What about unnamed namespaces?
        private void _ParseNamespace()
        {
            string namespaceName = _ParseQualifiedIdentifier();

            ExpectSymbol('{');

            m_parsingContext.PushContext_Namespace();
            m_cppTypeScope.Push(namespaceName);
        }

        // TODO(v.matushkin): Parse inheritance and '{' ?
        //  What about forward declarations?
        private void _ParseStruct()
        {
            Token structNameToken = ExpectIdentifier();

            Token nextToken = PeekToken();
            if (nextToken.IsSymbol(';'))
            {
                AdvanceToken();
                return;
            }
            if (nextToken.IsSymbol('{') == false)
            {
                if (nextToken.IsIdentifier("final"))
                {
                    AdvanceToken();
                    nextToken = NextToken();
                }

                if (nextToken.IsSymbol(':'))
                {
                    // TODO(v.matushkin): Support nested struct type and 'virtual'
                    ExpectIdentifier(); // inheritance access-specifier
                    ExpectIdentifier(); // inheritance class

                    nextToken = NextToken();
                }

                if (nextToken.IsSymbol('{') == false)
                {
                    throw ParserError.Message(m_filePath, nextToken, "Expected \"struct-inheritance(optional) '{'\"");
                }
            }
            else
            {
                AdvanceToken();
            }

            m_parsingContext.PushContext_Struct();
            m_cppTypeScope.Push(structNameToken.Value);
        }


        /// <summary>
        /// Parse module declaration, eg. expressions like 'A.ModuleB.ModuleB'
        /// </summary>
        private string _ParseModuleDeclaration()
        {
            m_stringBuilder.Append(ExpectIdentifier().Value);

            while (PeekToken().IsSymbol('.'))
            {
                AdvanceToken();
                m_stringBuilder.Append('.').Append(ExpectIdentifier().Value);
            }

            ExpectSymbol(';');

            return m_stringBuilder.ToStringAndClear();
        }

        private CppType _ParseVariableType()
        {
            void ThrowIfVariableSpecifier(Token token)
            {
                if (_IsVariableSpecifier(token.Value))
                {
                    throw ParserError.Message(m_filePath, token, $"Properties with declaration specifiers('{token.Value}') are not supported");
                }
            }

            Token typeToken = PeekToken();
            if (typeToken.IsIdentifier() == false)
            {
                throw ParserError.UnexpectedToken(m_filePath, typeToken, TokenType.Identifier);
            }

            ThrowIfVariableSpecifier(typeToken);

            if (CppType.IsFundamentalType(typeToken.Value))
            {
                AdvanceToken();
                return new CppType(typeToken.Value);
            }

            _ParseQualifiedIdentifier(m_stringBuilder);

            int angleBrackets = 0;
            while (PeekToken().IsSymbol('<'))
            {
                AdvanceToken();
                ++angleBrackets;
                m_stringBuilder.Append('<');

                _ParseQualifiedIdentifier(m_stringBuilder);
                while (PeekToken().IsSymbol(','))
                {
                    AdvanceToken();
                    m_stringBuilder.Append('<');
                    _ParseQualifiedIdentifier(m_stringBuilder);
                }

                if (PeekToken().IsSymbol('>'))
                {
                    m_stringBuilder.Append('>');
                    --angleBrackets;
                    if (angleBrackets == 0)
                    {
                        break;
                    }
                }
            }

            if (angleBrackets != 0)
            {
                throw ParserError.Message(m_filePath, typeToken, "Amount of '<' and '>' were not the same");
            }

            Token tokenAfterType = PeekToken();
            switch (tokenAfterType.Type)
            {
                case TokenType.Symbol:
                    if (tokenAfterType.IsValue('*') || tokenAfterType.IsValue('&'))
                    {
                        throw ParserError.Message(m_filePath, tokenAfterType, "Pointer and reference properties are not supported");
                    }
                    break;
                case TokenType.Identifier:
                    ThrowIfVariableSpecifier(tokenAfterType);
                    break;
            }

            return new CppType(m_stringBuilder.ToStringAndClear());

            // throw ParserError.Message(m_filePath, typeToken, "Unsupported Property type");
        }


        /// <summary>
        /// Parse expressions like 'type' or 'namespace::namespace::class::type'<br/>
        /// Expressions starting with '::' are not supported
        /// </summary>
        private string _ParseQualifiedIdentifier()
        {
            _ParseQualifiedIdentifier(m_stringBuilder);

            return m_stringBuilder.ToStringAndClear();
        }

        /// <summary>
        /// Same as <see cref="_ParseQualifiedIdentifier"/> but uses provided <see cref="StringBuilder"/> to append identifier
        /// </summary>
        private void _ParseQualifiedIdentifier(StringBuilder stringBuilder)
        {
            stringBuilder.Append(ExpectIdentifier().Value);

            while (PeekToken().IsSymbol(':'))
            {
                AdvanceToken();
                ExpectSymbol(':');
                stringBuilder.Append(ExpectIdentifier().Value);
            }
        }


        private static bool _IsVariableSpecifier(string str)
        {
            return str == CppIdentifier.Const
                || str == CppIdentifier.Constexpr
                || str == CppIdentifier.Inline
                || str == CppIdentifier.Static;
        }
    }
}
