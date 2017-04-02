#ifndef FEK_STREAMS_HPP
#define FEK_STREAMS_HPP

#include <cstring>

#include <streambuf>
#include <istream>
#include <iterator>


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

#endif
