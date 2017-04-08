#ifndef FEK_STREAMS_HPP
#define FEK_STREAMS_HPP

// windows
#ifdef _WIN32
#include <windows.h>
#endif

// libc
#include <cassert>
#include <cstring>

// std
#include <streambuf>
#include <istream>
#include <iterator>
#include <limits>
#include <array>

#ifdef _WIN32
#include <vector>
#endif

// for now-owning istream (istringstream makes a copy)
class streambufview : public std::streambuf {
public:
    // if c++>=17 add string_view constructor
    streambufview(const char* data, std::size_t len) : m_begin(data), m_end(m_begin + len), m_current(m_begin) { }
    explicit streambufview(const char* data) : streambufview(data, std::strlen(data)) { }

    // FIXME: verify that iterator is of type char
    template <class it>
    streambufview(it begin_, it end_) : m_begin(&(*begin_), &(*end_)) {
        static_assert(std::is_same<std::random_access_iterator_tag, typename std::iterator_traits<it>::iterator_category>::value,
            "streambufview only accepts random access iterators.");
    }

private:
    virtual int_type underflow() override {
        if (m_current == m_end) {
            return traits_type::eof();
        }
        return traits_type::to_int_type(*m_current);
    }
    virtual int_type uflow() override {
        if (m_current == m_end) {
            return traits_type::eof();
        }
        return traits_type::to_int_type(*m_current++);
    }
    virtual int_type pbackfail(int_type ch) override {
        if (m_current == m_begin || (ch != traits_type::eof() && ch != *(m_current -1))) {
            return traits_type::eof();
        }
        --m_current;
        return traits_type::to_int_type(*m_current);
    }
    virtual std::streamsize showmanyc() override {
        return m_end - m_current;
    }

    const char* const m_begin;
    const char* const m_end;
    const char* m_current;
};

// convenience class
class istringviewstream : public std::istream {
private:
    streambufview b;
public:
    // if c++>=17 add string_view constructor
    istringviewstream(const char* data, std::size_t len) : std::istream(nullptr), b(data, len){
        rdbuf(&b);
    }
    explicit istringviewstream(const char* data) : istringviewstream(data, std::strlen(data)) {}
};

template<size_t buffer_size = 128>
class fileviewbuf_base : public std::streambuf
{
public:
    explicit fileviewbuf_base(FILE* f) :
        m_file(f)
    {
        char* end = m_buffer.data() + m_buffer.size();
        setg(end, end, end);
        char* begin = m_buffer.data();
        setp(begin, end);
    }

private:
    virtual int_type underflow() override
    {
        if (gptr() < egptr()){
            return traits_type::to_int_type(*gptr());
        }

        char* base = m_buffer.data();

        const size_t n = std::fread(base, sizeof(char), m_buffer.size(), m_file);
        if (n == 0){
            return traits_type::eof();
        }

        // Set buffer pointers
        setg(base, base, base + n);

        return traits_type::to_int_type(*gptr());
    }
    virtual int_type overflow(int_type c) override {
        if (traits_type::eq_int_type(c, traits_type::eof())) {
            if(sync() == -1){
                return traits_type::eof();
            }
        }else{
            *pptr() = traits_type::to_char_type(c);
            pbump(1);
        }
        return traits_type::to_char_type(c);
    }
    virtual int_type sync() override{
        const auto num = pptr()-pbase();
        assert(num >= 0);
        const auto unum = static_cast<typename std::make_unsigned<decltype(num)>::type>(num);
        const auto res = ::fwrite(pbase(), sizeof(char_type), unum, m_file);
        if ( res == 0 ){
            return int_type{-1};
        }

        assert(num <= (std::numeric_limits<int_type>::max)());
        auto inum = static_cast<int_type>(num);
        pbump(-inum);
        return inum;
    }


private:
    FILE* m_file;
    std::array<char, buffer_size> m_buffer;
};

using fileviewbuf = fileviewbuf_base<>;

class iofilestrem : public std::iostream {
private:
    fileviewbuf f;
public:
    iofilestrem(FILE* fptr) : std::iostream(nullptr), f(fptr){
        rdbuf(&f);
    }
};

#ifdef _WIN32
inline std::size_t linelen(const char* str) {
	const char *s;
	for (s = str; *s != '\0' && *s != '\n'; ++s) {}
	return(s - str);
}
inline std::vector<const char*> create_zzstring_view(std::string& str) {
	std::vector<const char*> toreturn;
	for(std::size_t i = 0; i < str.size(); ) {
		const char* it = &str.at(i);
		const auto len = linelen(it);
		if (len > 0) {
			toreturn.push_back(it);
		}
		i += len;
		if (i < str.size()) {
			str.at(i) = '\0';
		}
		i += 1;
	}
	return toreturn;
}
void writedata(HANDLE hEventLog, std::string& buffer, const PSID usersid = nullptr) { // add other params required by ReportEvent
	if (buffer.empty()) {
		return;
	}
	auto strings = create_zzstring_view(buffer);
	const auto num_string = static_cast<WORD>(strings.size());

	const WORD type = EVENTLOG_INFORMATION_TYPE;
	const WORD category = 0;
	const DWORD identifier = 0;
	const DWORD binarydatasize = 0;
	void* binarydata = 0;
	ReportEventA(hEventLog, type, category, identifier, usersid, num_string, binarydatasize, strings.data(), binarydata);
	buffer.clear();
}

// source that redirects output to Windows Eventlog
class eventsource_buffer : public std::streambuf
{
	HANDLE hEventSource{};
	std::string buffer;
	const PSID psid = nullptr;
public:
	explicit eventsource_buffer(const std::string& name, const PSID psid_ = nullptr) :
		std::basic_streambuf<char>(), hEventSource(RegisterEventSourceA(nullptr, name.c_str())), psid(psid_)
	{
	}
	virtual ~eventsource_buffer() {
		writedata(hEventSource, buffer);
		DeregisterEventSource(hEventSource);
	}

protected:

	virtual int sync() override // not called when destructing ostream
	{
		writedata(hEventSource, buffer, psid);
		return 0;
	}

	virtual int_type overflow(int_type c = traits_type::eof()) override
	{
		if (traits_type::eq_int_type(c, traits_type::eof())) {
			sync();
		}
		else {
			buffer += traits_type::to_char_type(c);
		}
		return c;
	}
};

#endif

#endif
