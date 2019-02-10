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

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <type_traits>

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
 * Assertion.
 */
#define utf8pp_assert(x, msg) assert((x) && msg)
#define utf8pp_panic(msg) utf8pp_assert(false, msg)

/**
 * Our primitive types.
 */
using utf8_byte = std::uint8_t;
using utf8_ssize = std::int32_t;
using utf8_usize = std::size_t;
using utf8_cp = std::uint32_t;

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

/**
 * Error in codepoint value.
 */
inline constexpr utf8_ssize invalid_codepoint = -2;

} /* namespace error */

namespace detail {

/**
 * Used to check the range of the second, third and fourth bytes.
 */
inline constexpr bool next_byte_bad(utf8_byte b) noexcept {
    return b < 0x80 || b > 0xbf;
}

/**
 * Calculates the codepoint for a given length.
 */
inline /* constexpr */ utf8_cp
calc_codepoint(utf8_byte const* src, utf8_usize len) noexcept {
    utf8pp_assert(1 <= len && len <= 4,
        "The length of a unicode codepoint ban only be 1 to 4 bytes!");

    switch (len) {
    case 1:
        return   utf8_cp(src[0]);

    case 2:
        return ((utf8_cp(src[0]) & 0b00011111) << 6)
             |  (utf8_cp(src[1]) & 0b00111111);

    case 3:
        return ((utf8_cp(src[0]) & 0b00001111) << 12)
             | ((utf8_cp(src[1]) & 0b00111111) << 6)
             |  (utf8_cp(src[2]) & 0b00111111);

    case 4:
        return ((utf8_cp(src[0]) & 0b00000111) << 18)
             | ((utf8_cp(src[1]) & 0b00111111) << 12)
             | ((utf8_cp(src[2]) & 0b00111111) << 6)
             |  (utf8_cp(src[3]) & 0b00111111);
    }

    utf8pp_panic("Unreachable!");
    return utf8_cp(0);
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
inline /* constexpr */ utf8_ssize parse_next(utf8_byte const* src) noexcept {
    // End-of-string
    if (src[0] == '\0') return 0;
    // The MSB is 0, so it's a single-byte character
    if (src[0] < 0b10000000) return 1;
    // Must be at least 0b11000000 and at most 0b11110111
    if (src[0] < 0b11000000 || src[0] > 0b11110111) return error::invalid_utf8;
    // 2-byte encoding
    if (detail::next_byte_bad(src[1])) return error::invalid_utf8;
    if (src[0] < 0b11100000) return 2;
    // 3-byte encoding
    if (detail::next_byte_bad(src[2])) return error::invalid_utf8;
    if (src[0] < 0b11110000) return 3;
    // 4-byte encoding
    if (detail::next_byte_bad(src[3])) return error::invalid_utf8;
    return 4;
}

/**
 * Reads the next codepoint from the byte sequence.
 * @param src The pointer to the start of the source string. @see parse_next
 * @param dest The reference to write the results to. If there was an error (or
 * the string terminated) a null terminator is written.
 * @return @see parse_next
 */
inline /* constexpr */ utf8_ssize
read_next(utf8_byte const* src, utf8_cp& dest) noexcept {
    dest = '\0';

    auto result = parse_next(src);
    if (result <= 0) return result;

    dest = detail::calc_codepoint(src, result);
    return result;
}

/**
 * Same as @see parse_next, but parses backwards the first UTF8 character
 * *before* the parse position.
 * @param src_start The start of the entire string.
 * @param src @see parse_next
 * @return @see parse_next. If src_start == src, then the return value is zero.
 */
inline /* constexpr */ utf8_ssize
parse_prev(utf8_byte const* src_start, utf8_byte const* src) noexcept {
    if (src_start == src) return 0;
    if (src[-1] == '\0') return 0;
    // At least one byte long
    // Single-byte
    if (src[-1] < 0b10000000) return 1;
    // 2-byte encoding
    if (src_start == src - 1) return error::invalid_utf8;
    if (detail::next_byte_bad(src[-1])) return error::invalid_utf8;
    if (detail::next_byte_bad(src[-2])) {
        if (src[-2] < 0b11000000 || src[-2] > 0b11011111)
            return error::invalid_utf8;
        return 2;
    }
    // 3-byte encoding
    if (src_start == src - 2) return error::invalid_utf8;
    if (detail::next_byte_bad(src[-2])) return error::invalid_utf8;
    if (detail::next_byte_bad(src[-3])) {
        if (src[-3] < 0b11100000 || src[-3] > 0b11101111)
            return error::invalid_utf8;
        return 3;
    }
    // 4-byte encoding
    if (src_start == src - 3) return error::invalid_utf8;
    if (src[-4] < 0b11110000 || src[-4] > 0b11110111)
        return error::invalid_utf8;
    return 4;
}

/**
 * Same as @see read_next but with @see parse_prev.
 */
inline /* constexpr */ utf8_ssize
read_prev(utf8_byte const* src_start, utf8_byte const* src,
    utf8_cp& dest) noexcept {
    dest = '\0';

    auto result = parse_prev(src_start, src);
    if (result <= 0) return result;

    dest = detail::calc_codepoint(src - result, result);
    return result;
}

/**
 * Encode a codepoint into UTF8.
 * @param dst The destination buffer where the encoded character will be
 * written. The function does not perform any allocations, so the caller must
 * ensure that there is enough space in the buffer (at most 4 bytes).
 * @param cp The codepoint to be encoded.
 * @return The number of written bytes, or the negative error code
 * error::invalid_codepoint.
 */
inline /* constexpr */ utf8_ssize write(utf8_byte* dst, utf8_cp cp) noexcept {
    // Single-byte
    if (cp < 0x80) {
        dst[0] = utf8_byte(cp);
        return 1;
    }
    // 2-byte
    if (cp < 0x800) {
        dst[0] = utf8_byte((cp & 0b11111000000) >> 6) | 0b11000000;
        dst[1] = utf8_byte(cp & 0b111111) | 0b10000000;
        return 2;
    }
    // 3-byte
    if (cp < 0x10000) {
        dst[0] = utf8_byte((cp & 0b1111000000000000) >> 12) | 0b11100000;
        dst[1] = utf8_byte((cp & 0b111111000000) >> 6) | 0b10000000;
        dst[2] = utf8_byte(cp & 0b111111) | 0b10000000;
        return 3;
    }
    // 4-byte
    if (cp < 0x110000) {
        dst[0] = utf8_byte((cp & 0b111000000000000000000) >> 18) | 0b11110000;
        dst[1] = utf8_byte((cp & 0b111111000000000000) >> 12) | 0b10000000;
        dst[2] = utf8_byte((cp & 0b111111000000) >> 6) | 0b10000000;
        dst[3] = utf8_byte(cp & 0b111111) | 0b10000000;
        return 4;
    }
    return error::invalid_codepoint;
}

#undef utf8pp_assert
#undef utf8pp_panic

} /* namespace utf8pp */

#undef UTF8PP_WINDOWS
#undef UTF8PP_LINUX
#undef UTF8PP_OSX

#endif /* PETER_LENKEFI_UTF8PP_HPP */
