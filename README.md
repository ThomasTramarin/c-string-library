# C string library (sl)

`sl_str` is a small C library for managing dynamic strings easily.

This is a **learning project** created to explore and practice with the C programming, memory management and library design.

## Table of Contents
1. [Installation](#installation)
2. [Quick Start](#quick-start)  
    2.1. [Basics](#basics)  
    2.2. [Create a string](#create-a-string)  
    2.3. [Append to a string](#append-to-a-string)  
    2.4. [Get the length and the capacity](#get-the-length-and-the-capacity)  
    2.5. [Use of hashes](#use-of-hashes)
    2.6. [Compare strings](#compare-strings)  
    2.7. [Free a string](#free-a-string)
3. [API Reference](#api-reference)

## Installation

1. **Download the installation script**  

Create a file named `install.sh` in your folder and copy the provided script content into it.

2. **Make the script executable** 

```shell
chmod +x install.sh
```

3. **Run the script**  

```shell
./install.sh
```
This will create a folder `sl_string/` and download the main library files.

## Quick Start

### Basics

`sl_str` is just a `char*`, so you can use it in any function that expects a C string.
The library keeps track of some data fields, for example the length and the capacity by storing a header (`sl_hdr`) just before the buffer.

You never access the header directly, because it is handled automatically: when you pass a `sl_str` to any function in the library, it internally validates the pointer (by using a magic field) and computes the pointer to the header with a pointer subtraction.

The header contains these fields:
- `magic`: A special number to verify the string was created by the library
- `hash`: The hash number of the string (64 bit FNV-1a hashing algorithm)
- `len`: The length of the string (excluding the null terminator)
- `cap`: The total capacity of the string buffer (including null term)
- `data[]`: The actual character buffer (flexible array member)

All functions that can fail take an optional last parameter `sl_err *err`. This pointer will contain an error code if something goes wrong. This is the list of error codes in the `sl_err` enum:
- `SL_OK`: Success
- `SL_ERR_ALLOC`: Memory allocation failed
- `SL_ERR_NULL`: Input pointer was NULL
- `SL_ERR_INVALID`: String is not valid (not created by the library or already freed)

Because of this design, it is recommended to create a `sl_err` variable and check the error code after each operation.

Every `sl_str` returned by the library is owned by the caller.
The caller is responsible for freeing the string using `sl_free`. After calling `sl_free`, the pointer is set to `NULL` and must not be used again.


### Create a string

You can create dynamic string from a normal C null-terminated string using the `sl_from_cstr` function. This function initializes a new string and copies the content of `init` into the data buffer.

For example, creating a string containing `"Hello"` and check if the operation succeeded.
```c
sl_err err;
sl_str s = sl_from_cstr("Hello", &err);

if (err != SL_OK) {
    fprintf(stderr, "Error creating the string: %d\n", err);
    return 1;
}

printf("%s\n", s);
```

### Append to a string

To add more content to a string, use `sl_append_cstr`. This function appends a null-terminated C string to your existing `sl_str`. 
For example:

```c
s = sl_append_cstr(s, " World", &err);
```
Now `s` contains `"Hello World"`

### Get the length and the capacity
You can check the current length of a string with `sl_len`, which returns the number of characters excluding the null terminator. `sl_cap` returns the total capacity of the string buffer including the null terminator.

```c
size_t len = sl_len(s, &err);
size_t cap = sl_cap(s, &err);
printf("Length: %zu\nCapacity: %zu\n", len, cap);
```

### Use of hashes
Each `sl_str` string contains a `hash` field in the header. This is a `uint64_t` number that represents the **64 bit FNV-1a hash** of the current string content. So when the string content changes, also the hash is recalculated.

To get the hash of a dynamic string, you can use the function `sl_hash`. For example:
```c
uint64_t s_hash = sl_hash(s, &err);
printf("%zu", s_hash);
```

You can compute hashes using the `sl_compute_hash` and `sl_compute_hash_cstr` functions.
This allows you to compare a dynamic string without performing a full string comparison.

For example:
```c
const char *keyword = "while";
uint64_t keyword_hash = sl_compute_hash_cstr(keyword);

sl_err err;
sl_str s = sl_from_cstr("while", &err);

if (sl_hash(s, NULL) == keyword_hash) {
    printf("Keyword matched!\n");
}

sl_free(&s, NULL);
```

### Compare strings
You can check if two dynamic strings are equal using the `sl_eq` function. This function first compares the hashes and lengths of the strings. Only if both match it perform a byte-by-byte comparison.

```c
sl_str s1 = sl_from_cstr("Hello", &err);
sl_str s2 = sl_from_cstr("Hello", &err);

if(sl_eq(s1, s2, NULL)) {   // Note that you can always pass NULL if you do not check for errors
    printf("Strings are equal!\n");
} else {
    printf("Strings are different.\n");
}
```

### Free a string
After you are done using a string, you should free its memory using `sl_free`. This prevents memory leaks and invalidates the pointer internally, so any further use of it will be detected as an error.

The function takes the pointer to the `sl_str` variable because it sets the pointer to `NULL` after freeing the memory.

```c
sl_free(&s, &err);
if (err != SL_OK) {
    fprintf(stderr, "Error freeing the string: %d\n", err);
} else {
    printf("String successfully freed\n");
}
```

## Example
Copy this code into your project to see the library in action.
```c
#include "sl_string.h" // Include the header file
#include <stdio.h>

int main(void) {
    sl_err err = SL_OK;

    // Create a dynamic string from a C string
    sl_str s = sl_from_cstr("Hello", &err);
    if(!s) {
        fprintf(stderr, "Error creating the string: %d\n", err);
        return 1;
    }

    printf("Initial string: %s\n", s);
    printf("Length: %zu\n", sl_len(s, NULL));
    printf("Capacity: %zu\n\n", sl_cap(s, NULL));

    // Append a new C string
    s = sl_append_cstr(s, " World", &err);
    if(!s) {
        fprintf(stderr, "Error appending string: %d\n", err);
        return 1;
    }

    printf("After append: %s\n", s);
    printf("Length: %zu\n", sl_len(s, &err));
    printf("Capacity: %zu\n\n", sl_cap(s, &err));

    // Free the string
    sl_free(&s, &err);
    if(!s) {
        printf("String successfully freed\n");
    }

    return 0;
}
```

## API Reference

### `sl_from_cstr`
```c
sl_str sl_from_cstr(const char *init, sl_err *err);
```
#### Description
Creates a new string from a null-terminated C string.

#### Parameters
- `init`: Pointer to a C string
- `err`: Pointer to a `sl_err` variable, can be `NULL`

#### Returns
- `sl_str` on success.
- `NULL` if creation fails (check `sl_err` for more details).
#### Error Codes
- `SL_OK`: Success
- `SL_ERR_ALLOC`: Memory allocation failed
- `SL_ERR_NULL`: Input `init` is `NULL`

---

### `sl_free`
```c
void sl_free(sl_str *str, sl_err *err);
```

#### Description
Frees the memory allocated and invalidates the pointer (set it to `NULL` and invalidate `magic`).

#### Parameters
- `str`: Pointer to the `sl_str` variable
- `err`: Pointer to a `sl_err` variable, can be `NULL`

#### Error Codes
- `SL_OK`: Success
- `SL_ERR_INVALID`: String is not valid (not created by the library)
- `SL_ERR_NULL`: Input pointer is `NULL`

---

### `sl_len`
```c
size_t sl_len(sl_str str, sl_err *err);
```

#### Description
Returns the length of a string.

#### Parameters
- `str`: Pointer to the `sl_str` variable
- `err`: Pointer to a `sl_err` variable, can be `NULL`

#### Returns
- The length of the string (number of characters excluding the null terminator) on success.
- `SIZE_MAX` if an error occured (check `err` for more details).

#### Error Codes
- `SL_OK`: Success
- `SL_ERR_INVALID`: String is not valid (not created by the library)
- `SL_ERR_NULL`: Input pointer is `NULL`

---

### `sl_cap`
```c
size_t sl_cap(sl_str str, sl_err *err);
```

#### Description
Returns the capacity of a string

#### Parameters
- `str`: Pointer to the `sl_str` variable
- `err`: Pointer to a `sl_err` variable, can be `NULL`

#### Returns
- The capacity of the string (number of bytes reserved for the string buffer) on success.
- `SIZE_MAX` if an error occured (check `err` for more details).

#### Error Codes
- `SL_OK`: Success
- `SL_ERR_INVALID`: String is not valid (not created by the library)
- `SL_ERR_NULL`: Input pointer is `NULL`

---

### `sl_append_cstr`

```c
sl_str sl_append_cstr(sl_str str, const char *init, sl_err *err);
```

#### Description
Appends the content of a null-terminated C string to the end of a dynamic string.
Memory is reallocated exactly to fit the new length + null terminator if needed.

#### Parameters
- `str`: the dynamic string to append to
- `init`: the C string to append
- `err`: Pointer to a `sl_err` variable, can be `NULL`

#### Returns
- The updated string pointer (possibly reallocated).
- If an error occurs, the original string is returned unchanged and `err` is set.

#### Error Codes
- `SL_OK`: Success
- `SL_ERR_ALLOC`: Memory allocation failed
- `SL_ERR_INVALID`: String is not valid
- `SL_ERR_NULL`: Input `init` is null or string pointer is `NULL`

---

### `sl_eq`

```c
bool sl_eq(sl_str str1, sl_str str2, sl_err *err);
```

#### Description
Checks if two dynamic strings are equal. The function first compares string hashes and lengths to quickly detect inequality. Only if both match, it performs a byte-by-byte comparison of the string data.

#### Parameters
- `str1`: First string to compare
- `str2`: Second string to compare
- `err`: Pointer to a `sl_err` variable, can be `NULL`

#### Returns
- `true` if strings are equal
- `false` otherwise (or if there is an error)

#### Error Codes
- `SL_OK`: Success
- `SL_ERR_NULL`: One or both input strings are `NULL`
- `SL_ERR_INVALID`: One or both strings are not valid `sl_str`

---

### `sl_hash`

```c
uint64_t sl_hash(sl_str str, sl_err *err);
```

#### Description
Returns the precomputed hash of a dynamic string.
The hash is calculated internally using the **64-bit FNV-1a algorithm** and is automatically updated every time the string content changes (for example after `sl_append_cstr`).

This function is useful for fast comparisons, hash tables, and parsers.

#### Parameters
- `str`: Pointer to a valid `sl_str`
- `err`: Pointer to a `sl_err` variable, can be `NULL`

#### Returns
- The hash of the string on success.
- `0` if an error occured (check `err`)

#### Error Codes
- `SL_OK`: Success
- `SL_ERR_NULL`: Input string is `NULL`
- `SL_ERR_INVALID`: Input string is not a valid `sl_str`

---

### `sl_compute_hash`

```c
uint64_t sl_compute_hash(const void *data, size_t len);
```

#### Description
Calculate a **64-bit FNV-1a hash** of a generic memory buffer.

This function works on **binary data**, so it can calculate buffers containing `'\0'` bytes.

#### Parameters
- `data`: Pointer to a memory buffer
- `len`: Length of the buffer in bytes

#### Returns
- The computed hash.

#### Notes
- This function never fails.

---

### `sl_compute_hash_cstr`

```c
uint64_t sl_compute_hash_cstr(const char *str);
```

#### Description
Calculate a **64-bit FNV-1a hash** of a null-terminated C string.

This function is a wrapper of `sl_compute_hash` that automaticaly calculates the string length using
`strlen`. Thus, if you work with normal C strings (and not binary buffer), you can use this function.

#### Parameters
- `str`: Pointer to a null-terminated C string

#### Returns
- The computed 64-bit hash.
- `0` if `str` is `NULL`.

#### Notes
- This function never fails.
