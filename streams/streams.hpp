#ifndef FEK_STREAMS_HPP
#define FEK_STREAMS_HPP

#include <cassert>
#include <cstring>

#include <streambuf>
#include <istream>
#include <iterator>
#include <limits>
#include <array>

// for now-owning istream (istringstream makes a copy)
class streambufview : public std::streambuf {
public:
    // if c++>=17 add string_view constructor
    streambufview(const char* data, std::size_t len) : m_begin(data), m_end(m_begin + len), m_current(m_begin) { }
    explicit streambufview(const char* data) : streambufview(data, std::strlen(data)) { }

    // FIXME: verify that iterator is randomaccess and of type char
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

#endif
