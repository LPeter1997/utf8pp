#include <cstdio>
#include <utf8pp.hpp>

int main() {
    utf8pp::set_stdout_utf8();
    char const* str = u8"こんにちは世界";
    utf8pp::utf8_byte const* ustr = (utf8pp::utf8_byte const*)str;
    while (auto skp = utf8pp::parse_next(ustr)) {
        if (skp < 0) return 1;
        std::printf("%.*s\n", int(skp), (char const*)ustr);
        ustr += skp;
    }
    return 0;
}
