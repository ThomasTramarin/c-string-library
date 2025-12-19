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
 *       was allocated by the library (sl_validate will validate the string)
 */
static inline sl_hdr *sl_get_hdr(const sl_str str) {
    return (sl_hdr *)((char *)str - offsetof(sl_hdr, data));
}

/**
 * Validate that the string `str` was created by the library
 * 
 * @param str The string to validate
 * @param out_hdr A pointer to the string header. This param can be NULL
 *                and it is used to avoid recalculating the header in functions
 *                that use it.
 * 
 * @return A value of the sl_err enum representing the error code
 */
static inline sl_err sl_validate(sl_str str, sl_hdr **out_hdr){
    if(!str) return SL_ERR_NULL;
    
    sl_hdr *hdr = sl_get_hdr(str);

    // if the magic does not match the value of the SL_VALID constant,
    // it menas string was not created by the library or has been invalidated
    if(hdr->magic != SL_MAGIC) return SL_ERR_INVALID;

    if(out_hdr) *out_hdr = hdr;

    return SL_OK;
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
    // do nothing
    if (!str || !*str) {
        sl_set_err(err, SL_OK);
        return;
    }

    sl_hdr *hdr;
    sl_err e = sl_validate(*str, &hdr);

    if(e != SL_OK) {
        sl_set_err(err, e);
        return;
    }

    hdr->magic = 0; // invalidate the string
    free(hdr);
    *str = NULL;    // prevent use after free

    sl_set_err(err, SL_OK);
}

/**
 * Get the length of a string
 * 
 * @param str Pointer to the string buffer
 * @param err Pointer to an `sl_err` variable, can be NULL
 * @return Length of the string, or `SIZE_MAX` if error occurred
 */
size_t sl_len(sl_str str, sl_err *err){
    sl_hdr *hdr;
    sl_err e = sl_validate(str, &hdr);

    if(e != SL_OK) {
        sl_set_err(err, e);
        return SIZE_MAX;
    }

    sl_set_err(err, SL_OK);
    return hdr->len;
}

/**
 * Get the capacity of a string
 * 
 * The capacity represents the number of bytes reserved for the string buffer.
 * Thus, the total memory used by the string is sizeof(sl_hdr) + capacity.
 * 
 * @param str Pointer to the string buffer
 * @param err Pointer to an `sl_err` variable, can be NULL
 * @return Capacity of the string, or `SIZE_MAX` if error occurred
 */
size_t sl_cap(sl_str str, sl_err *err){
    sl_hdr *hdr;
    sl_err e = sl_validate(str, &hdr);

    if(e != SL_OK) {
        sl_set_err(err, e);
        return SIZE_MAX;
    }

    sl_set_err(err, SL_OK);
    return hdr->cap;
}

/**
 * Append a null-terminated C string to a `sl_str`
 * 
 * This function appends the content of `init` to the end of `str`
 * The memory is reallocated exactly to fit the new string + term.
 * 
 * @param str The dynamic string to append to. Must be a valid `sl_str`.
 * @param init A null-terminated C string to append. Must not be NULL.
 * @param err Pointer to an `sl_err` variable, can be NULL.
 * 
 * @return The reallocated string pointer after append.
 *         Returns NULL if memory allocation fails.
 */
sl_str sl_append_cstr(sl_str str, const char *init, sl_err *err){
    if(!init) {
        sl_set_err(err, SL_ERR_NULL);
        return str;
    }

    sl_hdr *hdr;
    sl_err e = sl_validate(str, &hdr);

    if(e != SL_OK) {
        sl_set_err(err, e);
        return str;
    }

    // calculate the new length of the string
    size_t init_len = strlen(init);
    size_t new_len = hdr->len + init_len;
    size_t new_cap = new_len + 1;

    // if new capacity exceeds the current capacity, reallocate the memory
    if(new_cap > hdr->cap) {
        sl_hdr *new_hdr = realloc(sl_get_hdr(str), offsetof(sl_hdr, data) + new_cap);
        if(!new_hdr) {
            sl_set_err(err, SL_ERR_ALLOC);
            return NULL; 
        }
        hdr = new_hdr;
    }

    // append the new string
    memcpy(hdr->data + hdr->len, init, init_len + 1);

    // set new field values
    hdr->len = new_len;
    hdr->cap = new_cap;
    
    sl_set_err(err, SL_OK);
    return hdr->data;
}