#include "sl_string.h"
#include "string.h"
#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

void test_sl_from_cstr(void) {
    sl_err err;

    // Basic test
    sl_str s = sl_from_cstr("Hello", &err);
    TEST_ASSERT_NOT_NULL(s);
    TEST_ASSERT_EQUAL_STRING("Hello", s);
    TEST_ASSERT_EQUAL(SL_OK, err);
    TEST_ASSERT_EQUAL(5, sl_len(s, &err));
    TEST_ASSERT_EQUAL(6, sl_cap(s, &err));

    // Test free
    sl_free(&s, &err);
    TEST_ASSERT_NULL(s);

    // Test with embedded null bytes (truncated at first '\0')
    s = sl_from_cstr("Hel\0lo", &err);
    TEST_ASSERT_NOT_NULL(s);
    TEST_ASSERT_EQUAL_STRING("Hel", s);
    TEST_ASSERT_EQUAL(3, sl_len(s, &err));
    TEST_ASSERT_EQUAL(4, sl_cap(s, &err));
    sl_free(&s, &err);
    TEST_ASSERT_NULL(s);

    // Test input NULL
    s = sl_from_cstr(NULL, &err);
    TEST_ASSERT_EQUAL(SL_ERR_NULL, err);
    TEST_ASSERT_NULL(s);
}

void test_sl_append_cstr(void) {
    sl_err err;

    // Basic append
    sl_str s = sl_from_cstr("Hello", &err);
    TEST_ASSERT_NOT_NULL(s);

    s = sl_append_cstr(s, " world", &err);
    TEST_ASSERT_NOT_NULL(s);
    TEST_ASSERT_EQUAL_STRING("Hello world", s);
    TEST_ASSERT_EQUAL(11, sl_len(s, &err));
    TEST_ASSERT_TRUE(sl_cap(s, &err) >= 12); // capacity >= len+1
    TEST_ASSERT_EQUAL(SL_OK, err);

    // Append empty string
    s = sl_append_cstr(s, "", &err);
    TEST_ASSERT_NOT_NULL(s);
    TEST_ASSERT_EQUAL_STRING("Hello world", s);
    TEST_ASSERT_EQUAL(11, sl_len(s, &err));

    // Append multiple times
    s = sl_append_cstr(s, "!!!", &err);
    TEST_ASSERT_NOT_NULL(s);
    TEST_ASSERT_EQUAL_STRING("Hello world!!!", s);
    TEST_ASSERT_EQUAL(14, sl_len(s, &err));
    TEST_ASSERT_TRUE(sl_cap(s, &err) >= 15);

    // Append to NULL should return error
    sl_str null_str = NULL;
    null_str = sl_append_cstr(null_str, "abc", &err);
    TEST_ASSERT_NULL(null_str);
    TEST_ASSERT_EQUAL(SL_ERR_NULL, err);

    sl_free(&s, &err);
    TEST_ASSERT_NULL(s);
}

void test_use_after_free(void) {
    sl_err err;
    sl_str s = sl_from_cstr("Hello", &err);
    sl_free(&s, &err);

    TEST_ASSERT_NULL(s);

    size_t len = sl_len(s, &err);
    TEST_ASSERT_EQUAL(SIZE_MAX, len);
    TEST_ASSERT_EQUAL(SL_ERR_NULL, err);
}

void test_sl_eq(void) {
    sl_err err;
    sl_str a = sl_from_cstr("Hello", &err);
    sl_str b = sl_from_cstr("Hello", &err);
    sl_str c = sl_from_cstr("World", &err);

    TEST_ASSERT_TRUE(sl_eq(a, b, &err));
    TEST_ASSERT_EQUAL(SL_OK, err);

    TEST_ASSERT_FALSE(sl_eq(a, c, &err));
    TEST_ASSERT_EQUAL(SL_OK, err);

    // Same pointer
    TEST_ASSERT_TRUE(sl_eq(a, a, &err));

    // Invalid string
    sl_free(&b, &err);
    TEST_ASSERT_FALSE(sl_eq(a, b, &err));
    TEST_ASSERT_EQUAL(SL_ERR_NULL, err);

    sl_free(&a, &err);
    sl_free(&c, &err);
}

void test_hash(void) {
    // basics
    char *str = "hello";
    sl_err err;
    uint64_t hash_buf = sl_compute_hash(str, strlen(str));
    uint64_t hash_cstr = sl_compute_hash_cstr(str);

    TEST_ASSERT_EQUAL(hash_buf, hash_cstr);

    sl_str s = sl_from_cstr(str, &err);
    TEST_ASSERT_EQUAL(SL_OK, err);

    uint64_t s_hash = sl_hash(s, &err);
    TEST_ASSERT_EQUAL(SL_OK, err);
    TEST_ASSERT_EQUAL(hash_cstr, s_hash);

    // hash changes after append
    uint64_t old_hash = s_hash;
    s = sl_append_cstr(s, " world", &err);
    TEST_ASSERT_EQUAL(SL_OK, err);

    uint64_t new_hash = sl_hash(s, &err);
    TEST_ASSERT_EQUAL(SL_OK, err);
    TEST_ASSERT_NOT_EQUAL(old_hash, new_hash);

    sl_free(&s, &err);
    TEST_ASSERT_EQUAL(SL_OK, err);

    // empty string hash
    char *empty = "";
    uint64_t empty_buf = sl_compute_hash(empty, 0);
    uint64_t empty_cstr = sl_compute_hash_cstr(empty);

    TEST_ASSERT_EQUAL(empty_buf, empty_cstr);

    s = sl_from_cstr(empty, &err);
    TEST_ASSERT_EQUAL(SL_OK, err);
    TEST_ASSERT_EQUAL(empty_cstr, sl_hash(s, &err));
    sl_free(&s, NULL);

    // NULL
    uint64_t null_hash = sl_hash(NULL, &err);
    TEST_ASSERT_EQUAL(0, null_hash);
    TEST_ASSERT_EQUAL(SL_ERR_NULL, err);

    TEST_ASSERT_EQUAL(0, sl_compute_hash_cstr(NULL));
}

void test_sl_from_bytes(void) {
    sl_err err;

    // basic test with a normal string
    const char data[] = "Hello";
    sl_str s = sl_from_bytes(data, 5, &err);
    TEST_ASSERT_NOT_NULL(s);
    TEST_ASSERT_EQUAL(SL_OK, err);
    TEST_ASSERT_EQUAL(5, sl_len(s, &err));
    TEST_ASSERT_TRUE(memcmp(s, data, 5) == 0);
    TEST_ASSERT_EQUAL_CHAR('\0', s[5]);
    sl_free(&s, &err);
    TEST_ASSERT_NULL(s);

    // test with null bytes inside
    const unsigned char bytes[] = {'A', 0, 'B', 0, 67};
    s = sl_from_bytes(bytes, sizeof(bytes), &err);
    TEST_ASSERT_NOT_NULL(s);
    TEST_ASSERT_EQUAL(SL_OK, err);
    TEST_ASSERT_EQUAL(sizeof(bytes), sl_len(s, &err));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(bytes, s, sizeof(bytes));
    sl_free(&s, &err);
    TEST_ASSERT_NULL(s);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_sl_from_cstr);
    RUN_TEST(test_sl_append_cstr);
    RUN_TEST(test_use_after_free);
    RUN_TEST(test_sl_eq);
    RUN_TEST(test_hash);
    RUN_TEST(test_sl_from_bytes);

    return UNITY_END();
}