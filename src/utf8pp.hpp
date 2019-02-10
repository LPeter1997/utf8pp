/**
 * utf8pp.hpp
 *
 * @author Peter Lenkefi
 * @date 2019-02-10
 * @description This is the utf8pp library header. Contains all library
 * functionality.
 * @license MIT
 */

#ifndef PETER_LENKEFI_UTF8PP_HPP
#define PETER_LENKEFI_UTF8PP_HPP

#include <cstddef>
#include <cstdint>

/**
 * Shortcuts for OSes.
 */
#define UTF8PP_WINDOWS 0
#define UTF8PP_LINUX 0
#define UTF8PP_OSX 0

#if defined(_WIN32) || defined(_WIN64)
#   undef UTF8PP_WINDOWS
#   define UTF8PP_WINDOWS 1
#elif defined(__linux__)
#   undef UTF8PP_LINUX
#   define UTF8PP_LINUX 1
#elif defined(__APPLE__)
#   undef UTF8PP_OSX
#   define UTF8PP_OSX 1
#else
#   error "Unsupported platform!"
#endif

#if UTF8PP_WINDOWS
#   include <Windows.h>
#endif

namespace utf8pp {

/**
 * Our primitive types.
 */
using utf8_byte = std::uint8_t;
using utf8_ssize = std::int32_t;

/**
 * Here we define the error codes.
 * We make them negative numbers so it's easy check for errors with a less-than
 * zero comparison.
 */
namespace error {

/**
 * Error in UTF8 encoding.
 */
inline constexpr utf8_ssize invalid_utf8 = -1;

} /* namespace error */

namespace detail {

/**
 * Used to check the range of the second, third and fourth bytes.
 */
inline constexpr bool next_byte_bad(utf8_byte b) noexcept {
    return b < 0x80 || b > 0xbf;
}

} /* namespace detail */

/**
 * Sets the stdout to UTF8 encoding on platforms that's not the default.
 */
inline void set_stdout_utf8() {
#if UTF8PP_WINDOWS
    SetConsoleOutputCP(CP_UTF8);
#endif
}

/**
 * Calculates how many bytes the next UTF8 codepoint occupies.
 * @param src The pointer to the start of the source string. Must be
 * null-terminated to avoid buffer-overruns.
 * @return Zero, if there is a null-terminator, the number of bytes of the next
 * UTF8 codepoint if valid (1 <= no. bytes <= 4), or the negative error code
 * error::invalid_utf8.
 */
inline constexpr utf8_ssize parse_next(utf8_byte const* src) noexcept {
    // End-of-string
    if (src[0] == '\0') return 0;
    // The MSB is 0, so it's a single-byte character
    if (src[0] < 0x80) return 1;
    // Must be at least 0b11000000 and at most 0b11110111
    if (src[0] < 0xc0 || src[0] > 0xf7) return error::invalid_utf8;
    // 2-byte encoding
    if (detail::next_byte_bad(src[1])) return error::invalid_utf8;
    if (src[0] < 0xe0) {
        return 2;
    }
    // 3-byte encoding
    if (detail::next_byte_bad(src[2])) return error::invalid_utf8;
    if (src[0] < 0xf0) {
        return 3;
    }
    // 4-byte encoding
    if (detail::next_byte_bad(src[3])) return error::invalid_utf8;
    return 4;
}

} /* namespace utf8pp */

#undef UTF8PP_WINDOWS
#undef UTF8PP_LINUX
#undef UTF8PP_OSX

#endif /* PETER_LENKEFI_UTF8PP_HPP */
