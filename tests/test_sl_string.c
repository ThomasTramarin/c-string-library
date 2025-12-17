#include "unity.h"
#include "sl_string.h"
#include "string.h"

void setUp(void) {}
void tearDown(void) {}

void test_sl_from_cstr(void) {
    sl_err err;
    sl_str s = sl_from_cstr("Hello", &err);
    TEST_ASSERT_NOT_NULL(s);
    TEST_ASSERT_EQUAL_STRING("Hello", s);
    TEST_ASSERT_EQUAL(SL_OK, err);
    sl_free(&s, NULL);
    TEST_ASSERT_NULL(s);

    s = sl_from_cstr("Hel\0lo", &err);
    TEST_ASSERT_EQUAL_STRING("Hel", s);
    sl_free(&s, NULL);

    s = sl_from_cstr(NULL, &err);
    TEST_ASSERT_EQUAL(SL_ERR_NULL, err);
    TEST_ASSERT_NULL(s);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_sl_from_cstr);
    return UNITY_END();
}