module;

#include <fmt/format.h>

module CopiumEditor.Assets.ShaderImporter.BaseParser;


import CopiumEngine.Utils.EnumUtils;

import <cctype>;
import <string_view>;


namespace
{

    using namespace Copium;


    // NOTE(v.matushkin): Idk ??? It's actully valid, may be make it's type int16 or something,
    //  so it can hold negative value to indicate that it's invalid.
    static constexpr char   k_InvalidSymbol    = '\0';
    static constexpr uint32 k_StartColumnValue = 1;


    [[nodiscard]] static bool IsDigit(char symbol)
    {
        return std::isdigit(static_cast<unsigned char>(symbol));
    }

    [[nodiscard]] static bool IsLetter(char symbol)
    {
        return std::isalpha(static_cast<unsigned char>(symbol));
    }

    [[nodiscard]] static bool IsLetterOrDigit(char symbol)
    {
        return std::isalnum(static_cast<unsigned char>(symbol));
    }

} // namespace


namespace Copium
{

    BaseParser::BaseParser(std::string_view input)
        : m_peekToken(std::nullopt)
        , m_currentToken(Token::None())
        , m_previousToken(Token::None())
        , m_inputCurrent(input.cbegin())
        , m_inputEnd(input.cend())
        , m_line(1)
        , m_column(k_StartColumnValue)
    {
        m_endOfStream = m_inputCurrent == m_inputEnd;
        m_currentSymbol = m_endOfStream ? k_InvalidSymbol : *m_inputCurrent;
    }


    BaseParser::Token BaseParser::NextToken()
    {
        m_previousToken = m_currentToken;
        return m_currentToken = _NextTokenInternal();
    }

    BaseParser::Token BaseParser::PeekToken()
    {
        if (m_peekToken.has_value() == false)
        {
            m_peekToken = NextToken();
        }

        return *m_peekToken;
    }

    void BaseParser::AdvanceToken()
    {
        m_peekToken.reset();
    }


    BaseParser::Token BaseParser::ExpectToken(TokenType tokenType)
    {
        Token token = NextToken();
        if (token.Type != tokenType)
        {
            throw Error_UnexpectedToken(token, tokenType);
        }

        return token;
    }

    BaseParser::Token BaseParser::ExpectSymbol(char symbol)
    {
        Token token = ExpectSymbol();
        if (token.IsValue(symbol) == false)
        {
            throw Error_UnexpectedSymbol(token, symbol);
        }

        return token;
    }

    BaseParser::Token BaseParser::ExpectIdentifier(std::string_view identifier)
    {
        Token token = ExpectIdentifier();
        if (token.IsValue(identifier) == false)
        {
            throw Error_UnexpectedIdentifier(token, identifier);
        }

        return token;
    }


    BaseParser::Token BaseParser::_NextTokenInternal()
    {
        if (m_endOfStream)
        {
            throw ParserError("Calling NextToken when the end of stream was reached");
        }

        if (m_peekToken.has_value())
        {
            const Token token = *m_peekToken;
            m_peekToken.reset();

            return token;
        }

        _SkipWhitespaceAndComments();

        if (m_endOfStream)
        {
            return Token(std::string_view(), m_line, m_column, TokenType::EndOfFile);
        }

        if (IsLetter(m_currentSymbol) || m_currentSymbol == '_')
        {
            return _ParseIdentifier();
        }
        else if (_IsNumber())
        {
            return _ParseNumber();
        }
        else if (m_currentSymbol == '"')
        {
            return _ParseString();
        }

        const ParserIterator tokenBegin = m_inputCurrent;
        const uint32 tokenColumn = m_column++;

        const char previousSymbol = m_currentSymbol;
        const bool notTheEnd = _MoveNext();

        if (previousSymbol == ':' && notTheEnd)
        {
            if (m_currentSymbol == ':')
            {
                m_column++;
                _MoveNext();

                return Token(_MakeStringView(tokenBegin, m_inputCurrent), m_line, tokenColumn, TokenType::DoubleSymbol);
            }
        }

        return Token(_MakeStringView(tokenBegin, m_inputCurrent), m_line, tokenColumn, TokenType::Symbol);
    }


