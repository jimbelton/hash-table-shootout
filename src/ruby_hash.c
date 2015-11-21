#include <ruby.h>
#define SETUP \
    ruby_init(); \
    VALUE hash = rb_hash_new(); \
    VALUE rb_int_value = INT2NUM(0);
#define INSERT_INT_INTO_HASH(key, value) do { \
        VALUE rb_int_key = INT2NUM(key); /* leak */ \
        rb_hash_aset(hash, rb_int_key, rb_int_value); \
    } while(0)
#define DELETE_INT_FROM_HASH(key) do { \
        VALUE rb_int_key = INT2NUM(key); /* leak */ \
        rb_hash_delete(hash, rb_int_key); \
    } while(0)
#define INSERT_STR_INTO_HASH(key, value) do { \
        VALUE rb_str_key = rb_str_new2(key); /* leak */ \
        rb_hash_aset(hash, rb_str_key, rb_int_value); \
    } while(0)
#define DELETE_STR_FROM_HASH(key) do { \
        VALUE rb_str_key = rb_str_new2(key); /* leak */ \
        rb_hash_delete(hash, rb_str_key); \
    } while(0)
#define STR_HASH_GET_SIZE() ((size_t)rb_hash_size(hash))

static size_t rb_hash_size_accumulator;

static int rb_hash_size_visit(void) {
    rb_hash_size_accumulator++;
    return ST_CONTINUE;
}

static size_t rb_hash_size(VALUE hash)
{
    rb_hash_size_accumulator = 0;
    rb_hash_foreach(hash, rb_hash_size_visit, (VALUE)0L);
    return rb_hash_size_accumulator;
}

#include "template.c"
