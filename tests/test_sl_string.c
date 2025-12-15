#include "unity.h"
#include "sl_string.h"
#include "string.h"

void setUp(void) {}
void tearDown(void) {}

void test_sl_from_cstr(void) {
    sl_str s = sl_from_cstr("Hello");
    TEST_ASSERT_NOT_NULL(s);
    TEST_ASSERT_EQUAL_STRING("Hello", s);
    sl_free(s);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_sl_from_cstr);
    return UNITY_END();
}