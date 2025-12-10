# sl_free

## Description
Frees a string allocated with [`sl_create`](sl_create.md) and sets the pointer to `NULL` to prevent dangling references.  

This function validates the pointer before freeing it to ensure it points to a valid string created by the library.

## Signature

```c
sl_err sl_free(char **pstr);
```

## Parameters
| Parameter | Type     | Description                                                                 |
| --------- | -------- | --------------------------------------------------------------------------- |
| `pstr`    | `char**` | Pointer to the string payload pointer. The function sets `*pstr` to `NULL`. |

## Return Value

- **Success**: SL_OK
- **Failure**: one of the sl_err codes:
    - `SL_ERR_NULL`
    - `SL_ERR_INVALID_MAGIC`

## Example

```c
#include "strlib.h"
#include <stdio.h>

int main() {
    sl_err err;
    char *s = sl_create("hello world", &err);
    if (err != SL_OK) {
        fprintf(stderr, "Failed to create string\n");
        return 1;
    }

    printf("String before free: %s\n", s);

    err = sl_free(&s);
    if (err != SL_OK) {
        fprintf(stderr, "Failed to free string: %d\n", err);
        return 1;
    }

    if (!s) {
        printf("Pointer successfully set to NULL after free.\n");
    }

    return 0;
}
```

## Notes
- Always pass a **pointer to the payload pointer** (`char**`).
- After calling `sl_free`, the original pointer is set to `NULL`.