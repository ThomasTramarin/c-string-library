# sl_create

## Description
Allocates the new string, including both the **header** and the **payload buffer**, and returns a pointer to the payload `char*`.

The returned string buffer is **null-terminated**. The function ensures a minimum buffer size of [`SL_MIN_CAP`](../constants/SL_MIN_CAP.md) to reduce reallocations for short strings.

This function optionally returns an error code via the second argument.

## Signature

```c
char* sl_create(const char *init, sl_err *out_err)
```

## Parameters
| Parameter | Type      | Description                                                                               |
| --------- | --------- | ----------------------------------------------------------------------------------------- |
| `init`    | `char*`   | Optional initial string. Can be `NULL` for an empty string.                               |
| `out_err` | `sl_err*` | Optional pointer to receive an error code. Can be `NULL` if you don't need error details. |

## Return Value
- **Success**: Pointer to the string payload.
- **Failure**: `NULL` (check out_err if provided).

## Example

```c
#include "strlib.h"
#include <stdio.h>

int main() {
    sl_err err;
    char *s = sl_create("hello world", &err);

    if (err != SL_OK) {
        fprintf(stderr, "Failed to create string: %d\n", err);
        return 1;
    }

    printf("Created string: %s\n", s);

    sl_free(&s); // remember to free the string
    return 0;
}
```

## Notes
- Passing `NULL` as `init` creates an empty string (equivalent to passing `""`).
- The returned string **must be freed** with [`sl_free`](./sl_free.md).
