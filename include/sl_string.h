#ifndef SL_STRING_H
#define SL_STRING_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef char *sl_str; // opaque type

// === ERROR CODES ===
typedef enum {
    SL_OK = 0,
    SL_ERR_ALLOC,
    SL_ERR_INVALID,
    SL_ERR_NULL,
} sl_err;


sl_str sl_from_cstr(const char *init, sl_err *err);
sl_str sl_from_bytes(const void *bytes, size_t len, sl_err *err);


void sl_free(sl_str *str, sl_err *err);
size_t sl_len(sl_str str, sl_err *err);
size_t sl_cap(sl_str str, sl_err *err);
sl_str sl_append_cstr(sl_str str, const char *init, sl_err *err);

bool sl_eq(sl_str str1, sl_str str2, sl_err *err);
uint64_t sl_compute_hash(const void *data, size_t len);
uint64_t sl_compute_hash_cstr(const char *str);
uint64_t sl_hash(sl_str str, sl_err *err);

#endif // SL_STRING_H