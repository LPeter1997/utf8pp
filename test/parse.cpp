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
    for (std::uint32_t n = 0; n < 0xffffffff; ++n) {
        auto oracle = test_oracle((utf8pp::utf8_byte*)&n);
        auto fn = utf8pp::parse_next((utf8pp::utf8_byte*)&n);
        REQUIRE(oracle == fn);
    }
}
