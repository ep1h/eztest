/**-----------------------------------------------------------------------------
; @file eztest.h
;
; @language C99
;
; @brief
;   The file contains functionality for writing unit tests.
;
;   Usage:
;     Define EZTEST preprocessor macro to be any non-zero value.
;     Create c/cpp file for writing tests.
;     Include this header file in the created c/cpp file.
;     Include file(s) with the functionality to be tested in the c/cpp file.
;     Describe test cases.
;       Testcase description syntax:
;         - Test cases begin with the TEST_BEGIN statement, followed by the name
;           of the test case in parentheses.
;         - Test cases end with TEST_END statement.
;     Write RUN_TESTS followed by the names of the test cases to be executed
;     in parentheses.
;
;   Example:
;
;       #include <eztest.h>
;       #include "unit_under_test.hpp"
;
;       TEST_BEGIN(sum_test)
;           uintptr_t result = unit_under_test_sum(2, 2);
;           EXPECT(result, 4);
;       TEST_END
;
;       TEST_BEGIN(mul_test)
;           uintptr_t result = unit_under_test_mul(10, 0);
;           EXPECT_ZERO(result);
;       TEST_END
;
;       RUN_TESTS(sum_test, mul_test);
;
;   Each test case described between TEST_BEGIN and TEST_END is a separate
;   function with all the consequences. The namespaces of each test case do not
;   overlap. All local objects are automatically deleted after the test case
;   ends. For dynamically allocated objects, the release of memory falls on the
;   shoulders of the one who writes the test.
;
;   Tests executed in the order they are specified in arguments for RUN_TESTS.
;   Anyway, results of good test cases should not depend on the order in which
;   they are executed.
;
;   The RUN_TESTS macro creates an entry point to the program. To build tests,
;   original entry point must be out of scope (for example, placed inside the
;   #ifndef EZTEST construct).
;
; @author ep1h
;-----------------------------------------------------------------------------*/
// TODO: Support unicode.
// TODO: Implement output in file.
// TODO: Implement global initialization function.
// TODO: Support entry points for Windows applications.
// TODO: Expectation macros for strings(data buffers).
// TODO: Implement support for multiple RUN_TESTS(...) calls.

#ifndef EZTEST_H_
#define EZTEST_H_

#ifndef EZTEST
#error This file should not be included in non-test builds. Define the build as\
 a test build (define the "EZTEST" preprocessor const to a non-zero value), or\
 exclude this file from the build.
#endif

/* Provide access to private and protected members of classes */
#ifdef __cplusplus
#define private   public
#define protected public
#endif

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#define PRINTF_ PRINTF_A_
#define OUTPUT  WRITE_IN_CONSOLE_

#define PRINTF_A_ printf

