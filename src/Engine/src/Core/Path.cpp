//
// Created by alexl on 16.09.2023.
//

#include "Path.h"
#include "Core/CodeSafety/Expects.h"
#include "Core/Application.h"

#if defined(WINDOWS)
#include "Platform/Windows/WindowsString.h"
#endif


namespace BeeEngine
{
    static constexpr size_t bufferSize = 1024;

    Path::Path()
    {

    }

    Path::Path(const UTF8String &path)
    {
        m_Path = path;
        std::replace(m_Path.begin(), m_Path.end(), '\\', '/');
        BeeEnsures(IsValidString(m_Path));
    }

    Path::Path(const std::filesystem::path &path)
    {
#if defined(WINDOWS)
        auto utf8 = Internal::WStringToUTF8(path.wstring());
#else
        auto utf8 = path.string();
#endif
        std::replace(utf8.begin(), utf8.end(), '\\', '/');
        m_Path = std::move(utf8);
        BeeEnsures(IsValidString(m_Path));
    }

    Path::Path(const UTF16String &path)
    {
        auto utf8 = ConvertUTF16ToUTF8(path);
        std::replace(utf8.begin(), utf8.end(), '\\', '/');
        m_Path = std::move(utf8);
        BeeEnsures(IsValidString(m_Path));
    }

    Path::Path(const char *path)
    {
        size_t len = constexpr_strlen(path);
        BeeExpects(len < bufferSize);
        char buffer[bufferSize];
        memcpy(buffer, path, len);
        buffer[len] = '\0';

        std::replace(buffer, buffer + len, '\\', '/');
        m_Path = UTF8String(buffer);
        BeeEnsures(IsValidString(m_Path));
    }

    Path::Path(UTF8String &&path) noexcept
    {
        m_Path = std::move(path);
        std::replace(m_Path.begin(), m_Path.end(), '\\', '/');
        BeeEnsures(IsValidString(m_Path));
    }

    Path::Path(std::filesystem::path &&path) noexcept
    {
#if defined(WINDOWS)
        auto utf8 = Internal::WStringToUTF8(path.wstring());
#else
        auto utf8 = path.string();
#endif
        std::replace(utf8.begin(), utf8.end(), '\\', '/');
        m_Path = std::move(utf8);
        BeeEnsures(IsValidString(m_Path));
    }

    Path::Path(UTF16String &&path) noexcept
    {
        auto utf8 = ConvertUTF16ToUTF8(path);
        std::replace(utf8.begin(), utf8.end(), '\\', '/');
        m_Path = std::move(utf8);
        BeeEnsures(IsValidString(m_Path));
    }

    Path &Path::operator=(const UTF8String &path)
    {
        m_Path = path;
        std::replace(m_Path.begin(), m_Path.end(), '\\', '/');
        BeeEnsures(IsValidString(m_Path));
        return *this;
    }

    Path &Path::operator=(const std::filesystem::path &path)
    {
#if defined(WINDOWS)
        auto utf8 = Internal::WStringToUTF8(path.wstring());
#else
        auto utf8 = path.string();
#endif
        std::replace(utf8.begin(), utf8.end(), '\\', '/');
        m_Path = std::move(utf8);
        BeeEnsures(IsValidString(m_Path));
        return *this;
    }

    Path &Path::operator=(const UTF16String &path)
    {
        auto utf8 = ConvertUTF16ToUTF8(path);
        std::replace(utf8.begin(), utf8.end(), '\\', '/');
        m_Path = std::move(utf8);
        BeeEnsures(IsValidString(m_Path));
        return *this;
    }

    Path &Path::operator=(const char *path)
    {
        size_t len = constexpr_strlen(path);
        BeeExpects(len < bufferSize);
        char buffer[bufferSize];
        memcpy(buffer, path, len);
        buffer[len] = '\0';

        std::replace(buffer, buffer + len, '\\', '/');
        m_Path = UTF8String(buffer);
        BeeEnsures(IsValidString(m_Path));
        return *this;
    }

    Path &Path::operator=(UTF8String &&path) noexcept
    {
        m_Path = std::move(path);
        std::replace(m_Path.begin(), m_Path.end(), '\\', '/');
        BeeEnsures(IsValidString(m_Path));
        return *this;
    }

    Path &Path::operator=(std::filesystem::path &&path) noexcept
    {
#if defined(WINDOWS)
        auto utf8 = Internal::WStringToUTF8(path.wstring());
#else
        auto utf8 = path.string();
#endif
        std::replace(utf8.begin(), utf8.end(), '\\', '/');
        m_Path = std::move(utf8);
        BeeEnsures(IsValidString(m_Path));
        return *this;
    }

    Path &Path::operator=(UTF16String &&path) noexcept
    {
        auto utf8 = ConvertUTF16ToUTF8(path);
        std::replace(utf8.begin(), utf8.end(), '\\', '/');
        m_Path = std::move(utf8);
        BeeEnsures(IsValidString(m_Path));
        return *this;
    }