    void BaseParser::_SkipWhitespaceAndComments()
    {
        do
        {
            // Whitespace
            if (m_currentSymbol == ' ' || m_currentSymbol == '\t' || m_currentSymbol == '\r')
            {
                m_column++;
            }
            // NewLine
            else if (m_currentSymbol == '\n')
            {
                m_column = k_StartColumnValue;
                m_line++;
            }
            // Comment
            else if (m_currentSymbol == '/')
            {
                const ParserIterator inputNext = m_inputCurrent + 1;
                if (inputNext != m_inputEnd)
                {
                    // Single line comment "//"
                    if (*inputNext == '/')
                    {
                        _MoveNext();
                        m_column++;

                        while (_MoveNext())
                        {
                            if (m_currentSymbol == '\n')
                            {
                                m_column = k_StartColumnValue;
                                m_line++;
                                break;
                            }
                            m_column++; // NOTE(v.matushkin): No need to count columns since we are skipping until new line anyway?
                        }
                    }
                    // C-style multi-lines comment "/**/"
                    else if (*inputNext == '*')
                    {
                        _MoveNext();
                        m_column++;
                        bool foundCommentEnd = false;

                        while (_MoveNext())
                        {
                            if (m_currentSymbol == '*')
                            {
                                if (_MoveNext())
                                {
                                    if (m_currentSymbol == '/')
                                    {
                                        _MoveNext();
                                        m_column++;
                                        foundCommentEnd = true;
                                        break;
                                    }
                                }
                                else
                                {
                                    throw Error_UnexpectedEndOfFile(m_line, m_column);
                                }
                            }
                            else if (m_currentSymbol == '\n')
                            {
                                m_column = k_StartColumnValue;
                                m_line++;
                            }
                            else
                            {
                                m_column++;
                            }
                        }

                        if (foundCommentEnd)
                        {
                            throw Error_Message("Couldn't find the end of a multi line comment", m_line, m_column);
                        }
                    }
                    // It was not a comment, just a '/' symbol
                    else
                    {
                        break;
                    }
                }
                // Reached the EndOfStream, it was not a comment, just a '/' symbol
                else
                {
                    break;
                }
            }
            // Stop, current symbol is not a whitespace
            else
            {
                break;
            }

        } while (_MoveNext());
    }


    bool BaseParser::_IsNumber()
    {
        char symbolToCheck;

        if (m_currentSymbol == '-' || m_currentSymbol == '+')
        {
            const ParserIterator inputNext = m_inputCurrent + 1;
            if (inputNext == m_inputEnd)
            {
                // It was just a '-'/'+' symbol, not a number
                return false;
            }

            symbolToCheck = *inputNext;
        }
        else
        {
            symbolToCheck = m_currentSymbol;
        }

        if (IsDigit(symbolToCheck))
        {
            return true;
        }

        return false;
    }


    BaseParser::Token BaseParser::_ParseIdentifier()
    {
        const ParserIterator tokenBegin = m_inputCurrent;
        const uint32 tokenColumn = m_column++;

        while (_MoveNext() && (IsLetterOrDigit(m_currentSymbol) || m_currentSymbol == '_'))
        {
            m_column++;
        }

        return Token(_MakeStringView(tokenBegin, m_inputCurrent), m_line, tokenColumn, TokenType::Identifier);
    }

    BaseParser::Token BaseParser::_ParseNumber()
    {
        const ParserIterator tokenBegin = m_inputCurrent;
        const uint32 tokenColumn = m_column++;

        if (m_currentSymbol == '-' || m_currentSymbol == '+')
        {
            // No need to check for EndOfStream, it was already done in _IsNumber()
            _MoveNext();
            m_column++;
        }

        while (_MoveNext() && IsDigit(m_currentSymbol))
        {
            m_column++;
        }

        return Token(_MakeStringView(tokenBegin, m_inputCurrent), m_line, tokenColumn, TokenType::Number);
    }

