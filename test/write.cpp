#include "catch.hpp"
#include <cstring>
#include <utf8pp.hpp>

TEST_CASE("write CPs UTF8 encoded", "[encode_cp]") {
    // U+70, U+190, U+1300, U+10080
    char const* oracle_string = u8"pƐጀ\U00010080";
    utf8pp::utf8_byte const* oracle = (utf8pp::utf8_byte const*)oracle_string;

    utf8pp::utf8_byte test[17];
    utf8pp::utf8_byte* test_ptr = test;

    {
        auto r = utf8pp::encode_cp(test_ptr, 0xffffffff);
        REQUIRE(r == utf8pp::error::invalid_codepoint);
    }

    {
        auto r = utf8pp::encode_cp(test_ptr, 0x70);
        REQUIRE(r == 1);
        test_ptr += r;
    }
    {
        auto r = utf8pp::encode_cp(test_ptr, 0x190);
        REQUIRE(r == 2);
        test_ptr += r;
    }
    {
        auto r = utf8pp::encode_cp(test_ptr, 0x1300);
        REQUIRE(r == 3);
        test_ptr += r;
    }
    {
        auto r = utf8pp::encode_cp(test_ptr, 0x10080);
        REQUIRE(r == 4);
        test_ptr += r;
    }
    {
        auto r = utf8pp::encode_cp(test_ptr, '\0');
        REQUIRE(r == 1);
    }

    REQUIRE(std::strcmp((char const*)test, (char const*)oracle) == 0);

    // Reading it back should also be correct
    test_ptr = test;
    utf8pp::utf8_cp cp = 0;
    {
        auto r = utf8pp::read_next(test_ptr, cp);
        REQUIRE(r == 1);
        REQUIRE(cp == 0x70);
        test_ptr += r;
    }
    {
        auto r = utf8pp::read_next(test_ptr, cp);
        REQUIRE(r == 2);
        REQUIRE(cp == 0x190);
        test_ptr += r;
    }
    {
        auto r = utf8pp::read_next(test_ptr, cp);
        REQUIRE(r == 3);
        REQUIRE(cp == 0x1300);
        test_ptr += r;
    }
    {
        auto r = utf8pp::read_next(test_ptr, cp);
        REQUIRE(r == 4);
        REQUIRE(cp == 0x10080);
    }
}
