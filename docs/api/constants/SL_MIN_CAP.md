# SL_MIN_CAP

## Definition

```c
#define SL_MIN_CAP 16
```

## Description
`SL_MIN_CAP` defines the **minimum buffer size** used when creating a string with [`sl_create`](../functions/sl_create.md).

Even if the initial string is shorter, the allocation will always be at least `SL_MIN_CAP` bytes. This **reduces the need for frequent reallocations for short strings** and improves performance.

## Example
```c
#include "strlib.h"
#include <stdio.h>

int main(void) {
    sl_err err;
    char *s = sl_create("hi", &err);

    printf("Capacity: %zu\n", sl_cap(s, NULL)); // 16

    sl_free(&s);
    return 0;
}
```

## Notes
- This constant is primarily for **performance optimization** and memory management.