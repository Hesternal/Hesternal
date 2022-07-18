using System;
using System.Text;
using Copium.Core;

namespace Copium.HeaderTool.Parser
{
    internal enum TokenType
    {
        None,
        Identifier,
        Symbol,
        DoubleSymbol,
        Number,
        String,
        EndOfFile,
    }

    internal static class DoubleSymbol
    {
        public const string Colon = "::";
    }

    internal sealed class Token
    {
        public readonly string Value;
        public readonly int Line;
        public readonly int Column;
        public readonly TokenType Type;

        public Token(string value, int line, int column, TokenType type)
        {
            Value = value;
            Line = line;
            Column = column;
            Type = type;
        }

        public bool IsValue(char symbol)            => Value[0] == symbol;
        public bool IsValue(string str)             => str == Value;
        public bool IsSymbol()                      => Type == TokenType.Symbol;
        public bool IsSymbol(char symbol)           => IsSymbol() && IsValue(symbol);
        public bool IsDoubleSymbol()                => Type == TokenType.DoubleSymbol;
        public bool IsDoubleSymbol(string symbol)   => IsDoubleSymbol() && IsValue(symbol);
        public bool IsIdentifier()                  => Type == TokenType.Identifier;
        public bool IsIdentifier(string identifier) => IsIdentifier() && IsValue(identifier);
        public bool IsEndOfFile()                   => Type == TokenType.EndOfFile;

        public char AsSymbol() => Value[0];
        public long AsNumber() => long.Parse(Value);
    }


    internal class BaseParser
    {
        // For reuse
        private readonly StringBuilder m_stringBuilder = new StringBuilder();

        private CharEnumerator m_input;
        private char m_inputCurrent;
        private Token m_peekToken = null;
        private int m_line = 1;
        private int m_column = 0;

        protected readonly string m_filePath;

        protected bool bEndOfStream { get; private set; } = false;
        protected Token CurrentToken { get; private set; } = new Token(string.Empty, 0, 0, TokenType.None);
        protected Token PreviousToken { get; private set; } = new Token(string.Empty, 0, 0, TokenType.None);


        protected BaseParser(string filePath, string input)
        {
            m_filePath = filePath;
            m_input = input.GetEnumerator();
            _ = _MoveNext();
        }


        protected Token NextToken()
        {
            PreviousToken = CurrentToken;
            return CurrentToken = _NextTokenInternal();
        }

        private Token _NextTokenInternal()
        {
            if (bEndOfStream)
            {
                throw new InvalidOperationException("Calling NextToken when the end of stream was reached");
            }

            if (m_peekToken != null)
            {
                Token token = m_peekToken;
                m_peekToken = null;

                return token;
            }

            _SkipWhitespaceAndComments();

            if (bEndOfStream)
            {
                return new Token(string.Empty, m_line, m_column, TokenType.EndOfFile);
            }

            char currentChar = m_inputCurrent;

            if (char.IsLetter(currentChar) || currentChar == '_')
            {
                return _ParseIdentifier();
            }
            else if (_IsNumber(currentChar))
            {
                return _ParseNumber();
            }
            else if (currentChar == '"')
            {
                return _ParseString();
            }

            int tokenColumn = m_column++;
            bool notTheEnd = _MoveNext();

            if (currentChar == ':' && notTheEnd)
            {
                if (m_inputCurrent == ':')
                {
                    m_column++;
                    _ = _MoveNext();

                    return new Token(DoubleSymbol.Colon, m_line, tokenColumn, TokenType.DoubleSymbol);
                }
            }

            return new Token(currentChar.ToString(), m_line, tokenColumn, TokenType.Symbol);
        }

        protected Token PeekToken()
        {
            if (m_peekToken != null)
            {
                return m_peekToken;
            }

            m_peekToken = NextToken();

            return m_peekToken;
        }

        protected void AdvanceToken()
        {
            m_peekToken = null;
        }


        protected Token ExpectToken(TokenType tokenType)
        {
            Token token = NextToken();
            if (token.Type != tokenType)
            {
                throw ParserError.UnexpectedToken(m_filePath, token, tokenType);
            }

            return token;
        }

        protected Token ExpectIdentifier() => ExpectToken(TokenType.Identifier);
        protected Token ExpectSymbol()     => ExpectToken(TokenType.Symbol);
        protected Token ExpectNumber()     => ExpectToken(TokenType.Number);

        protected Token ExpectSymbol(char symbol)
        {
            Token token = ExpectSymbol();
            if (token.IsValue(symbol) == false)
            {
                throw ParserError.UnexpectedSymbol(m_filePath, token, symbol);
            }

            return token;
        }

        protected Token ExpectIdentifier(string identifier)
        {
            Token token = ExpectIdentifier();
            if (token.IsValue(identifier) == false)
            {
                throw ParserError.UnexpectedIdentifier(m_filePath, token, identifier);
            }

            return token;
        }


        private bool _IsNumber(char currentChar)
        {
            if (char.IsDigit(currentChar))
                return true;

            if (currentChar == '-' || currentChar == '+')
            {
                var nextChar = (CharEnumerator)m_input.Clone();
                if (nextChar.MoveNext() && char.IsDigit(nextChar.Current))
                {
                    return true;
                }
            }

            return false;
        }


