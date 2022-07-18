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
            public const string Final     = "final";
            public const string Inline    = "inline";
            public const string Module    = "module";
            public const string Namespace = "namespace";
            public const string Private   = "private";
            public const string Protected = "protected";
            public const string Public    = "public";
            public const string Static    = "static";
            public const string Struct    = "struct";
            public const string Template  = "template";
            public const string Virtual   = "virtual";
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
                            throw ParserError.Message(m_filePath, token, "CHT_PROPERTY macro can only be used inside CHT_STRUCT");
                        }
                    }
                    else if (token.IsValue(ChtIdentifier.Struct))
                    {
                        _ParseMacroStruct();
                    }
                    // CppIdentifiers
                    else if (token.IsValue(CppIdentifier.Class))
                    {
                        _ParseStructOrClass(false);
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
                        _ParseStructOrClass(true);
                    }
                    else if (token.IsValue(CppIdentifier.Template))
                    {
                        _ParseTemplate();
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
                    enumVariableValue = enumVariableValueToken.AsNumber();
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

        // TODO(v.matushkin): Parsing of CHT_STRUCT is lagging behind of parsing usual struct
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
                        openBrackets++;
                    }
                    else if (token.IsValue('}'))
                    {
                        if (--openBrackets == 0)
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

        private void _ParseStructOrClass(bool isStruct)
        {
            // Potential struct declaration to parse
            // struct is_basic_string<std::basic_string<Ch, Tr, Al>> final : std::true_type {};

            string type = _ParseType();

            // At this point it could be either Identifier("final") or Symbol(':'(inheritance) or ';'(forward declaration) or '{'), so I need to Peek()
            Token nextToken = PeekToken();

            // If "final" identifier (optional)
            if (nextToken.IsIdentifier(CppIdentifier.Final))
            {
                AdvanceToken();
                // Next token should be symbol, so there is no need to Peek()
                // Not using ExpectSymbol() since this is optional path and I need to check TokenType later anyway
                nextToken = NextToken();
            }

            if (nextToken.IsSymbol() == false)
            {
                throw ParserError.UnexpectedToken(m_filePath, nextToken, TokenType.Symbol);
            }

            // If inheritance (optional)
            if (nextToken.IsValue(':'))
            {
                AdvanceToken();

                // Virtual (optional)
                if (PeekToken().IsIdentifier(CppIdentifier.Virtual))
                {
                    AdvanceToken();
                }

                // Inheritance access-specifier (public, private, protected) (optional)
                Token accessToken = PeekToken();
                if (accessToken.IsIdentifier())
                {
                    string accessStr = accessToken.Value;
                    if (accessStr == CppIdentifier.Public || accessStr == CppIdentifier.Private || accessStr == CppIdentifier.Protected)
                    {
                        AdvanceToken();
                    }
                }

                _ = _ParseType();
                // At this point there could only be Symbol tokens, so use ExpectSymbol()
                nextToken = ExpectSymbol();
            }

            if (nextToken.IsValue('{'))
            {
                if (isStruct)
                {
                    m_parsingContext.PushContext_Struct();
                }
                else
                {
                    m_parsingContext.PushContext_Class();
                }

                // NOTE(v.matushkin): I think I can't just push this struct type to the type scope and there are cases where this will break things
                m_cppTypeScope.Push(type);
            }
            else if (nextToken.IsValue(';') == false)
            {
                throw ParserError.Message(m_filePath, nextToken, $"Expected either ';' or '{{' symbol, got {nextToken.AsSymbol()}");
            }

            // If there were no "final" keyword or inheritance, we are still peeking the token, so need to advance
            AdvanceToken();
        }

        // NOTE(v.matushkin): This function exists only because I don't wanna deal with usage of "class" instead of "typename" in "template<...>" expression
        /// <summary>
        /// Skip "<...>" part of the "template<...>" expression
        /// </summary>
        private void _ParseTemplate()
        {
            Token openAngleBracketToken = PeekToken();
            if (openAngleBracketToken.IsValue('<') == false)
            {
                throw ParserError.UnexpectedSymbol(m_filePath, openAngleBracketToken, '<');
            }

            _ParseTypeTemplatedPart();
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

            string variableType = _ParseType();

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

            return new CppType(variableType);

            // throw ParserError.Message(m_filePath, typeToken, "Unsupported Property type");
        }


        private string _ParseType()
        {
            return _ParseType(m_stringBuilder);
        }

        private string _ParseType(StringBuilder stringBuilder)
        {
            _ParseQualifiedIdentifier(stringBuilder);
            _ParseTypeTemplatedPart(stringBuilder);

            return stringBuilder.ToStringAndClear();
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

            while (PeekToken().IsDoubleSymbol(DoubleSymbol.Colon))
            {
                AdvanceToken();
                stringBuilder.Append(DoubleSymbol.Colon);
                stringBuilder.Append(ExpectIdentifier().Value);
            }
        }

        /// <summary>
        /// Should be called to skip templated part of the type
        /// </summary>
        private void _ParseTypeTemplatedPart()
        {
            Token openAngleBracketToken = PeekToken();

            if (openAngleBracketToken.IsValue('<') == false)
            {
                return;
            }

            AdvanceToken();
            int openAngleBrackets = 1;

            while (true)
            {
                Token token = NextToken();

                if (token.IsEndOfFile())
                {
                    throw ParserError.UnexpectedEndOfFile(m_filePath, token);
                }

                if (token.IsSymbol())
                {
                    if (token.IsValue('<'))
                    {
                        openAngleBrackets++;
                    }
                    else if (token.IsValue('>'))
                    {
                        if (--openAngleBrackets == 0)
                        {
                            break;
                        }
                    }
                }
            }

            if (openAngleBrackets != 0)
            {
                throw ParserError.Message(m_filePath, openAngleBracketToken, "Amount of '<' and '>' were not the same");
            }
        }

        /// <summary>
        /// Should be called to append templated part of the type to <see cref="StringBuilder"/><br/>
        /// Appends nothing if type was not templated
        /// </summary>
        private void _ParseTypeTemplatedPart(StringBuilder stringBuilder)
        {
            Token openAngleBracketToken = PeekToken();

            if (openAngleBracketToken.IsValue('<') == false)
            {
                return;
            }

            AdvanceToken();
            stringBuilder.Append('<');
            int openAngleBrackets = 1;

            while (true)
            {
                Token token = NextToken();

                if (token.IsEndOfFile())
                {
                    throw ParserError.UnexpectedEndOfFile(m_filePath, token);
                }

                stringBuilder.Append(token.Value);

                if (token.IsSymbol())
                {
                    if (token.IsValue('<'))
                    {
                        openAngleBrackets++;
                    }
                    else if (token.IsValue('>'))
                    {
                        if (--openAngleBrackets == 0)
                        {
                            break;
                        }
                    }
                }
            }

            if (openAngleBrackets != 0)
            {
                throw ParserError.Message(m_filePath, openAngleBracketToken, "Amount of '<' and '>' were not the same");
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
