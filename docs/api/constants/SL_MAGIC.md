# SL_MAGIC

## Definition 

```c
#define SL_MAGIC 0x534C4942
```

## Description
`SL_MAGIC` is a constant used internally to **verify the integrity** of a string.

Every string created with [`sl_create`](../functions/sl_create.md) has a header ([`sl_hdr`](../types/sl_hdr.md)) stored before the payload.
The header includes a `magic` field, which is set to `SL_MAGIC` during allocation.

This allows functions to detect:
- invalid pointers
- corrupted memory
- buffers not allocated by this library

## Notes
- It is automatically written by `sl_create` and cleared by `sl_free`
- Users must not modify this value directly