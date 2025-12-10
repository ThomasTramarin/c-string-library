#include "strlib.h"

/**
 * string header
 * this struct is stored immediately before the string payload in memory
 * it contains metadata to manage the string
 */
typedef struct {
    uint32_t magic; // magic number to detect invalid pointers
    size_t len;     // current length of the string (not including '\0')
    size_t cap;     // the total capacity of the buffer in bytes
} sl_hdr;

/**
 * get the pointer to the header from a pointer to the string payload
 * note: str must point to the payload returned by sl_create
 */
static inline sl_hdr* sl_get_hdr(const char *str) { 
    return (sl_hdr*)((const char*)str - sizeof(sl_hdr));
}

/**
 * validate a pointer and returns an sl_err code
 */
sl_err sl_validate(const char *str) {
    if(!str) return SL_ERR_NULL;
    sl_hdr *hdr = sl_get_hdr(str);

    if(hdr->magic != SL_MAGIC) return SL_ERR_INVALID_MAGIC;
    if(hdr->len > hdr->cap) return SL_ERR_OVERFLOW;
    if(str[hdr->len] != '\0') return SL_ERR_TERM;

    return SL_OK;
}

/**
 * this function allocates the memory for both the header and the string buffer
 * it returns a pointer to the buffer
 * 
 * the buffer size is at least SL_MIN_CAP to reduce reallocations for small strings
 */
char* sl_create(const char *init, sl_err *out_err) {
    size_t length = init ? strlen(init) : 0; // length of initial string (0 if init is NULL)

    //calculate payload size (length + term). Min 16 bytes to reduce reallocations with short strings
    size_t payload_size = length + 1; 
    if(payload_size < SL_MIN_CAP) payload_size = SL_MIN_CAP;

    // check for overflow
    if(payload_size > SIZE_MAX - sizeof(sl_hdr)) {
        if(out_err) *out_err = SL_ERR_OVERFLOW;
        return NULL;
    }

    // allocate the memory for the header + payload
    sl_hdr *hdr = (sl_hdr*) malloc(sizeof(sl_hdr) + payload_size);
    if(!hdr) {
        if(out_err) *out_err = SL_ERR_ALLOC;
        return NULL;
    }

    // pointer to the payload
    char *buf = (char*)(hdr+1);
    
    // initialize header
    hdr->len = length;
    hdr->cap = payload_size;
    hdr->magic = SL_MAGIC;

    // copy
    if (length == 0) buf[0] = '\0';
    else memcpy(buf, init, length+1);

    if (out_err) *out_err = SL_OK;
    return buf;
}

/**
 * free a string safely
 * pstr: pointer to the string payload pointer (char**)
 */
sl_err sl_free(char **pstr){
    if (!pstr || !*pstr) return SL_ERR_NULL;

    sl_hdr *hdr = sl_get_hdr(*pstr);
    if (hdr->magic != SL_MAGIC) return SL_ERR_INVALID_MAGIC;

    hdr->magic = 0; // invalidate
    free(hdr);
    *pstr = NULL;    // prevent dangling pointer

    return SL_OK;
}

/**
 * returns the capacity of the string buffer (not including the header)
 * 
 * str: pointer to the string payload
 *
 * return:
 *   - on success: capacity of the buffer in bytes
 *   - on failure: 0 (check out_err if provided)
 */
size_t sl_cap(const char *str, sl_err *out_err){
    if(!str) {
        if(out_err) *out_err = SL_ERR_NULL;
        return 0;
    }

    sl_hdr *hdr = sl_get_hdr(str);

    if (hdr->magic != SL_MAGIC) {
        if (out_err) *out_err = SL_ERR_INVALID_MAGIC;
        return 0;
    }

    if (out_err) *out_err = SL_OK;
    return hdr->cap;
}