//
// Created by alexl on 09.09.2023.
//

#include "String.h"
#include "Core/CodeSafety/Expects.h"
#define UTF_CPP_CPLUSPLUS 201703L
#include <unicode/ucnv.h>
#include <unicode/unistr.h>
#include <utfcpp/include/utf8.h>

namespace BeeEngine
{

    bool IsValidString(const UTF8String& string)
    {
        return utf8::is_valid(string.begin(), string.end());
    }

    UTF16String ConvertUTF8ToUTF16(const UTF8String& string)
    {
        BeeExpects(IsValidString(string));
        return utf8::utf8to16(static_cast<std::string_view>(string));
    }

    UTF8String ConvertUTF16ToUTF8(const UTF16String& string)
    {
        UTF8String result;
        utf8::utf16to8(string.begin(), string.end(), std::back_inserter(result));
        BeeEnsures(IsValidString(result));
        return result;
    }

    const unsigned char kFirstBitMask = 128; // 1000000
    const unsigned char kSecondBitMask = 64; // 0100000
    const unsigned char kThirdBitMask = 32;  // 0010000
    const unsigned char kFourthBitMask = 16; // 0001000
    const unsigned char kFifthBitMask = 8;   // 0000100

    UTF8StringView::iterator::iterator(String::const_iterator it) : m_StringIterator(it) {}

    UTF8StringView::iterator::iterator(const UTF8StringView::iterator& source)
        : m_StringIterator(source.m_StringIterator)
    {
    }

    UTF8StringView::iterator& UTF8StringView::iterator::operator=(const UTF8StringView::iterator& rhs)
    {
        m_StringIterator = rhs.m_StringIterator;
        m_IsDirty = rhs.m_IsDirty;
        m_Codepoint = rhs.m_Codepoint;
        return *this;
    }

    UTF8StringView::iterator::~iterator() {}

    UTF8StringView::iterator& UTF8StringView::iterator::operator++()
    {
        char firstByte = *m_StringIterator;

        std::string::difference_type offset = 1;

        if (firstByte &
            kFirstBitMask) // This means the first byte has a value greater than 127, and so is beyond the ASCII range.
        {
            if (firstByte & kThirdBitMask) // This means that the first byte has a value greater than 224, and so it
                                           // must be at least a three-octet code point.
            {
                if (firstByte & kFourthBitMask) // This means that the first byte has a value greater than 240, and so
                                                // it must be a four-octet code point.
                    offset = 4;
                else
                    offset = 3;
            }
            else
            {
                offset = 2;
            }
        }

        m_StringIterator += offset;
        m_IsDirty = true;
        return *this;
    }

    UTF8StringView::iterator UTF8StringView::iterator::operator++(int)
    {
        UTF8StringView::iterator temp = *this;
        ++(*this);
        return temp;
    }

    UTF8StringView::iterator& UTF8StringView::iterator::operator--()
    {
        --m_StringIterator;

        if (*m_StringIterator & kFirstBitMask) // This means that the previous byte is not an ASCII character.
        {
            --m_StringIterator;
            if ((*m_StringIterator & kSecondBitMask) == 0)
            {
                --m_StringIterator;
                if ((*m_StringIterator & kSecondBitMask) == 0)
                {
                    --m_StringIterator;
                }
            }
        }
        m_IsDirty = true;
        return *this;
    }

    UTF8StringView::iterator UTF8StringView::iterator::operator--(int)
    {
        UTF8StringView::iterator temp = *this;
        --(*this);
        return temp;
    }

    bool UTF8StringView::iterator::operator==(const UTF8StringView::iterator& rhs) const
    {
        return m_StringIterator == rhs.m_StringIterator;
    }

    bool UTF8StringView::iterator::operator!=(const UTF8StringView::iterator& rhs) const
    {
        return m_StringIterator != rhs.m_StringIterator;
    }

    char32_t UTF8StringView::iterator::operator*() const
    {
        if (m_IsDirty)
            UpdateCodepoint();
        return m_Codepoint;
    }
    void UTF8StringView::iterator::UpdateCodepoint() const
    {
        m_IsDirty = false;
        char32_t codePoint = 0;

        char firstByte = *m_StringIterator;

        if (firstByte &
            kFirstBitMask) // This means the first byte has a value greater than 127, and so is beyond the ASCII range.
        {
            if (firstByte & kThirdBitMask) // This means that the first byte has a value greater than 191, and so it
                                           // must be at least a three-octet code point.
            {
                if (firstByte & kFourthBitMask) // This means that the first byte has a value greater than 224, and so
                                                // it must be a four-octet code point.
                {
                    codePoint = (firstByte & 0x07) << 18;
                    char secondByte = *(m_StringIterator + 1);
                    codePoint += (secondByte & 0x3f) << 12;
                    char thirdByte = *(m_StringIterator + 2);
                    codePoint += (thirdByte & 0x3f) << 6;
                    ;
                    char fourthByte = *(m_StringIterator + 3);
                    codePoint += (fourthByte & 0x3f);
                }
                else
                {
                    codePoint = (firstByte & 0x0f) << 12;
                    char secondByte = *(m_StringIterator + 1);
                    codePoint += (secondByte & 0x3f) << 6;
                    char thirdByte = *(m_StringIterator + 2);
                    codePoint += (thirdByte & 0x3f);
                }
            }
            else
            {
                codePoint = (firstByte & 0x1f) << 6;
                char secondByte = *(m_StringIterator + 1);
                codePoint += (secondByte & 0x3f);
            }
        }
        else
        {
            codePoint = firstByte;
        }
        m_Codepoint = codePoint;
    }
    UTF8StringView::UTF8StringView(const UTF8String& string) : m_String(&string)
    {
        BeeExpects(IsValidString(string));
    }

    UTF8String ToUppercase(std::string_view string)
    {
        icu::UnicodeString unicodeString = icu::UnicodeString::fromUTF8(string);
        unicodeString.toUpper();
        UTF8String result;
        unicodeString.toUTF8String(result);
        return result;
    }
    UTF8String ToLowercase(std::string_view string)
    {
        icu::UnicodeString unicodeString = icu::UnicodeString::fromUTF8(string);
        unicodeString.toLower();
        UTF8String result;
        unicodeString.toUTF8String(result);
        return result;
    }
} // namespace BeeEngine