#define WRITE_IN_CONSOLE_(format, ...)                                         \
    do                                                                         \
    {                                                                          \
        PRINTF_(format, ##__VA_ARGS__);                                        \
    } while (0)

#define TEST_PASS_ 0
#define TEST_FAIL_ 1

#define TOTAL_EXPECTEDS_  total_expecteds
#define FAILED_EXPECTEDS_ failed_expecteds
#define TEST_RESULT_      result

typedef struct TestCaseInfo
{
    uintptr_t TOTAL_EXPECTEDS_;
    uintptr_t FAILED_EXPECTEDS_;
    uintptr_t TEST_RESULT_;
} TestCaseInfo;

#define TEST_BEGIN(TEST_NAME)                                                  \
    static void TEST_NAME(TestCaseInfo* ti)                                    \
    {                                                                          \
        uintptr_t TOTAL_EXPECTEDS_ = 0;                                        \
        uintptr_t FAILED_EXPECTEDS_ = 0;                                       \
        uintptr_t TEST_RESULT_ = 0;                                            \
        void* eztest_value = 0;                                                \
        void* eztest_expected = 0;                                             \
        OUTPUT("Executing test \'%s\'...\n", #TEST_NAME);

#define TEST_FOOTER_                                                           \
    do                                                                         \
    {                                                                          \
        ti->TOTAL_EXPECTEDS_ = TOTAL_EXPECTEDS_;                               \
        ti->FAILED_EXPECTEDS_ = FAILED_EXPECTEDS_;                             \
        ti->TEST_RESULT_ = TEST_RESULT_;                                       \
    } while (0);

#define TEST_END                                                               \
    TEST_FOOTER_;                                                              \
    return;                                                                    \
    }

#define EXPECT(value, expected)                                                \
    do                                                                         \
    {                                                                          \
        ++TOTAL_EXPECTEDS_;                                                    \
        eztest_value = (void*)(value);                                         \
        eztest_expected = (void*)(expected);                                   \
        if (eztest_value != eztest_expected)                                   \
        {                                                                      \
            OUTPUT("Failed expecation. Line: %d. actual: %" PRIuPTR            \
                   "(0x%" PRIxPTR ") expected: %" PRIuPTR "(0x%" PRIxPTR       \
                   ")\n",                                                      \
                   __LINE__, (uintptr_t)eztest_value, (uintptr_t)eztest_value, \
                   (uintptr_t)eztest_expected, (uintptr_t)eztest_expected);    \
            ++FAILED_EXPECTEDS_;                                               \
            TEST_RESULT_ = TEST_FAIL_;                                         \
        }                                                                      \
    } while (0)

#define EXPECT_ZERO(value) EXPECT((value == 0), 1)

#define EXPECT_NOT_ZERO(value) EXPECT((value == 0), 0)

#define FORCE_FAIL_TEST                                                        \
    TEST_RESULT_ = TEST_FAIL_;                                                 \
    TEST_FOOTER_;                                                              \
    return;

#define EXPECT_BUF(value, expected, size)                                      \
    do                                                                         \
    {                                                                          \
        ++TOTAL_EXPECTEDS_;                                                    \
        for (size_t i = 0; i < size; i++)                                      \
        {                                                                      \
            if (((char*)value)[i] != ((char*)expected)[i])                     \
            {                                                                  \
                ++FAILED_EXPECTEDS_;                                           \
                TEST_RESULT_ = TEST_FAIL_;                                     \
                break;                                                         \
            }                                                                  \
        }                                                                      \
    } while (0)

#define TESTS_SEPARATOR_                                                       \
    "------------------------------------------------------------------------" \
    "--------\n"

#define RUN_TESTS(TESTS_GROUP_NAME, ...)                                       \
    uintptr_t TESTS_GROUP_NAME(void)                                           \
    {                                                                          \
        uintptr_t failed_testcases = 0;                                        \
        void (*tests_array[])(TestCaseInfo*) = {__VA_ARGS__};                  \
        TestCaseInfo ti = {                                                    \
            .TOTAL_EXPECTEDS_ = 0, .FAILED_EXPECTEDS_ = 0, .TEST_RESULT_ = 0}; \
        for (uintptr_t i = 0; i < sizeof(tests_array) / sizeof(void*); i++)    \
        {                                                                      \
            tests_array[i](&ti);                                               \
            if (ti.TEST_RESULT_ == TEST_PASS_)                                 \
            {                                                                  \
                OUTPUT("PASSED ");                                             \
            }                                                                  \
            else                                                               \
            {                                                                  \
                OUTPUT("FAILED ");                                             \
                ++failed_testcases;                                            \
            }                                                                  \
            OUTPUT("(%" PRIuPTR "/%" PRIuPTR ")\n",                            \
                   ti.TOTAL_EXPECTEDS_ - ti.FAILED_EXPECTEDS_,                 \
                   ti.TOTAL_EXPECTEDS_);                                       \
            OUTPUT(TESTS_SEPARATOR_);                                          \
        }                                                                      \
        OUTPUT("Executed tests: %" PRIuPTR " (%" PRIuPTR " passed, %" PRIuPTR  \
               " failed).\n",                                                  \
               sizeof(tests_array) / sizeof(void*),                            \
               sizeof(tests_array) / sizeof(void*) - failed_testcases,         \
               failed_testcases);                                              \
        return failed_testcases;                                               \
    }

#endif /* EZTEST_H_ */