    BaseParser::Token BaseParser::_ParseString()
    {
        // Skip '"' symbol
        if (_MoveNext() == false)
        {
            throw Error_UnexpectedEndOfFile(m_line, m_column);
        }

        const ParserIterator tokenBegin = m_inputCurrent;
        const uint32 tokenColumn = ++m_column;

        bool foundStringEnd = false;
        bool notEscapedChar = true;

        while (_MoveNext())
        {
            if (notEscapedChar && m_currentSymbol == '"')
            {
                foundStringEnd = true;
                break;
            }

            m_column++;
            // NOTE(v.matushkin): Do I even need to handle this? I think not
            notEscapedChar = m_currentSymbol != '\\';
        }

        if (foundStringEnd == false)
        {
            throw Error_Message("Couldn't find the end of a string literal", m_line, tokenColumn);
        }

        // Skip '"' symbol
        if (_MoveNext() == false)
        {
            throw Error_UnexpectedEndOfFile(m_line, m_column);
        }
        m_column++;

        return Token(_MakeStringView(tokenBegin, m_inputCurrent), m_line, tokenColumn, TokenType::String);
    }


    bool BaseParser::_MoveNext()
    {
        bool notTheEnd = ++m_inputCurrent != m_inputEnd;

        if (notTheEnd)
        {
            m_endOfStream = false;
            m_currentSymbol = *m_inputCurrent;
        }
        else
        {
            m_endOfStream = true;
            m_currentSymbol = k_InvalidSymbol;
        }

        return notTheEnd;
    }


    std::string_view BaseParser::_MakeStringView(ParserIterator begin, ParserIterator end)
    {
        return std::string_view(&*begin, end - begin);
    }


    ParserError BaseParser::Error_Message(std::string_view message, uint32 line, uint32 column)
    {
        return ParserError(fmt::format("({:d}, {:d}) {:s}", line, column, message));
    }

    ParserError BaseParser::Error_Message(std::string_view message, Token token)
    {
        return Error_Message(message, token.Line, token.Column);
    }

    ParserError BaseParser::Error_UnexpectedToken(Token receivedToken)
    {
        std::string_view receivedStr = EnumUtils::ToString(receivedToken.Type);
        return Error_Message(fmt::format("Unexpected TokenType '{:s}'", receivedStr), receivedToken);
    }

    ParserError BaseParser::Error_UnexpectedToken(Token receivedToken, TokenType expectedTokenType)
    {
        std::string_view expectedStr = EnumUtils::ToString(expectedTokenType);
        std::string_view receivedStr = EnumUtils::ToString(receivedToken.Type);
        return Error_Message(fmt::format("Expected TokenType '{:s}', got '{:s}'", expectedStr, receivedStr), receivedToken);
    }

    ParserError BaseParser::Error_UnexpectedSymbol(Token receivedSymbol, char expectedSymbol)
    {
        return Error_Message(fmt::format("Expected Symbol '{:c}', got '{:c}'", expectedSymbol, receivedSymbol.AsChar()), receivedSymbol);
    }

    ParserError BaseParser::Error_UnexpectedIdentifier(Token receivedIdentifier)
    {
        return Error_Message(fmt::format("Unexpected Identifier '{:s}'", receivedIdentifier.Value), receivedIdentifier);
    }

    ParserError BaseParser::Error_UnexpectedIdentifier(Token receivedIdentifier, std::string_view expectedIdentifier)
    {
        return Error_Message(fmt::format("Expected Identifier '{:s}', got '{:s}'", expectedIdentifier, receivedIdentifier.Value), receivedIdentifier);
    }

    ParserError BaseParser::Error_UnexpectedEndOfFile(uint32 line, uint32 column)
    {
        return Error_Message("Unexpected EndOfFile", line, column);
    }

    ParserError BaseParser::Error_UnexpectedEndOfFile(Token endOfFileToken)
    {
        return Error_Message("Unexpected EndOfFile", endOfFileToken);
    }

} // namespace Copium
