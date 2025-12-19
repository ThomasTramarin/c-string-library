#ifndef SL_STRING_H
#define SL_STRING_H

#include <stddef.h>

typedef char *sl_str; // opaque type

// === ERROR CODES ===
typedef enum {
    SL_OK = 0,
    SL_ERR_ALLOC,
    SL_ERR_INVALID,
    SL_ERR_NULL,
} sl_err;


sl_str sl_from_cstr(const char *init, sl_err *err);
void sl_free(sl_str *str, sl_err *err);
size_t sl_len(sl_str str, sl_err *err);
size_t sl_cap(sl_str str, sl_err *err);

#endif // SL_STRING_H