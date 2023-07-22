//
// Created by alexl on 21.07.2023.
//

#pragma once
#include <string>
#include <filesystem>
#include <map>

namespace BeeEngine::Editor
{
    enum class TokenType
    {
        PROPERTY, FUNCTION_PROP, CLASS_PROP, STRUCT_PROP,
        STRUCT_KW, CLASS_KW, COLON,

        LEFT_PAREN, RIGHT_PAREN, HASHTAG, LEFT_BRACKET, RIGHT_BRACKET,
        SEMICOLON, EQUAL, PLUS_EQUAL, MINUS_EQUAL, TIMES_EQUAL, DIV_EQUAL,
        MODULO_EQUAL, CARET_EQUAL, AMPERSAND_EQUAL, BAR_EQUAL, RIGHT_SHIFT_EQUAL,
        LEFT_SHIFT_EQUAL, EQUAL_EQUAL, BANG_EQUAL, LESS_THAN_EQUAL, GREATER_THAN_EQUAL,

        CONST_KW, IDENTIFIER, AUTO_KW,

        STAR, REF, LEFT_ANGLE_BRACKET, RIGHT_ANGLE_BRACKET,

        STRING_LITERAL, NUMBER, TRUE_KW, FALSE_KW,

        END_OF_FILE,
        ERROR_TYPE
    };

    struct Token
    {
        size_t m_Line;
        size_t m_Column;
        TokenType m_Type;
        std::string m_Lexeme;
        // void* m_Literal;
    };

    class ScriptScanner
    {
    public:
        ScriptScanner(const std::filesystem::path& path);

        std::vector<Token> ScanTokens();
    private:
        Token ScanToken();
        Token PropertyIdentifier();
        Token Number();
        Token String();

        char Advance();
        char Peek();
        char PeekNext();
        char PeekNextNext();
        bool Match(char expected);

        inline bool IsDigit(char c) const noexcept
        {
            return c >= '0' && c <= '9';
        }
        inline bool IsAlphaNumeric(char c) const noexcept
        {
            return IsAlpha(c) || IsDigit(c);
        }

        inline bool IsAlpha(char c) const noexcept
        {
            return (c >= 'a' && c <= 'z') ||
                   (c >= 'A' && c <= 'Z') ||
                   c == '_';
        }

        inline bool IsAtEnd() const noexcept
        {
            return m_Cursor >= m_FileContents.size();
        }

        inline Token GenerateToken(TokenType type, std::string lexeme)
        {
            return { m_Line, m_Column, type, lexeme };
        }

        inline Token GenerateErrorToken()
        {
            return { -1uz, -1uz, TokenType::ERROR_TYPE, "" };
        }

        const std::map<std::string, TokenType> m_Keywords = {
                { "UPROPERTY",   TokenType::PROPERTY },
                { "UCLASS",      TokenType::CLASS_PROP },
                { "USTRUCT",     TokenType::STRUCT_PROP },
                { "UFUNCTION",   TokenType::FUNCTION_PROP },
                { "auto",        TokenType::AUTO_KW },
                { "const",       TokenType::CONST_KW },
                { "class",       TokenType::CLASS_KW },
                { "struct",      TokenType::STRUCT_KW }
        };

        std::string m_FileContents;
        std::filesystem::path m_Path;

        size_t m_Cursor = 0;
        size_t m_Line = 1;
        size_t m_Column = 0;
        size_t m_Start = 0;

    };
}
