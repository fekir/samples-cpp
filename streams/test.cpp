#include <catch.hpp>

#ifdef _WIN32


#include <atomic>
#include <condition_variable>
#include <thread>
#include <chrono>

#include <windows.h>
#include <winevt.h>
#endif

#include "streams.hpp"

TEST_CASE("string_view", "[string_view][istream]"){
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
	// is mentioned as operating system. The file is also create in the %TMP% directory and not on the system drive
    FILE* f = std::tmpfile(); // opened like "wb+", deleted in all cases (even modern windows)
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


#ifdef _WIN32
struct EvtClose_functor {
	typedef EVT_HANDLE pointer;
	void operator()(const EVT_HANDLE h) const noexcept {
		const auto res = ::EvtClose(h);
		assert(res == TRUE); (void)res;
	}
};
using unique_EVTHANDLE = std::unique_ptr<EVT_HANDLE, EvtClose_functor>;

// utf8-encoded
std::string ws2s(const std::wstring& s) {
	if (s.empty()) {
		return "";
	}
	if (s.length() > (static_cast<size_t>((std::numeric_limits<int>::max)()))) {
		throw std::runtime_error("string is too long");
	}
	constexpr auto flag = WC_ERR_INVALID_CHARS; // alternative to WC_ERR_INVALID_CHARS is 0
												// https://msdn.microsoft.com/en-us/library/windows/desktop/dd374130%28v=vs.85%29.aspx
	int len = WideCharToMultiByte(CP_UTF8, flag, s.c_str(), static_cast<int>(s.length()), nullptr, 0, nullptr, nullptr);
	if (len <= 0) {
		throw std::runtime_error("error when converting to string");
	}
	std::string buf(static_cast<size_t>(len), '\0');
	len = WideCharToMultiByte(CP_UTF8, flag, s.c_str(), static_cast<int>(s.length()), &buf[0], len, nullptr, nullptr);
	if (len <= 0) {
		throw std::runtime_error("error when converting to string");
	}
	buf.resize(static_cast<size_t>(len));
	return buf;
}

size_t getRequiredSize_XML(const EVT_HANDLE hEvent) {
	DWORD dwBufferUsed = 0;
	if (EvtRender(nullptr, hEvent, EvtRenderEventXml, 0, nullptr, &dwBufferUsed, nullptr)) {
		return 0;
	}
	DWORD status = GetLastError();
	if (ERROR_INSUFFICIENT_BUFFER != status) {
		throw std::runtime_error("unexpected error while querying size for message:" + std::to_string(status));
	}
	return dwBufferUsed / 2;
}

std::string get_rendered_content(const EVT_HANDLE hEvent)
{
	DWORD dwBufferUsed = 0;
	std::wstring renderedContent(getRequiredSize_XML(hEvent), L'\0');

	if (!EvtRender(nullptr, hEvent, EvtRenderEventXml, static_cast<DWORD>(renderedContent.size()) * sizeof(wchar_t), &renderedContent.at(0), &dwBufferUsed, nullptr)) {
		const DWORD status = GetLastError(); assert(ERROR_INSUFFICIENT_BUFFER != status);
		throw std::runtime_error("unexpected error while querying message:" + std::to_string(status));
	}
	std::string toreturn = ws2s(renderedContent);
	toreturn.erase(toreturn.find_last_not_of('\0') + 1);
	return toreturn;
}

struct context {
	std::condition_variable cv;
	std::mutex cv_m;
	std::vector<std::string> messages;
	std::size_t total_events = 1;
};

DWORD WINAPI callback(const EVT_SUBSCRIBE_NOTIFY_ACTION action, PVOID pContext, const EVT_HANDLE hEvent) try {
	assert(pContext != nullptr);
	auto& ctx = *reinterpret_cast<context*>(pContext);
	if (action != EvtSubscribeActionDeliver) {
		return 0;
	}

	const auto var = get_rendered_content(hEvent);
	{
		std::lock_guard<std::mutex> lk(ctx.cv_m);
		ctx.messages.push_back(var);
		if (ctx.messages.size() == ctx.total_events) {
			ctx.cv.notify_all();
		}
	}
	return ERROR_SUCCESS;
}
catch (...) {
	return ERROR_INVALID_DATA;
}

unique_EVTHANDLE register_callback(context& ctx) {

	EVT_HANDLE session{};
	HANDLE signal_event{};
	const auto path = L"Application";
	const auto query = L"*[System/EventID=0]";
	EVT_HANDLE bookmark{};

	unique_EVTHANDLE h(EvtSubscribe(session, signal_event, path, query, bookmark, &ctx, callback, EvtSubscribeToFutureEvents));
	REQUIRE(h);
	return h;
}


std::size_t count_messages(const std::string& msg, const std::string& to_find) {
	size_t nPos = msg.find(to_find, 0); // fist occurrence
	size_t count = 0;
	while (nPos != std::string::npos)
	{
		++count;
		nPos = msg.find(to_find, nPos + 1);
	}
	return count;
}

// buffer that contains PTOKEN_USER, accessible throug reinterpret_cast, in case of error empty buffer
std::vector<unsigned char> get_token_user() {
	std::vector<unsigned char> buffer;
	HANDLE hToken{}; // FIXME: should use RAII wrapper
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
	{
		return buffer;
	}

	DWORD dwBufferSize = 0;
	if (!GetTokenInformation(hToken, TokenUser, nullptr, 0, &dwBufferSize) && (GetLastError() != ERROR_INSUFFICIENT_BUFFER))
	{
		CloseHandle(hToken);
		return buffer;
	}
	assert(dwBufferSize >= sizeof(TOKEN_USER));
	buffer.resize(dwBufferSize);
	const auto pTokenUser = reinterpret_cast<PTOKEN_USER>(buffer.data());

	if (!GetTokenInformation(hToken, TokenUser, pTokenUser, dwBufferSize, &dwBufferSize))
	{
		CloseHandle(hToken);
		buffer.clear();
	}
	buffer.resize(dwBufferSize);
	return buffer;
}

TEST_CASE("eventsource", "[eventsource][ostream]") {

	PTOKEN_USER token_user = nullptr;
	auto buf = get_token_user();
	if (!buf.empty()) {
		token_user = reinterpret_cast<PTOKEN_USER>(buf.data());
		REQUIRE(IsValidSid(token_user->User.Sid));
	}

	eventsource_buffer t("test", token_user->User.Sid);
	std::ostream os(&t);

	SECTION("single line") {
		const std::string message = "hello world [single line]";

		context ctx;
		ctx.total_events = 1;

		{
			auto h = register_callback(ctx);
			std::unique_lock<std::mutex> lk(ctx.cv_m);
			os << message << std::endl;
			using namespace std::chrono_literals;
			REQUIRE(ctx.cv.wait_for(lk, 100ms) == std::cv_status::no_timeout);
		}
		REQUIRE(ctx.total_events == 1);

		size_t count = count_messages(ctx.messages.at(0), message);
		REQUIRE(count == 1);

	}

	SECTION("multiple lines at once(1)") {
		const std::string message1 = "hello world [multiple1] 1";
		const std::string message2 = "hello world [multiple1] 2";
		context ctx;

		{
			auto h = register_callback(ctx);
			std::unique_lock<std::mutex> lk(ctx.cv_m);
			os << message1 + "\n" << message2 + "\n" << std::flush;
			using namespace std::chrono_literals;
			REQUIRE(ctx.cv.wait_for(lk, 100ms) == std::cv_status::no_timeout);
		}
		REQUIRE(ctx.total_events == 1);

		size_t count = count_messages(ctx.messages.at(0), message1) + count_messages(ctx.messages.at(0), message2);
		REQUIRE(count == 2);
	}
	SECTION("multiple lines at once(2)") {
		const std::string message1 = "hello world [multiple2] 1";
		const std::string message2 = "hello world [multiple2] 2";
		context ctx;

		{
			auto h = register_callback(ctx);
			std::unique_lock<std::mutex> lk(ctx.cv_m);
			os << message1 + "\n" << message2 << std::flush;
			using namespace std::chrono_literals;
			REQUIRE(ctx.cv.wait_for(lk, 100ms) == std::cv_status::no_timeout);
		}
		REQUIRE(ctx.total_events == 1);

		size_t count = count_messages(ctx.messages.at(0), message1) + count_messages(ctx.messages.at(0), message2);
		REQUIRE(count == 2);
	}
}


#endif

#include <iostream>

#ifndef _WIN32 // if syslog avaiable
TEST_CASE("syslog", "[syslog][ostream]") {

	// AFAIK it is not possible to register the calls to syslog (/var/log/syslog and /var/log/messages on my machine), without manually reading the configuration file
	streamsyslog t("cpp_stream");
	std::ostream os(&t);
	const std::string message = "hello world";

	os << msg_prio(LOG_INFO) << message << std::endl;
	os << msg_prio(LOG_ERR) << message << std::endl;
	os << msg_prio(LOG_DEBUG) << message << std::endl;

}

#endif
