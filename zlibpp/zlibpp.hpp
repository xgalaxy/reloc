#ifndef ZLIBPP_ZLIBPP_HPP_INCLUDED
#define ZLIBPP_ZLIBPP_HPP_INCLUDED

// zlib のラッパー

#include <cstddef>

namespace zlibpp {

extern const int NO_FLUSH;
extern const int PARTIAL_FLUSH;
extern const int SYNC_FLUSH;
extern const int FULL_FLUSH;
extern const int FINISH;
extern const int BLOCK;
extern const int TREES;

extern const int OK;
extern const int STREAM_END;
extern const int NEED_DICT;
extern const int ERRNO;
extern const int STREAM_ERROR;
extern const int DATA_ERROR;
extern const int MEM_ERROR;
extern const int BUF_ERROR;
extern const int VERSION_ERROR;

extern const int NO_COMPRESSION;
extern const int BEST_SPEED;
extern const int BEST_COMPRESSION;
extern const int DEFAULT_COMPRESSION;

struct stream {
    const void* next_in;
    std::size_t avail_in;
    std::size_t total_in;

    void* next_out;
    std::size_t avail_out;
    std::size_t total_out;
};
class stream_ptr {
    stream* p;
    typedef void (*deleter)(stream*);
    deleter d;

    stream_ptr(const stream_ptr&);
    stream_ptr& operator=(const stream_ptr&);

public:
    struct move_type {
        stream* p;
        deleter d;
    };

    stream_ptr() : p(0), d(0) { }
    explicit stream_ptr(stream* p, deleter d) : p(p), d(d) { }
    stream_ptr(const move_type& m) : p(m.p), d(m.d) { }
    ~stream_ptr() { reset(); }

    void reset() {
        if (p) {
            d(p);
            p = 0;
            d = 0;
        }
    }
    void reset(const move_type& m) {
        if (p == m.p && d == m.d) return;
        reset();
        p = m.p;
        d = m.d;
    }

    move_type release() {
        move_type m = { p, d };
        p = 0;
        d = 0;
        return m;
    }

    stream* get() const { return p; }
    stream* operator->() const { return p; }
    stream& operator*() const { return *p; }

    typedef stream* stream_ptr::*unspecified_bool_type;
    operator unspecified_bool_type() const {
        return p ? &stream_ptr::p : 0;
    }
};

extern void deflate_init(stream_ptr& sp, int level);
extern int deflate(const stream_ptr& sp, int flush);

extern void inflate_init(stream_ptr& sp);
extern int inflate(const stream_ptr& sp, int flush);

class deflate_stream {
    stream_ptr sp;

public:
    deflate_stream(int level) {
        deflate_init(sp, level);
    }
    int deflate(int flush) {
        return zlibpp::deflate(sp, flush);
    }

    stream* get() const { return sp.get(); }
    stream* operator->() const { return sp.get(); }
    stream& operator*() const { return *sp; }

    typedef stream_ptr deflate_stream::*unspecified_bool_type;
    operator unspecified_bool_type() const {
        return sp ? &deflate_stream::sp : 0;
    }
};

class inflate_stream {
    stream_ptr sp;

public:
    inflate_stream() {
        inflate_init(sp);
    }
    int inflate(int flush) {
        return zlibpp::inflate(sp, flush);
    }

    stream* get() const { return sp.get(); }
    stream* operator->() const { return sp.get(); }
    stream& operator*() const { return *sp; }

    typedef stream_ptr inflate_stream::*unspecified_bool_type;
    operator unspecified_bool_type() const {
        return sp ? &inflate_stream::sp : 0;
    }
};

}

#endif // ZLIBPP_ZLIBPP_HPP_INCLUDED
