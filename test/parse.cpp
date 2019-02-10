#include "catch.hpp"
#include <utf8pp.hpp>

utf8pp::utf8_ssize test_oracle(utf8pp::utf8_byte const* src) {
    if (src[0] == '\0') return 0;
    if ((src[0] & 0b10000000) == 0) return 1;
    if ((src[0] & 0b11100000) == 0b11000000) {
        if ((src[1] & 0b11000000) == 0b10000000) return 2;
        return utf8pp::error::invalid_utf8;
    }
    if ((src[0] & 0b11110000) == 0b11100000) {
        if ((src[1] & 0b11000000) == 0b10000000
         && (src[2] & 0b11000000) == 0b10000000) return 3;
        return utf8pp::error::invalid_utf8;
    }
    if ((src[0] & 0b11111000) == 0b11110000) {
        if ((src[1] & 0b11000000) == 0b10000000
         && (src[2] & 0b11000000) == 0b10000000
         && (src[3] & 0b11000000) == 0b10000000) return 4;
        return utf8pp::error::invalid_utf8;
    }
    return utf8pp::error::invalid_utf8;
}

TEST_CASE("determine the byte-length of an utf8-encoded CP", "[parse_next]") {
    #if 0
    for (std::uint32_t n = 0; n < 0xffffffff; ++n) {
        auto oracle = test_oracle((utf8pp::utf8_byte*)&n);
        auto fn = utf8pp::parse_next((utf8pp::utf8_byte*)&n);
        REQUIRE(oracle == fn);
    }
    #endif
}

TEST_CASE("read CPs from utf8-encoded strings", "[read_next]") {
    char const* test_str =
        u8"\0a\u007f\u0080\u0195\u07ff\u0800\u0939\uffff\U00010000";
    utf8pp::utf8_byte const* src = (utf8pp::utf8_byte const*)test_str;
    utf8pp::utf8_cp cp = 0;

    SECTION("null-terminator") {
        auto res = utf8pp::read_next(src, cp);

        REQUIRE(res == 0);
        REQUIRE(cp == '\0');
    }
    ++src;
    SECTION("single-byte character") {
        auto res = utf8pp::read_next(src, cp);

        REQUIRE(res == 1);
        REQUIRE(cp == 'a');
    }
    ++src;
    SECTION("single-byte character upper-bound") {
        auto res = utf8pp::read_next(src, cp);

        REQUIRE(res == 1);
        REQUIRE(cp == 0x7f);
    }
    ++src;
    SECTION("2-byte character lower-bound") {
        auto res = utf8pp::read_next(src, cp);

        REQUIRE(res == 2);
        REQUIRE(cp == 0x80);
    }
    src += 2;
    SECTION("2-byte character") {
        auto res = utf8pp::read_next(src, cp);

        REQUIRE(res == 2);
        REQUIRE(cp == 0x195);
    }
    src += 2;
    SECTION("2-byte character upper-bound") {
        auto res = utf8pp::read_next(src, cp);

        REQUIRE(res == 2);
        REQUIRE(cp == 0x7ff);
    }
    src += 2;
    SECTION("3-byte character lower-bound") {
        auto res = utf8pp::read_next(src, cp);

        REQUIRE(res == 3);
        REQUIRE(cp == 0x800);
    }
    src += 3;
    SECTION("3-byte character") {
        auto res = utf8pp::read_next(src, cp);

        REQUIRE(res == 3);
        REQUIRE(cp == 0x939);
    }
    src += 3;
    SECTION("3-byte character upper-bound") {
        auto res = utf8pp::read_next(src, cp);

        REQUIRE(res == 3);
        REQUIRE(cp == 0xffff);
    }
    src += 3;
    SECTION("4-byte character lower-bound") {
        auto res = utf8pp::read_next(src, cp);

        REQUIRE(res == 4);
        REQUIRE(cp == 0x10000);
    }
}
