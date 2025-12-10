# C libstr

A small library written in C to handle strings, created as a learning project.

## Getting started
You can start use the library by includinf the two source files in your project:

```
/your_project
  ├── strlib.h
  └── strlib.c
```

Then include the header:
```c
#include "strlib.h"
```

### Compilation

If your project has a `main.c`, you can compile and link everything together like this:
```bash
gcc -o my_program main.c strlib.c
```

Now you can run your program:
```bash
./my_program
```