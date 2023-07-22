//
// Created by alexl on 21.07.2023.
//

#pragma once
#include <string>
#include <filesystem>
#include <vector>
#include "ScriptScanner.h"
namespace BeeEngine::Editor
{
    struct BeeVariable
    {
        std::string Type;
        std::string Identifier;
    };

    struct BeeClass
    {
        std::string Name;
        std::filesystem::path Path;
        std::vector<BeeVariable> Variables;
    };

    class ScriptParser
    {
    public:
        ScriptParser(std::vector<Token>& tokens, std::filesystem::path& path)
        : m_Tokens(tokens), m_Path(path)
        {}
        std::string GenerateHeaderFile();
        void Parse();

        bool CanGenerateHeaderFile() const noexcept
        {
            return !m_Classes.empty();
        }

        const std::vector<BeeClass>& GetClasses() const noexcept
        {
            return m_Classes;
        }

        static std::string GetFileNameAsClassName(const std::string& filename) noexcept;
    private:
        void ParseClass();
        void ParseStruct();
        BeeVariable ParseVariable();
        const Token& Expect(TokenType type);
        bool Match(TokenType type);

        inline Token GenerateErrorToken() const noexcept
        {
            return Token{ 0, 0, TokenType::ERROR_TYPE, "" };
        }

        size_t m_CurrentToken = 0;
        std::vector<Token>::iterator m_CurrentIterator;
        const std::filesystem::path& m_Path;

        std::vector<Token>& m_Tokens;
        std::vector<BeeClass> m_Classes;
    };
}
