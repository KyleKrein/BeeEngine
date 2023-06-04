/**
 * @file source_location
 * @author Ondřej Beňuš
 * @brief Compatibility header for source_location, since some compilers had support for a long long time, just no header file.
 * @date 2022-01-15
 *
 * @copyright Copyright (c) 2022
 *
 * @details
 * This header takes advantage of the fact that the compiler is able to produce source_location
 * behavior, but it is simply missing the header file. In my findings, here are versions of the
 * compiler that support source_location:
 * GCC: 11.0
 * Clang: 12.0.1
 * MSVC: 16.10 (_MSC_VER 1929)
 *
 * However, the needed builtin functions are available since:
 * GCC: 4.8.1 (constexpr __builtin_LINE since 7.1)
 * Clang: 9.0.0
 * MSVC: 16.6 (_MSC_VER 1926)
 *
 */

#ifndef __SOURCE_LOCATION__
#define __SOURCE_LOCATION__
#pragma once

#ifdef __GNUC__
#	pragma GCC system_header
#endif

#if defined __has_include_next
#	if __has_include_next(<source_location>)
#		include_next <source_location>
#	endif
#endif

#if not defined __cpp_lib_source_location && defined __has_include
#	if __has_include(<version>)
#		include <version>
#		if defined __cpp_lib_source_location && __cpp_lib_source_location >= 201907L
#			include_next <source_location>
#		endif
#	endif
#endif

#if !defined __cpp_lib_source_location || __cpp_lib_source_location < 201907L
#	ifdef __cpp_consteval
#		define CONSTEVAL consteval
#	else
#		define CONSTEVAL constexpr
#	endif

#	if defined __clang__ || defined _MSC_VER
#		define HAS_BUILTIN_COLUMN
#	endif

#	include <cstdint>

namespace std {
    struct source_location {
        constexpr source_location() noexcept                       = default;
        constexpr source_location(source_location const&) noexcept = default;
        constexpr source_location(uint_least32_t line, uint_least32_t column, const char* file, const char* function) noexcept
                : _line(line), _col(column), _file(file), _func(function) {
        }

        static CONSTEVAL source_location current(
                uint_least32_t _line = __builtin_LINE(),
#	ifdef HAS_BUILTIN_COLUMN
                uint_least32_t _col = __builtin_COLUMN(),
#	else
                uint_least32_t _col = 0,
#	endif
                const char* _file = __builtin_FILE(),
                const char* _func = __builtin_FUNCTION()) noexcept {
            return source_location{_line, _col, _file, _func};
        }

        constexpr uint_least32_t line() const noexcept {
            return _line;
        }
        constexpr uint_least32_t column() const noexcept {
            return _col;
        }
        constexpr const char* file_name() const noexcept {
            return _file;
        }
        [[deprecated]] constexpr const char* file() const noexcept {
            return _file;
        }
        constexpr const char* function_name() const noexcept {
            return _func;
        }
        [[deprecated]] constexpr const char* function() const noexcept {
            return _func;
        }

    private:
        uint_least32_t _line = 0;
        uint_least32_t _col  = 0;
        const char*    _file = nullptr;
        const char*    _func = nullptr;
    };
} // namespace std

#endif

#endif // __SOURCE_LOCATION__