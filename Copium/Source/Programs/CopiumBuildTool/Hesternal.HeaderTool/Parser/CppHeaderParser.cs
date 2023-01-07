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

        private static class ChtFlag
        {
            public const string Abstract = "abstract";
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
        private readonly List<CppClassDesc> m_cppClasses = new List<CppClassDesc>();


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
            // If header is empty or if it containts only whitespaces/comments
            if (bEndOfStream || PeekToken().IsEndOfFile())
            {
                return null;
            }

            _ParseFileHeader();
            _ParseFileBody();

            foreach (CppStructDesc cppStructDesc in m_cppStructs)
            {
                if (cppStructDesc.Properties.Count == 0)
                {
                    throw ParserError.Message(
                        m_filePath, cppStructDesc.GeneratedBodyMacroLine, 0, "Found CHT_STRUCT without any CHT_PROPERTY, which is useless right now.");
                }
            }

            return m_cppEnums.Count != 0 || m_cppStructs.Count != 0 || m_cppClasses.Count != 0
                ? new CppHeaderDesc(m_headerFile, m_module, m_cppEnums, m_cppStructs, m_cppClasses)
                : null;
        }

        private void _ParseFileHeader()
        {
            while (true)
            {
                Token token = NextToken();

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
                        _ParseMacroStructOrClass(false);
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
                        else if (m_parsingContext.TryGetCppClassDesc(out CppClassDesc cppClassDesc))
                        {
                            cppClassDesc.Properties.Add(_ParseMacroProperty());
                        }
                        else
                        {
                            throw ParserError.Message(m_filePath, token, "CHT_PROPERTY macro can only be used inside CHT_STRUCT or CHT_CLASS");
                        }
                    }
                    else if (token.IsValue(ChtIdentifier.Struct))
                    {
                        _ParseMacroStructOrClass(true);
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

        /// <summary>
        /// <para>
        ///   Parse struct/class name and type qualifier(optional)<br/>
        ///   Adds it to <see cref="m_cppTypeScope"/> and <see cref="m_parsingContext"/>
        /// </para>
        /// Should be called after <see cref="ChtIdentifier.Struct"/> or <see cref="ChtIdentifier.Class"/> macro was parsed
        /// </summary>
        private void _ParseMacroStructOrClass(bool isStruct)
        {
            ClassFlags classFlags = _ParseClassFlags();
            ExpectIdentifier(isStruct ? CppIdentifier.Struct : CppIdentifier.Class);

            Token nameToken = ExpectIdentifier();

            string baseClass = _ParseInheritance();

            ExpectSymbol('{');

            int generatedBodyMacroLine = ExpectIdentifier(ChtIdentifier.GeneratedBody).Line;
            ExpectSymbol('(');
            ExpectSymbol(')');

            var cppType = new CppType(m_cppTypeScope.GetCurrent(), nameToken.Value);

            if (isStruct)
            {
                var cppStructDesc = new CppStructDesc(cppType, generatedBodyMacroLine);
                m_cppStructs.Add(cppStructDesc);
                m_parsingContext.PushContext(cppStructDesc);
            }
            else
            {
                var cppClassDesc = new CppClassDesc(cppType, baseClass, classFlags, generatedBodyMacroLine);
                m_cppClasses.Add(cppClassDesc);
                m_parsingContext.PushContext(cppClassDesc);
            }

            m_cppTypeScope.Push(cppType.BaseType);
        }


        private ClassFlags _ParseClassFlags()
        {
            ExpectSymbol('(');

            ClassFlags classFlags = ClassFlags.None;

            if (PeekToken().IsIdentifier(ChtFlag.Abstract))
            {
                AdvanceToken();
                classFlags = ClassFlags.Abstract;
            }

            ExpectSymbol(')');

            return classFlags;
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

        private void _ParseNamespace()
        {
            string namespaceName = null;

            if (PeekToken().IsIdentifier())
            {
                namespaceName = _ParseQualifiedIdentifier();
            }

            Token token = ExpectSymbol();

            // Most likely it was 'using namespace Name;', skip.
            if (token.IsValue(';'))
            {
            }
            else if (token.IsValue('{'))
            {
                // If it was anonymous namespace
                if (namespaceName == null)
                {
                    // NOTE(v.matushkin): I don't know how should I handle anonymous namespace, since it doesn't have a name,
                    //  but right now I'm not using these names anyway. May be just assign empty string,
                    //  or doesn't push anything to type scope at all?
                    namespaceName = "ANON";
                }

                m_parsingContext.PushContext_Namespace();
                m_cppTypeScope.Push(namespaceName);
            }
            else
            {
                throw ParserError.Message(m_filePath, token, $"Expected ';' or '{{' symbol, got '{token.AsSymbol()}'");
            }
        }

        private void _ParseStructOrClass(bool isStruct)
        {
            // Potential declaration to parse
            // struct is_basic_string<std::basic_string<Ch, Tr, Al>> final : virtual public std::true_type {};

            string type = _ParseType();

            _ = _ParseInheritance();

            Token token = NextToken();

            if (token.IsValue('{'))
            {
                if (isStruct)
                {
                    m_parsingContext.PushContext_Struct();
                }
                else
                {
                    m_parsingContext.PushContext_Class();
                }

                // NOTE(v.matushkin): I think I can't just push this struct/class type to the type scope and there are cases where this will break things
                m_cppTypeScope.Push(type);
            }
            else if (token.IsValue(';') == false)
            {
                throw ParserError.Message(m_filePath, token, $"Expected either ';' or '{{' symbol, got {token.Value}");
            }
        }

        // NOTE(v.matushkin): This function exists only because I don't wanna deal with usage of "class" instead of "typename" in "template<class ...>" expression
        /// <summary>
        /// Skip "<...>" part of the "template<...>" expression
        /// </summary>
        private void _ParseTemplate()
        {
            // Skip stuff that I don't wanna handle, e.g. template instantiation 'extern template struct TRect<int32>;'
            if (PeekToken().IsValue('<'))
            {
                _ParseTypeTemplatedPart();
            }
        }


        /// <summary>
        /// Parse module declaration, eg. expressions like 'A.ModuleB.ModuleC' or 'A.ModuleB:Partition'
        /// </summary>
        private string _ParseModuleDeclaration()
        {
            m_stringBuilder.Append(ExpectIdentifier().Value);

            while (PeekToken().IsSymbol('.'))
            {
                AdvanceToken();
                m_stringBuilder.Append('.').Append(ExpectIdentifier().Value);
            }

            // Check for module partition
            if (PeekToken().IsSymbol(':'))
            {
                AdvanceToken();
                m_stringBuilder.Append(':').Append(ExpectIdentifier().Value);
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


        // TODO(v.matushkin): Multiple inheritance
        // TODO(v.matushkin): Template inheritance
        private string _ParseInheritance()
        {
            string baseClass = null;

            // At this point it could be either Identifier("final") or Symbol(':'(inheritance) or ';'(forward declaration) or '{'), so I need to Peek()
            Token nextToken = PeekToken();

            // If "final" identifier (optional)
            if (nextToken.IsIdentifier(CppIdentifier.Final))
            {
                AdvanceToken();
                // Next token should be symbol, so there is no need to Peek()
                // Not using ExpectSymbol() since this is optional path and I need to check TokenType later anyway
                nextToken = PeekToken();
            }

            if (nextToken.IsSymbol() == false)
            {
                throw ParserError.UnexpectedToken(m_filePath, CurrentToken, TokenType.Symbol);
            }

            // If inheritance (optional)
            if (nextToken.IsValue(':'))
            {
                AdvanceToken();

                // virtual (optional)
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

                baseClass = _ParseType();
            }

            return baseClass;
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
