# sl_len

## Description
Returns the **current length of the string**. The value doesn't include the string terminator. 

The function validates the pointer to ensure it belongs to a string created by the library.

---

## Signature
 
```c
size_t sl_len(const char *str, sl_err *out_err)
```

## Parameters 

| Parameter | Type          | Description                                                             |
| --------- | ------------- | ----------------------------------------------------------------------- |
| `str`     | `const char*` | Pointer to the string payload (returned by `sl_create`)                 |
| `out_err` | `sl_err*`     | Optional pointer to receive an error code. Can be `NULL` if not needed. |

## Return value

- **Success**: Capacity of the string buffer in bytes
- **Failure**: 0 (check `out_err` if provided)

## Example

```c
#include "strlib.h"
#include <stdio.h>

int main() {
    sl_err err;
    char *s = sl_create("hello", &err);
    if (err != SL_OK) return 1;

    size_t cap = sl_len(s, &err);
    if (err != SL_OK) {
        fprintf(stderr, "Failed to get length: %d\n", err);
        sl_free(&s);
        return 1;
    }

    printf("Length: %zu\n", cap);

    sl_free(&s);
    return 0;
}
```
