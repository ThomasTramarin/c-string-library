# sl_err

## Descrription
`sl_err` is used by all the functions that can fail, e.g., `sl_create` or `sl_free`.

- Some functions **return `sl_err` directly** when there are no additional output parameters.
- Other functions **take a `sl_err*` as the last argument**, to report error codes while returning another value (e.g., `sl_create` returns a string pointer and sets `sl_err` via the pointer).

---

## Error Code Explanation
| Error Code             | Meaning / When it occurs                               |
| ---------------------- | ------------------------------------------------------ |
| `SL_OK`                | Operation succeeded without any error                  |
| `SL_ERR_NULL`          | A NULL pointer was passed to a function                |
| `SL_ERR_ALLOC`         | Memory allocation (malloc/realloc) failed              |
| `SL_ERR_OVERFLOW`      | String length or capacity would overflow `size_t`      |
| `SL_ERR_INVALID_MAGIC` | Pointer is not a valid string (magic number mismatch)  |
| `SL_ERR_TERM`          | String is missing null-terminator at expected position |

## How to use `sl_err`

### Example 1: Function returning `sl_err` directly

```c
sl_err err;
char *s = sl_create("hello", NULL); // ignore error type
sl_err e = sl_free(&s);

if (e != SL_OK) {
    fprintf(stderr, "failed to free string: %d\n", e);
}
```

### Example 2: Function returning a pointer with `sl_err*`

```c
sl_err err;
char *s = sl_create("hello", &err);

if (err != SL_OK) {
    // handle error
    fprintf(stderr, "failed to create string: %d\n", err);
}
```