    bool Path::IsAbsolute() const noexcept
    {
#if defined(WINDOWS)
        if(m_Path.size() >= 3 && m_Path[1] == ':' && m_Path[2] == '/')
        {
            return true;
        }
        return m_Path.size() >= 2 && m_Path[0] == '/' && m_Path[1] == '/';
#elif defined(MACOS) || defined(LINUX)
        return !m_Path.empty() && m_Path[0] == '/';
#else
        #error "Path is unsupported on this platform"
#endif
    }

    bool Path::IsRelative() const noexcept
    {
        return !IsAbsolute();
    }

    Path Path::GetParent() const
    {
        auto slash = m_Path.find_last_of('/');
        if (slash == BeeEngine::UTF8String::npos)
        {
            return {};
        }
        return Path(UTF8String(m_Path.begin(), m_Path.begin() + slash));
    }

    Path Path::GetFileName() const
    {
        auto slash = m_Path.find_last_of('/');
        if (slash == BeeEngine::UTF8String::npos)
        {
            return m_Path;
        }
        return Path(UTF8String(m_Path.begin() + slash + 1, m_Path.end()));
    }

    Path Path::GetFileNameWithoutExtension() const
    {
        auto slash = m_Path.find_last_of('/');
        if (slash == BeeEngine::UTF8String::npos)
        {
            return {};
        }
        auto dot = m_Path.find_last_of('.');
        if (dot == BeeEngine::UTF8String::npos)
        {
            return {UTF8String(m_Path.begin() + slash + 1, m_Path.end())};
        }
        return {UTF8String(m_Path.begin() + slash + 1, m_Path.begin() + dot)};
    }

    Path Path::GetExtension() const
    {
        auto dot = m_Path.find_last_of('.');
        if (dot == BeeEngine::UTF8String::npos)
        {
            return {};
        }
        return {UTF8String(m_Path.begin() + dot, m_Path.end())};
    }

    Path Path::GetRelativePath(const Path &other) const
    {
        auto thisPath = ToStdPath();
        auto otherPath = other.ToStdPath();
        auto relativePath = std::filesystem::relative(thisPath, otherPath);
        return Path(relativePath);
    }

    Path Path::GetAbsolutePath() const
    {
        if(IsAbsolute())
        {
            return *this;
        }
        return {std::filesystem::current_path() / m_Path};
    }

    Path Path::GetAbsolutePath(const Path &relativeTo) const
    {
        if(IsAbsolute())
        {
            return *this;
        }
        auto relativeToPath = relativeTo;
        return relativeToPath.operator/=(m_Path);
    }

    Path &Path::operator/=(const Path &other)
    {
        if(other.IsAbsolute())
        {
            m_Path = other.m_Path;
        }
        else
        {
            if(!m_Path.empty() && m_Path.back() != '/')
                m_Path += '/';
            m_Path += other.m_Path;
        }
        return *this;
    }

    Path &Path::operator/=(Path &&other)
    {
        if(other.IsAbsolute())
        {
            m_Path = std::move(other.m_Path);
        }
        else
        {
            if(!m_Path.empty() && m_Path.back() != '/')
                m_Path += '/';
            m_Path += other.m_Path;
        }
        return *this;
    }

    void Path::ReplaceExtension(const Path &newExtension)
    {
        auto dot = m_Path.find_last_of('.');
        if (dot == BeeEngine::UTF8String::npos)
        {
            m_Path += newExtension.m_Path;
        }
        else
        {
            m_Path = UTF8String(m_Path.begin(), m_Path.begin() + dot) + newExtension.m_Path;
        }
    }

    void Path::ReplaceFileName(const Path &newFileName)
    {
        auto slash = m_Path.find_last_of('/');
        if (slash == BeeEngine::UTF8String::npos)
        {
            m_Path = newFileName.m_Path;
        }
        else
        {
            m_Path = UTF8String(m_Path.begin(), m_Path.begin() + slash + 1) + newFileName.m_Path;
        }
    }

    Path Path::RemoveExtension() const
    {
        auto dot = m_Path.find_last_of('.');
        if (dot == BeeEngine::UTF8String::npos)
        {
            return *this;
        }
        return UTF8String(m_Path.begin(), m_Path.begin() + dot);
    }

    Path Path::RemoveFileName() const
    {
        auto slash = m_Path.find_last_of('/');
        if (slash == BeeEngine::UTF8String::npos)
        {
            return *this;
        }
        return UTF8String(m_Path.begin(), m_Path.begin() + slash);
    }

    Path Path::operator/(const Path &other) const
    {
        return Path(*this) /= other;
    }

    std::filesystem::path Path::ToStdPath() const
    {
#if defined(WINDOWS)
        return {Internal::WStringFromUTF8(m_Path)};
#else
        return {m_Path};
#endif
    }
}
