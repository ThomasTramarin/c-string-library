#ifndef STRLIB_H
#define STRLIB_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define SL_MAGIC 0x534C4942
#define SL_MIN_CAP 16

typedef enum {
    SL_OK = 0,
    SL_ERR_NULL,            // null pointer
    SL_ERR_ALLOC,           // malloc/realloc failed
    SL_ERR_OVERFLOW,        // cap/len overflow
    SL_ERR_INVALID_MAGIC,   // magic doesn't match with SL_MAGIC
    SL_ERR_TERM,            // null terminator '\0' missing
} sl_err;

char *sl_create(const char *init, sl_err *out_err);
sl_err sl_free(char **pstr);
sl_err sl_validate(const char *str);
size_t sl_cap(const char *str, sl_err *out_err);
size_t sl_len(const char *str, sl_err *out_err);

#endif