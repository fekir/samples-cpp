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

TEST_CASE("file", "[file][istream][ostream]"){
	// std::tmpfile seems to work perfectly on windows without admin privileges 
	// (tested on win 7 and win 10 x64, even from guest account and UAC to maximum level). 
	// The msdn documentation states that admin privileges may be required, but only windows vista 
	// is mentioned as operating system.
    FILE* f = std::tmpfile(); // opened like "wb+", deleted in all cases (even modern widnows)
    fileviewbuf_base<3> b(f);
    std::istream is(&b);
    const char buffer[] = "hello_world!";
    SECTION("read content"){
        ::fwrite(buffer , sizeof(char), std::strlen(buffer), f);
        ::rewind(f);
        std::string str;
        REQUIRE(is >> str);
        REQUIRE(str == buffer);
    }
    SECTION("multiple reads"){
        ::fwrite(buffer , sizeof(char), std::strlen(buffer), f);
        ::fwrite(" " , sizeof(char), 1, f); // whitespace used as delimiter
        ::fwrite(buffer , sizeof(char), std::strlen(buffer), f);
        ::rewind(f);
        std::string str;
        REQUIRE(is >> str);
        REQUIRE(str == buffer);
        REQUIRE(is >> str);
        REQUIRE(str == buffer);
    }

    SECTION("wrong read content"){
        ::fwrite(buffer , sizeof(char), std::strlen(buffer), f);
        ::rewind(f);
        std::string str;
        int a{};
        REQUIRE_FALSE(is >> a);
    }


    SECTION("write content"){
        fileviewbuf b(f);
        std::ostream ios(&b);
        ios << buffer << std::flush; // otherwise content is not yet written to buffer
        rewind(f);
        char buffer2[sizeof(buffer)]{};
        const auto size = fread(buffer2, sizeof(char), std::strlen(buffer), f);
        REQUIRE(size == std::strlen(buffer));
        REQUIRE(std::equal(buffer2, buffer2+sizeof(buffer2), buffer));
    }

    SECTION("write content2"){
        fileviewbuf b(f);
        std::ostream ios(&b);
        ios << 2 << std::flush; // otherwise content is not yet written to buffer
        rewind(f);
        char buffer2[1]{};
        const auto size = fread(buffer2, sizeof(char), std::strlen(buffer), f);
        REQUIRE(size == 1);
        REQUIRE(buffer2[0] == '2');
    }
}
