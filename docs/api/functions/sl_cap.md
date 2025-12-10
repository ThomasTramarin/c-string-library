# sl_cap

## Description
Returns the **capacity of the string buffer** allocated by [`sl_create`](sl_create.md).  

This is the total buffer size in bytes, **not including the header**, and includes space for the null terminator.  

The function validates the pointer to ensure it belongs to a string created by the library.

---

## Signature

```c
size_t sl_cap(const char *str, sl_err *out_err)
```

## Parameters 

| Parameter | Type          | Description                                                             |
| --------- | ------------- | ----------------------------------------------------------------------- |
| `str`     | `const char*` | Pointer to the string payload (returned by `sl_create`)                 |
| `out_err` | `sl_err*`     | Optional pointer to receive an error code. Can be `NULL` if not needed. |

## Return value

- **Success**: Capacity of the string buffer in bytes
- **Failure**: 0 (check `out_err` if provided):
    - `SL_ERR_NULL`
    - `SL_ERR_INVALID_MAGIC`

## Example

```c
#include "strlib.h"
#include <stdio.h>

int main() {
    sl_err err;
    char *s = sl_create("hello", &err);
    if (err != SL_OK) return 1;

    size_t cap = sl_cap(s, &err);
    if (err != SL_OK) {
        fprintf(stderr, "Failed to get capacity: %d\n", err);
        sl_free(&s);
        return 1;
    }

    printf("Buffer capacity: %zu\n", cap);

    sl_free(&s);
    return 0;
}
```

## Notes
- The returned value includes space for the null terminator `\0`.