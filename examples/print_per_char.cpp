#include <cstdio>
#include <utf8pp.hpp>

int main() {
    utf8pp::set_stdout_utf8();
    char const* str = u8"こんにちは世界";
    utf8pp::utf8_byte const* ustr_start = (utf8pp::utf8_byte const*)str;
    utf8pp::utf8_byte const* ustr = ustr_start;

    std::puts("Forward:");
    {
        while (auto skp = utf8pp::parse_next(ustr)) {
            if (skp < 0) return 1;
            std::printf(" %.*s\n", int(skp), (char const*)ustr);
            ustr += skp;
        }
    }

    std::puts("Backward:");
    {
        while (auto skp = utf8pp::parse_prev(ustr_start, ustr)) {
            if (skp < 0) return 1;
            ustr -= skp;
            std::printf(" %.*s\n", int(skp), (char const*)ustr);
        }
    }

    return 0;
}
