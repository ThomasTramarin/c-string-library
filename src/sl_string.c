#include "sl_string.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <stddef.h>
#include <stdbool.h>

// this constant is used to verify if the string is valid
#define SL_MAGIC 0x534C4942

#define FNV_PRIME 1099511628211ULL
#define FNV_OFFSET 14695981039346656037ULL


/* ===== INTERNAL FUNCTIONS ===== */

/** 
 * Header string
 * 
 * The string is stored as a struct `sl_hdr` followed immediately by
 * the char* buffer in memory.
 * Total allocated size: sizeof(sl_hdr) + cap
 */
typedef struct sl_hdr {
    uint32_t magic; /**< If set to SL_MAGIC, the string is valid */
    uint64_t hash;  /**< String hash number */
    size_t len;     /**< Length of the string (excluding null term) */
    size_t cap;     /**< Capacity of data buffer (including null term) */
    char data[];    /**< Flexible array member (the data buffer) */
} sl_hdr;

/**
 * Utility function to set an error to a sl_err variable
 */
static inline void sl__set_err(sl_err *err, sl_err code) {
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
static inline sl_hdr *sl__get_hdr(const sl_str str) {
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
static inline sl_err sl__validate(sl_str str, sl_hdr **out_hdr){
    if(!str) return SL_ERR_NULL;
    
    sl_hdr *hdr = sl__get_hdr(str);

    // if the magic does not match the value of the SL_VALID constant,
    // it menas string was not created by the library or has been invalidated
    if(hdr->magic != SL_MAGIC) return SL_ERR_INVALID;

    if(out_hdr) *out_hdr = hdr;

    return SL_OK;
}

/**
 * Get the hash number from a series of bytes (fnv-1a)
 */
static uint64_t sl__compute_hash(const void *bytes, size_t len) {
    uint64_t hash = FNV_OFFSET;
    const unsigned char *ptr = (const unsigned char*) bytes;

    for(size_t i=0; i<len; i++) {
        hash ^= ptr[i];
        hash *= FNV_PRIME;
    }

    return hash;
}

/* ===== PUBLIC API FUNCTIONS ===== */

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
        sl__set_err(err, SL_ERR_NULL);
        return NULL;
    }

    size_t len = strlen(init);
    size_t cap = len + 1; // +1 (the null term)

    sl_hdr *hdr = malloc(offsetof(sl_hdr, data) + cap);
    if(!hdr) {
        sl__set_err(err, SL_ERR_ALLOC);
        return NULL;
    } 

    hdr->magic = SL_MAGIC;
    hdr->len = len;
    hdr->cap = cap;
    
    // copy `init` into `data`
    memcpy(hdr->data, init, len + 1);
    
    hdr->hash = sl__compute_hash(hdr->data, len);
    sl__set_err(err, SL_OK);
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
        sl__set_err(err, SL_OK);
        return;
    }

    sl_hdr *hdr;
    sl_err e = sl__validate(*str, &hdr);

    if(e != SL_OK) {
        sl__set_err(err, e);
        return;
    }

    hdr->magic = 0; // invalidate the string
    free(hdr);
    *str = NULL;    // prevent use after free

    sl__set_err(err, SL_OK);
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
    sl_err e = sl__validate(str, &hdr);

    if(e != SL_OK) {
        sl__set_err(err, e);
        return SIZE_MAX;
    }

    sl__set_err(err, SL_OK);
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
    sl_err e = sl__validate(str, &hdr);

    if(e != SL_OK) {
        sl__set_err(err, e);
        return SIZE_MAX;
    }

    sl__set_err(err, SL_OK);
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
        sl__set_err(err, SL_ERR_NULL);
        return str;
    }

    sl_hdr *hdr;
    sl_err e = sl__validate(str, &hdr);

    if(e != SL_OK) {
        sl__set_err(err, e);
        return str;
    }

    // calculate the new length of the string
    size_t init_len = strlen(init);
    size_t new_len = hdr->len + init_len;
    size_t new_cap = new_len + 1;

    // if new capacity exceeds the current capacity, reallocate the memory
    if(new_cap > hdr->cap) {
        sl_hdr *new_hdr = realloc(sl__get_hdr(str), offsetof(sl_hdr, data) + new_cap);
        if(!new_hdr) {
            sl__set_err(err, SL_ERR_ALLOC);
            return NULL; 
        }
        hdr = new_hdr;
    }

    // append the new string
    memcpy(hdr->data + hdr->len, init, init_len + 1);

    // set new field values
    hdr->len = new_len;
    hdr->cap = new_cap;
    hdr->hash = sl__compute_hash(hdr->data, new_len);
    
    sl__set_err(err, SL_OK);
    return hdr->data;
}

/**
 * Check if two strings (`sl_str`) are equal
 * 
 * This function first compares string hashes and lengths to quickly detect inequality. 
 * Only if both hash and length are equal, it performs a byte-by-byte comparison to be
 * sure strings have the same content.
 * 
 * Time complexity:
 * - Best case (hash/length mismatch or pointer equality) O(1)
 * - Worst case (hash and length match): O(n)
 * 
 * @param str1 The first `sl_str` to compare
 * @param str2 The second `sl_str` to compare
 * @param err Pointer to an `sl_err` variable, can be NULL.
 * 
 * @return `true` if the two strings are equal and `false` otherwise
 * 
 * @note Two strings are considered equal if they have the same length and content
 *       or if they point to the same string
 */
bool sl_eq(sl_str str1, sl_str str2, sl_err *err) {
    sl_hdr *h1, *h2;

    sl_err e1 = sl__validate(str1, &h1);
    sl_err e2 = sl__validate(str2, &h2);

    if(e1 != SL_OK || e2 != SL_OK) {
        sl__set_err(err, e1 != SL_OK ? e1 : e2);
        return false;
    }

    if(str1 == str2) {
        sl__set_err(err, SL_OK);
        return true;
    }

    if(h1->hash != h2->hash) {
        sl__set_err(err, SL_OK);
        return false;
    }

    if(h1->len != h2->len) {
        sl__set_err(err, SL_OK);
        return false;
    }

    bool eq = memcmp(h1->data, h2->data, h1->len) == 0;
    sl__set_err(err, SL_OK);
    return eq;
}