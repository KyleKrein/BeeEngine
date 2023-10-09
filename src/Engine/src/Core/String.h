//
// Created by alexl on 09.09.2023.
//

#pragma once
#include <string>
#include <filesystem>

namespace BeeEngine
{
    using String = std::string;
    using UTF8String = String;
    using UTF16String = std::u16string;

    bool IsValidString(const UTF8String& string);
    UTF16String ConvertUTF8ToUTF16(const UTF8String& string);
    UTF8String ConvertUTF16ToUTF8(const UTF16String& string);

    class UTF8StringView
    {
    public:
        UTF8StringView(const UTF8String& string);
        class iterator: public std::iterator<std::bidirectional_iterator_tag, char32_t, std::string::difference_type, const char32_t*, const char32_t&>
        {
        public:
            iterator(std::string::const_iterator it);
            iterator(const iterator& source);
            iterator& operator=(const iterator& rhs);
            ~iterator();

            iterator& operator++();
            iterator operator++(int);
            iterator& operator--();
            iterator operator--(int);
            bool operator==(const iterator& rhs) const;
            bool operator!=(const iterator& rhs) const;
            char32_t operator*() const;
        private:
            std::string::const_iterator m_StringIterator;
            mutable char32_t m_Codepoint = 0;
            mutable bool m_IsDirty = true;

            void UpdateCodepoint() const;
        };
        iterator begin() const
        {
            return iterator(m_String->cbegin());
        }
        iterator end() const
        {
            return iterator(m_String->cend());
        }
    private:
        const UTF8String* m_String;
    };


    constexpr std::size_t constexpr_strlen(const char* s)
    {
        return std::char_traits<char>::length(s);
        // or
        return std::string::traits_type::length(s);
    }

    constexpr std::size_t constexpr_wcslen(const wchar_t* s)
    {
        return std::char_traits<wchar_t>::length(s);
        // or
        return std::wstring::traits_type::length(s);
    }
}