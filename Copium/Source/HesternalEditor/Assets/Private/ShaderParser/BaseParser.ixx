module;

#include "Hesternal/Core/Defines.hpp"
#include "EditorCht/BaseParser.cht.hpp"

export module HesternalEditor.Assets.ShaderImporter.BaseParser;

import Hesternal.Core;

import HesternalEditor.Assets.ShaderImporter.ParserError;

import <optional>;
import <string_view>;


export namespace Hesternal
{

    // TODO(v.matushkin): Was defined in BaseParser as protected, but HeaderTool can't specialize type trait there,
    //  and I'm lazy to find a workaround.
    CHT_ENUM()
    enum class TokenType : uint8
    {
        None,
        Identifier,
        Symbol,
        DoubleSymbol, // "::"
        Number,
        String,
        EndOfFile,
    };


    class BaseParser
    {
        using ParserIterator = std::string_view::const_iterator;

    public:
        ~BaseParser() = default;

        BaseParser(const BaseParser&) = delete;
        BaseParser& operator=(const BaseParser&) = delete;

        BaseParser(BaseParser&& other) noexcept = default;
        BaseParser& operator=(BaseParser&& other) noexcept = default;

    protected:
        struct Token final
        {
            std::string_view Value;
            uint32           Line;
            uint32           Column;
            TokenType        Type;

            [[nodiscard]] bool IsValue(char symbol)                      const { return Value[0] == symbol; }
            [[nodiscard]] bool IsValue(std::string_view str)             const { return str == Value; }
            [[nodiscard]] bool IsSymbol()                                const { return Type == TokenType::Symbol; }
            [[nodiscard]] bool IsSymbol(char symbol)                     const { return IsSymbol() && IsValue(symbol); }
            [[nodiscard]] bool IsDoubleSymbol()                          const { return Type == TokenType::DoubleSymbol; }
            [[nodiscard]] bool IsDoubleSymbol(std::string_view symbol)   const { return IsDoubleSymbol() && IsValue(symbol); }
            [[nodiscard]] bool IsIdentifier()                            const { return Type == TokenType::Identifier; }
            [[nodiscard]] bool IsIdentifier(std::string_view identifier) const { return IsIdentifier() && IsValue(identifier); }
            [[nodiscard]] bool IsString()                                const { return Type == TokenType::String; }
            [[nodiscard]] bool IsEndOfFile()                             const { return Type == TokenType::EndOfFile; }

            [[nodiscard]] char AsChar() { return Value[0]; }

            [[nodiscard]] static Token None() { return Token(std::string_view(), 0, 0, TokenType::None); }
        };


        BaseParser(std::string_view input);

        [[nodiscard]] const Token& GetCurrentToken() const { return m_currentToken; }
        [[nodiscard]] const Token& GetPreviousToken() const { return m_previousToken; }

        [[nodiscard]] bool IsEndOfStream() const { return m_endOfStream; }

        [[nodiscard]] Token NextToken();
        [[nodiscard]] Token PeekToken();
        void AdvanceToken();

        [[maybe_unused]] Token ExpectToken(TokenType tokenType);
        [[maybe_unused]] Token ExpectSymbol()     { return ExpectToken(TokenType::Symbol); }
        [[maybe_unused]] Token ExpectSymbol(char symbol);
        [[maybe_unused]] Token ExpectIdentifier() { return ExpectToken(TokenType::Identifier); }
        [[maybe_unused]] Token ExpectIdentifier(std::string_view identifier);
        [[maybe_unused]] Token ExpectString()     { return ExpectToken(TokenType::String); }

        [[nodiscard]] static ParserError Error_Message(std::string_view message, uint32 line, uint32 column);
        [[nodiscard]] static ParserError Error_Message(std::string_view message, Token token);
        [[nodiscard]] static ParserError Error_UnexpectedToken(Token receivedToken);
        [[nodiscard]] static ParserError Error_UnexpectedToken(Token receivedToken, TokenType expectedTokenType);
        [[nodiscard]] static ParserError Error_UnexpectedSymbol(Token receivedSymbol, char expectedSymbol);
        [[nodiscard]] static ParserError Error_UnexpectedIdentifier(Token receivedIdentifier);
        [[nodiscard]] static ParserError Error_UnexpectedIdentifier(Token receivedIdentifier, std::string_view expectedIdentifier);
        [[nodiscard]] static ParserError Error_UnexpectedEndOfFile(uint32 line, uint32 column);
        [[nodiscard]] static ParserError Error_UnexpectedEndOfFile(Token endOfFileToken);

    private:
        [[nodiscard]] Token _NextTokenInternal();

        void _SkipWhitespaceAndComments();

        [[nodiscard]] bool _IsNumber();

        [[nodiscard]] Token _ParseIdentifier();
        [[nodiscard]] Token _ParseNumber();
        [[nodiscard]] Token _ParseString();

        [[maybe_unused]] bool _MoveNext();

        [[nodiscard]] static std::string_view _MakeStringView(ParserIterator begin, ParserIterator end);

    private:
        std::optional<Token> m_peekToken;
        Token                m_currentToken;
        Token                m_previousToken;

        ParserIterator       m_inputCurrent;
        const ParserIterator m_inputEnd;

        uint32               m_line;
        uint32               m_column;

        char                 m_currentSymbol;
        bool                 m_endOfStream;
    };

} // export namespace Hesternal
