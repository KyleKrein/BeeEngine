//
// Created by alexl on 16.09.2023.
//

#pragma once

#include "Move.h"
#include "String.h"
#include <version>

namespace BeeEngine
{
    class Path
    {
    public:
        Path();
        Path(const UTF8String& path);
        Path(const std::filesystem::path& path);
        Path(const UTF16String& path);
        Path(const char* path);
        Path(UTF8String&& path) noexcept;
        Path(std::filesystem::path&& path) noexcept;
        Path(UTF16String&& path) noexcept;

        Path& operator=(const UTF8String& path);
        Path& operator=(const std::filesystem::path& path);
        Path& operator=(const UTF16String& path);
        Path& operator=(const char* path);
        Path& operator=(UTF8String&& path) noexcept;
        Path& operator=(std::filesystem::path&& path) noexcept;
        Path& operator=(UTF16String&& path) noexcept;

        bool IsAbsolute() const noexcept;
        bool IsRelative() const noexcept;

        bool IsEmpty() const noexcept { return m_Path.empty(); }
#if 0//__has_attribute(__cpp_explicit_this_parameter)
        template <typename Self>
        auto&& AsUTF8(this Self&& self) noexcept
        {
            return std::forward<Self>(self).m_Path;
        }
#else
        const UTF8String& AsUTF8() const& noexcept { return m_Path; }
        UTF8String& AsUTF8() & noexcept { return m_Path; }
        UTF8String&& AsUTF8() && noexcept { return BeeMove(m_Path); }
#endif
        const char* AsCString() const noexcept { return m_Path.c_str(); }
        // const char* AsCString() const&& noexcept = delete;
        UTF16String ToUTF16() const { return ConvertUTF8ToUTF16(m_Path); }

        String ToString() const { return m_Path; }

        std::filesystem::path ToStdPath() const;

        // operator UTF8String&() { return m_Path; }
        operator const UTF8String&() const { return m_Path; }

        void Clear() noexcept { m_Path.clear(); }

        Path GetParent() const;
        Path GetFileName() const;
        Path GetFileNameWithoutExtension() const;
        Path GetExtension() const;
        Path GetRelativePath(const Path& other) const;
        Path GetAbsolutePath() const;
        Path GetAbsolutePath(const Path& relativeTo) const;

        void ReplaceExtension(const Path& newExtension);
        void ReplaceFileName(const Path& newFileName);
        Path RemoveExtension() const;
        Path RemoveFileName() const;

        Path& operator/=(const Path& other);
        Path& operator/=(Path&& other);
        Path operator/(const Path& other) const;

        bool operator==(const Path& other) const noexcept { return m_Path == other.m_Path; }
        bool operator!=(const Path& other) const noexcept { return m_Path != other.m_Path; }

    private:
        UTF8String m_Path;

        void RefactorApplyAndCheck(UTF8String&& utf8);
    };
} // namespace BeeEngine
namespace std
{
    template <>
    struct hash<BeeEngine::Path>
    {
        size_t operator()(const BeeEngine::Path& path) const noexcept
        {
            return hash<BeeEngine::UTF8String>()(path.AsUTF8());
        }
    };
} // namespace std
