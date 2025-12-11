#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../strlib.h"

int main(void) {
    sl_err err;

    // Test 1: create + free + NULL
    char *s = sl_create("hello", &err);
    assert(err == SL_OK);
    assert(s != NULL);
    assert(strcmp(s, "hello") == 0);

    err = sl_free(&s);
    assert(err == SL_OK);
    assert(s == NULL);

    // Test 2: free NULL pointer
    err = sl_free(&s);
    assert(err == SL_ERR_NULL);

    // Test 3: create empty string
    s = sl_create(NULL, &err);
    assert(err == SL_OK);
    assert(s != NULL);
    assert(strcmp(s, "") == 0);

    err = sl_free(&s);
    assert(err == SL_OK);
    sl_free(&s);

    // Test 4: length and capacity
    s = sl_create("hello", NULL);
    assert(s != NULL);
    assert(sl_cap(s, NULL) == 16);
    assert(sl_len(s, NULL) == 5);

    sl_free(&s);
    printf("all tests passed\n");
    return 0;
}