#include <catch.hpp>

#include "streams.hpp"

TEST_CASE("string_view", "[string_view]"){
    const char data[] = "123 c 456";
    const auto len = 7;
    streambufview strv(&data[1], len); // no copy!, and '\0'-termination not required
    std::istream iss(&strv);
    SECTION("read content"){
        int a{};
        int b{};
        char c{};
        REQUIRE(iss >> a >> c >> b);
        REQUIRE(a== 23);
        REQUIRE(c=='c');
        REQUIRE(b== 45);
    }

    SECTION("fail to read numbers"){
        int a{};
        int b{};
        int c{};
        REQUIRE_FALSE(iss >> a >> b >> c);
    }
    SECTION("in_avail"){
        auto i = strv.in_avail();
        REQUIRE(i == len);
        REQUIRE(iss >> i);
        REQUIRE(i == 23);
        i = strv.in_avail();
        REQUIRE(i == 5);
        char c{};
        REQUIRE(iss >> c);
        REQUIRE(c == 'c');
        i = strv.in_avail();
        REQUIRE(i == 3);
        REQUIRE(iss >> i);
        REQUIRE(i == 45);
        i = strv.in_avail();
        REQUIRE(i == 0);
    }
    SECTION("snextc/sgetc/sgetn"){
        auto c = strv.sgetc();
        REQUIRE(c == '2');
        c = strv.snextc();
        REQUIRE(c == '3');
        c = strv.sgetc();
        REQUIRE(c == '3');
        c = strv.snextc();
        REQUIRE(c == ' ');
        char tmp[3]{};
        strv.sgetn(tmp, sizeof(tmp));
        REQUIRE(tmp[0] == ' ');
        REQUIRE(tmp[1] == 'c');
        REQUIRE(tmp[2] == ' ');
    }
    SECTION("sungetc"){
        auto c = strv.sgetc();
        REQUIRE(c == '2');
        c = strv.sungetc();
        REQUIRE(c == EOF);
        REQUIRE(iss >> c);
        REQUIRE(c==23);
        c = strv.sgetc();
        REQUIRE(c==' ');
        c = strv.sungetc();
        REQUIRE(c=='3');
    }
}

