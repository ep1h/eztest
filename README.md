# eztest

eztest is a minimalistic testing framework designed for simplicity and ease of use, primarily catering to C99.

## Features

- Provides a set of macros to streamline test definition and execution.
- Allows easy comparison of values and buffers.
- Generates readable output for test results.
- Can access `private` and `protected` members of C++ classes for testing purposes.

## Usage

1. **Setup**: 
    Define the `EZTEST` preprocessor macro to any non-zero value.

2. **Writing Tests**:
    - Create a `.c` or `.cpp` file for writing tests.
    - Include the `eztest.h` header file.
    - Include the file(s) with the functionality you want to test.
    - Describe your test cases using the `TEST_BEGIN` and `TEST_END` syntax.
    - Finally, use the `RUN_TESTS` macro followed by the names of the test cases you wish to run.

3. **Sample Test**:
    ```c
    #include <eztest.h>
    #include "unit_under_test.hpp"

    TEST_BEGIN(sum_test)
        int result = unit_under_test_sum(2, 2);
        EXPECT(result, 4);
    TEST_END

    TEST_BEGIN(mul_test)
        int result = unit_under_test_mul(10, 0);
        EXPECT_ZERO(result);
    TEST_END

    RUN_TESTS(sum_test, mul_test);
    ```

4. **Note**:
    - Each test case described between `TEST_BEGIN` and `TEST_END` is its own function.
    - The namespaces for each test case do not overlap.
    - All local objects are automatically cleaned up post-test, but dynamically allocated objects need manual memory management.
    - Tests execute in the order they're specified in `RUN_TESTS`. However, well-designed tests should be independent of their execution order.
    - `RUN_TESTS` macro creates the program's entry point, so when building tests, ensure the original program's main function is excluded.

---

For more details and advanced usage, refer to the `eztest.h` file's documentation.
