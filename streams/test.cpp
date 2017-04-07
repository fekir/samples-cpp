#include <catch.hpp>

#include "streams.hpp"

TEST_CASE("string_view", "[string_view]"){
    const char data[] = "123 c 456";
    const auto len = 7;
    streambufview strv(&data[1], len); // no copy!, and '\0'-termination not required
    std::istream isv(&strv);
    std::stringbuf strb(std::string(&data[1], len));
    std::istream iss(&strb); // should be an istringstream
    SECTION("read content"){
        int a{};
        int b{};
        char c{};
        REQUIRE(isv >> a >> c >> b);
        int a2{};
        int b2{};
        char c2{};
        REQUIRE(iss >> a2 >> c2 >> b2);
        REQUIRE(a== a2);
        REQUIRE(c==c2);
        REQUIRE(b== b2);
    }

    SECTION("fail to read numbers"){
        int a{};
        int b{};
        int c{};
        REQUIRE_FALSE(iss >> a >> b >> c);
        REQUIRE_FALSE(isv >> a >> b >> c);
    }

    SECTION("in_avail"){
        auto i = strv.in_avail();
        auto i2 = strb.in_avail();
        REQUIRE(i == len);
        REQUIRE(i == i2);
        REQUIRE(isv >> i);
        REQUIRE(iss >> i2);
        REQUIRE(i == i2);
        i = strv.in_avail();
        i2 = strb.in_avail();
        REQUIRE(i == i2);
        char c{};
        char c2{};
        REQUIRE(isv >> c);
        REQUIRE(iss >> c2);
        REQUIRE(c == c2);
        i = strv.in_avail();
        i2 = strb.in_avail();
        REQUIRE(i == i2);
        REQUIRE(isv >> i);
        REQUIRE(iss >> i2);
        REQUIRE(i == i2);
        i = strv.in_avail();
        i2 = strb.in_avail();
        REQUIRE(i == 0);
        REQUIRE(i == i2);
    }
    SECTION("snextc/sgetc/sgetn"){
        auto c1 = strv.sgetc();
        auto c2 = strb.sgetc();
        REQUIRE(c1 == '2');
        REQUIRE(c1 == c2);
        c1 = strv.snextc();
        c2 = strb.snextc();
        REQUIRE(c1 == '3');
        REQUIRE(c1 == c2);
        c1 = strv.sgetc();
        c2 = strb.sgetc();
        REQUIRE(c1 == '3');
        c1 = strv.snextc();
        c2 = strb.snextc();
        REQUIRE(c1 == ' ');
        REQUIRE(c1 == c2);
        char tmp[3]{};
        strv.sgetn(tmp, sizeof(tmp));
        char tmp2[3]{};
        strb.sgetn(tmp2, sizeof(tmp2));
        REQUIRE(tmp[0] == ' ');
        REQUIRE(tmp[0] == tmp2[0]);
        REQUIRE(tmp[1] == 'c');
        REQUIRE(tmp[1] == tmp2[1]);
        REQUIRE(tmp[2] == ' ');
        REQUIRE(tmp[2] == tmp2[2]);
    }
    SECTION("sungetc"){
        auto c1 = strv.sgetc();
        auto c2 = strb.sgetc();
        REQUIRE(c1 == '2');
        REQUIRE(c1 == c2);
        c1 = strv.sungetc();
        c2 = strb.sungetc();
        REQUIRE(c1 == EOF);
        REQUIRE(c1 == c2);
        REQUIRE(isv >> c1);
        REQUIRE(iss >> c2);
        REQUIRE(c1==23);
        REQUIRE(c1==c2);
        c1 = strv.sgetc();
        c2 = strb.sgetc();
        REQUIRE(c1==' ');
        REQUIRE(c1==c2);
        c1 = strv.sungetc();
        c2 = strb.sungetc();
        REQUIRE(c1=='3');
        REQUIRE(c1==c2);
    }
}

