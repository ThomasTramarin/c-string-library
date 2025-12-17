# C string library (sl)

`sl_str` is a small C library for managing dynamic strings easily.

This is a **learning project** created to explore and practice with the C programming, memory management and library design.

## Installation
You can download the code using the provided `install.sh` script:
```bash
chmod +x install.sh
./install.sh
```
This will:
- Create a folder `sl_string/`.
- Download the main library files into that folder.

After running the script, you can move files into your project and start using the library.

## How It Works

sl_string uses a **dynamic string structure**, but exposes only a `char*` pointer (`sl_str`) to the user. This keeps the API simple and safe, while internally managing memory and metadata.

### Memory Layout

Every string created by the library has a small header (`sl_hdr`) before the actual string data in memory:
```
[sl_hdr | data buffer]
```

Where `sl_hdr` contains:

- `magic`: A special number to verify the string was created by the library
- `len`: The length of the string (excluding the null terminator)
- `cap`: The total capacity of the string buffer (including null term)
- `data[]`: The actual character buffer (flexible array member)

Users never access the header directly. They only use the `sl_str` pointer (pointer to data buffer)

## Example
```c
#include <sl_string.h> // Include the header file
#include <stdio.h>

int main(void) {
    sl_err err;

    // Create a dynamic string from a C string
    sl_str s = sl_from_cstr("Hello World", &err);
    if(!s) {
        fprintf(stderr, "Error creating the string: %d\n", err);
        return 1;
    }

    // Print the string
    printf("%s\n", s);

    // Free the string
    sl_free(&s, &err);
    if(!s) {
        printf("String successfully freed\n");
    }

    return 0;
}
```

## API
### sl_from_cstr
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

### sl_free
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
