#include "sl_string.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <stddef.h>

// this constant is used to verify if the string is valid
#define SL_MAGIC 0x534C4942 

/** 
 * Header string
 * 
 * The string is stored as a struct `sl_hdr` followed immediately by
 * the char* buffer in memory.
 * 
 * Total allocated size: sizeof(sl_hdr) + cap
 */
typedef struct sl_hdr {
    uint32_t magic; /**< If set to SL_MAGIC, the string is valid */
    size_t len;     /**< Length of the string (excluding null term) */
    size_t cap;     /**< Capacity of data buffer (including null term) */
    char data[];    /**< Flexible array member (the data buffer) */
} sl_hdr;

/**
 * Utility function to set an error to a sl_err variable
 */
static inline void sl_set_err(sl_err *err, sl_err code) {
    if(err) *err = code;
}

/**
 * Get the pointer to the header of a string
 * 
 * This function calculates the pointer to the `sl_hdr` structure
 * given a pointer to the `data` buffer
 * 
 * @param s Pointer to the string's data buffer
 * @return Pointer to `sl_hdr`
 * 
 * @note This function is only used internally. It assumes the string
 *       was allocated by the library
 */
static inline sl_hdr *sl_get_hdr(const sl_str s) {
    return (sl_hdr *)((char *)s - offsetof(sl_hdr, data));
}


/**
 * Create a new dynamic string (`sl_str`) from a null-terminated C string.
 * 
 * This function allocates memory for the string header and the buffer,
 * copies all characters from `init` into the buffer and sets length and 
 * capacity based on `init`
 * 
 * @param init A pointer to a null-terminated C string. 
 *             It must not contain any zero bytes except the terminating null.
 * @param err A pointer to the error variable
 * 
 * @note The function stops at the first null byte. Any null bytes inside the string
 *       before the terminator will be treated as the end of the string.
 */
sl_str sl_from_cstr(const char *init, sl_err *err){
    if(!init) {
        sl_set_err(err, SL_ERR_NULL);
        return NULL;
    }

    size_t len = strlen(init);
    size_t cap = len + 1; // +1 (the null term)

    sl_hdr *hdr = malloc(offsetof(sl_hdr, data) + cap);
    if(!hdr) {
        sl_set_err(err, SL_ERR_ALLOC);
        return NULL;
    } 

    hdr->magic = SL_MAGIC;
    hdr->len = len;
    hdr->cap = cap;

    // copy `init` into `data`
    memcpy(hdr->data, init, len + 1);

    sl_set_err(err, SL_OK);
    return hdr->data;
}

/**
 * Free the memory allocated for a dynamic string
 * 
 * This function releases the memory of a string previously created.
 * 
 * @param str Pointer to the `sl_str` variable (pointer to the string pointer)
 * @param err A pointer to the error variable
 * @warning The implementation assumes the string was allocated with
 *          a function of this library such as `sl_from_cstr` and has
 *          a valid header before the data buffer.
 */
void sl_free(sl_str *str, sl_err *err){
    // if the string doesn't exists, do nothing
    if(!str || !*str) {
        sl_set_err(err, SL_OK);
        return;
    }

    sl_hdr *hdr = sl_get_hdr(*str);

    if(hdr->magic != SL_MAGIC) {
        sl_set_err(err, SL_ERR_INVALID);
        return;
    }

    hdr->magic = 0; // invalidate the string
    free(hdr);
    *str = NULL;    // prevent use after free

    sl_set_err(err, SL_OK);
}

/**
 * Check the length of a string
 * 
 * @param str Pointer to the string buffer
 * @param err Pointer to an `sl_err` variable, can be NULL
 * @return Length of the string, or `SIZE_MAX` if error occurred
 */
size_t sl_len(sl_str str, sl_err *err){
    if(!str) {
        sl_set_err(err, SL_ERR_NULL);
        return SIZE_MAX;
    }

    sl_hdr *hdr = sl_get_hdr(str);
    if(hdr->magic != SL_MAGIC) {
        sl_set_err(err, SL_ERR_INVALID);
        return SIZE_MAX;
    }

    sl_set_err(err, SL_OK);
    return hdr->len;
}