        private void _SkipWhitespaceAndComments()
        {
            var inputPrevious = (CharEnumerator)m_input.Clone();

            do
            {
                // WhiteSpace
                if (m_inputCurrent == ' ' || m_inputCurrent == '\t' || m_inputCurrent == '\r')
                {
                    m_column++;
                }
                // NewLine
                else if (m_inputCurrent == '\n')
                {
                    m_column = 0;
                    m_line++;
                }
                // Comment
                else if (m_inputCurrent == '/')
                {
                    if (_MoveNext())
                    {
                        // Single line comment "//"
                        if (m_inputCurrent == '/')
                        {
                            m_column++;
                            while (_MoveNext())
                            {
                                if (m_inputCurrent == '\n')
                                {
                                    m_column = 0;
                                    m_line++;
                                    break;
                                }
                                m_column++; // NOTE(v.matushkin): No need to count columns since we are skipping until new line anyway?
                            }
                        }
                        // C-style multi-lines comment "/**/"
                        else if (m_inputCurrent == '*')
                        {
                            m_column++;
                            bool foundCommentEnd = false;

                            while (_MoveNext())
                            {
                                if (m_inputCurrent == '*')
                                {
                                    if (_MoveNext())
                                    {
                                        if (m_inputCurrent == '/')
                                        {
                                            _MoveNext();
                                            m_column++;
                                            foundCommentEnd = true;
                                            break;
                                        }
                                    }
                                    else
                                    {
                                        throw ParserError.UnexpectedEndOfFile(m_filePath, m_line, m_column);
                                    }
                                }
                                else if (m_inputCurrent == '\n')
                                {
                                    m_column = 0;
                                    m_line++;
                                }
                                else
                                {
                                    m_column++;
                                }
                            }

                            if (foundCommentEnd)
                            {
                                throw ParserError.Message(m_filePath, m_line, m_column, "Couldn't find the end of a multi line comment");
                            }
                        }
                    }
                    // Reached the EndOfStream, it was not a comment, just a '/' symbol, backtrack and stop
                    else
                    {
                        m_input = inputPrevious;
                        break;
                    }
                }
                // TODO(v.matushkin): Do I even need to backtrack here?
                // Stop, current symbol is not a whitespace
                else
                {
                    m_input = inputPrevious;
                    break;
                }

                inputPrevious = m_input;

            } while (_MoveNext());
        }


        private Token _ParseIdentifier()
        {
            // NOTE(v.matushkin): No need to append chars one by one,
            //  but I see no way to create string from 2 CharEnumerators and I'm lazy to think of anything else
            int tokenColumn = ++m_column;

            m_stringBuilder.Append(m_inputCurrent);

            while (_MoveNext() && (char.IsLetterOrDigit(m_inputCurrent) || m_inputCurrent == '_'))
            {
                m_stringBuilder.Append(m_inputCurrent);
                m_column++;
            }

            return new Token(m_stringBuilder.ToStringAndClear(), m_line, tokenColumn, TokenType.Identifier);
        }

        private Token _ParseNumber()
        {
            int tokenColumn = m_column++;

            m_stringBuilder.Append(m_inputCurrent);

            if (m_inputCurrent == '-' || m_inputCurrent == '+')
            {
                _ = _MoveNext();
                m_stringBuilder.Append(m_inputCurrent);
                m_column++;
            }

            while (_MoveNext() && char.IsDigit(m_inputCurrent))
            {
                m_stringBuilder.Append(m_inputCurrent);
                m_column++;
            }

            return new Token(m_stringBuilder.ToStringAndClear(), m_line, tokenColumn, TokenType.Number);
        }

        private Token _ParseString()
        {
            // Skip '"' symbol
            if (_MoveNext() == false)
            {
                throw ParserError.UnexpectedEndOfFile(m_filePath, m_line, m_column);
            }

            int tokenColumn = ++m_column;

            m_stringBuilder.Append(m_inputCurrent);

            bool foundStringEnd = false;

            while (_MoveNext())
            {
                if (m_inputCurrent == '"')
                {
                    foundStringEnd = true;
                    break;
                }

                m_stringBuilder.Append(m_inputCurrent);
                m_column++;

                // NOTE(v.matushkin): I think this checks for '\\' and '\0' are bugged
                if (m_inputCurrent == '\\')
                {
                    if (_MoveNext() == false)
                    {
                        throw ParserError.UnexpectedEndOfFile(m_filePath, m_line, m_column);
                    }

                    m_stringBuilder.Append(m_inputCurrent);
                    m_column++;
                    if (m_inputCurrent == '\0')
                    {
                        break;
                    }
                }
            }

            if (foundStringEnd == false)
            {
                throw ParserError.Message(m_filePath, m_line, m_column, "Couldn't find the end of a string");
            }
            // Skip '"' symbol
            if (_MoveNext() == false)
            {
                throw ParserError.UnexpectedEndOfFile(m_filePath, m_line, m_column);
            }

            m_column++;

            return new Token(m_stringBuilder.ToStringAndClear(), m_line, tokenColumn, TokenType.String);
        }


        private bool _MoveNext()
        {
            bool notTheEnd = m_input.MoveNext();

            if (notTheEnd)
            {
                bEndOfStream = false;
                m_inputCurrent = m_input.Current;
            }
            else
            {
                bEndOfStream = true;
                m_inputCurrent = char.MaxValue;
            }

            return notTheEnd;
        }
    }
}
