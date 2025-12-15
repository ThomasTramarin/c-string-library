#include "sl_string.h"
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <stdio.h>

/** 
 * Header string
 * 
 * The string is stored as a struct `sl_hdr` followed immediately by
 * the char* buffer in memory
 * 
 * Fields:
 * @field len Length of the string (excluding null terminator)
 * @field cap Is the capacity of the data buffer (including space for null terminator)
 *            The total allocated size for the string is sizeof(sl_hdr) + cap
 *          
 * Memory layout:
 *  [sl_hdr struct][data buffer of size cap]
 * 
 * @field data Is a flexible array member. 
 *             The struct itself does not allocate space for it .
 */
typedef struct {
    size_t len;
    size_t cap;
    char data[];
} sl_hdr;

/**
 * Create a new dynamic string (`sl_str`) from a null-terminated C string.
 * 
 * This function allocates memory for the string header and the buffer,
 * copies all characters from `init` into the buffer and sets length and 
 * capacity based on `init`
 * 
 * @param init A pointer to a null-terminated C string. 
 *             It must not contain any zero bytes except the terminating null.
 * 
 * @note The function stops at the first null byte. Any null bytes inside the string
 *       before the terminator will be treated as the end of the string.
 */
sl_str sl_from_cstr(const char *init){
    if(!init) return NULL;

    size_t len = strlen(init);
    size_t cap = len + 1; // +1 (the null term)

    sl_hdr *hdr = malloc(sizeof(sl_hdr) + cap);
    if(!hdr) return NULL; // check malloc

    hdr->len = len;
    hdr->cap = cap;

    // copy `init` into `data`
    memcpy(hdr->data, init, len);
    hdr->data[len] = '\0';

    return hdr->data;
}

/**
 * Free the memory allocated for a dynamic string
 * 
 * This function releases the memory of a string previously created.
 * It safely calculates the pointer to the string header.
 * 
 * @param str Pointer to the `data` member
 * @note If str is null the function does nothing
 * @warning The implementation assumes the string was allocated with
 *          a function of this library such as `sl_from_cstr` and has
 *          a valid header before the data buffer.
 */
void sl_free(sl_str str){
    if(!str) return;

    // calculate the pointer to the header 
    sl_hdr *hdr = (sl_hdr*)((char*)str - offsetof(sl_hdr, data));
    free(hdr);
}
