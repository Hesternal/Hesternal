using System;

namespace Copium.HeaderTool.Parser
{
    public sealed class ParserException : Exception
    {
        public readonly string FilePath;
        public readonly int Line;
        public readonly int Column;

        public ParserException(string filePath, int line, int column, string message)
            : base(message)
        {
            FilePath = filePath;
            Line = line;
            Column = column;
        }
    }


    internal static class ParserError
    {
        public static ParserException Message(string file, int line, int column, string message)
        {
            return new ParserException(file, line, column, message);
        }

        public static ParserException Message(string file, Token token, string message)
        {
            return Message(file, token.Line, token.Column, message);
        }


        public static ParserException UnexpectedToken(string file, Token receivedToken, TokenType expectedTokenType)
        {
            return Message(file, receivedToken, $"Expected TokenType '{expectedTokenType}', got '{receivedToken.Type}'");
        }

        public static ParserException UnexpectedSymbol(string file, Token receivedSymbol, char expectedSymbol)
        {
            return Message(file, receivedSymbol, $"Expected Symbol '{expectedSymbol}', got '{receivedSymbol.Value}'");
        }

        public static ParserException UnexpectedIdentifier(string file, Token receivedIdentifier, string expectedIdentifier)
        {
            return Message(file, receivedIdentifier, $" Expected Identifier '{expectedIdentifier}', got '{receivedIdentifier.Value}'");
        }


        public static ParserException UnexpectedEndOfFile(string file, int line, int column)
        {
            return Message(file, line, column, "Unexpected EndOfFile");
        }

        public static ParserException UnexpectedEndOfFile(string file, Token endOfFile)
        {
            return Message(file, endOfFile, " Unexpected EndOfFile");
        }
    }
}
