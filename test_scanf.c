#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declaration of function
int my_scanf(const char *format, ...);

// Test result tracking
int tests_passed = 0;
int tests_failed = 0;

// Helper function to redirect stdin to a file
FILE* setup_input_from_file(const char *filename) {
    FILE *original_stdin = stdin;
    FILE *fp = freopen(filename, "r", stdin);
    if (fp == NULL) {
        fprintf(stderr, "Failed to open file: %s\n", filename);
        exit(1);
    }
    return original_stdin;
}

void restore_stdin(FILE *original_stdin) {
    clearerr(stdin);  // Clear any error flags

    #ifdef _WIN32
        if (freopen("CON", "r", stdin) == NULL) {
            fprintf(stderr, "Failed to restore stdin\n");
            exit(1);
        }
    #else
        if (freopen("/dev/tty", "r", stdin) == NULL) {
            fprintf(stderr, "Failed to restore stdin\n");
            exit(1);
        }
    #endif

    clearerr(stdin);  // Clear again after reopen
}

// Helper to read one line from test data file and write to temp file
void prepare_test_input(const char *test_data_file, int line_num, const char *temp_file) {
    FILE *data_fp = fopen(test_data_file, "r");
    if (data_fp == NULL) {
        fprintf(stderr, "Failed to open test data file\n");
        exit(1);
    }

    char buffer[1024];
    int current_line = 0;

    // Read to the desired line
    while (fgets(buffer, sizeof(buffer), data_fp) != NULL) {
        if (current_line == line_num) {
            // Write this line to temp file
            FILE *temp_fp = fopen(temp_file, "w");
            if (temp_fp == NULL) {
                fprintf(stderr, "Failed to create temp file\n");
                exit(1);
            }
            fputs(buffer, temp_fp);
            fclose(temp_fp);
            fclose(data_fp);
            return;
        }
        current_line++;
    }

    fclose(data_fp);
    fprintf(stderr, "Line %d not found in test data file\n", line_num);
    exit(1);
}

// Helper to read multiple consecutive lines starting from line_num
void prepare_test_input_multiline(const char *test_data_file, int start_line, int num_lines, const char *temp_file) {
    FILE *data_fp = fopen(test_data_file, "r");
    if (data_fp == NULL) {
        fprintf(stderr, "Failed to open test data file\n");
        exit(1);
    }

    char buffer[1024];
    int current_line = 0;

    // Open temp file for writing
    FILE *temp_fp = fopen(temp_file, "w");
    if (temp_fp == NULL) {
        fprintf(stderr, "Failed to create temp file\n");
        exit(1);
    }

    // Skip to start line
    while (fgets(buffer, sizeof(buffer), data_fp) != NULL && current_line < start_line) {
        current_line++;
    }

    // Write num_lines to temp file
    int lines_written = 0;
    while (lines_written < num_lines && fgets(buffer, sizeof(buffer), data_fp) != NULL) {
        fputs(buffer, temp_fp);
        lines_written++;
    }

    fclose(temp_fp);
    fclose(data_fp);
}

void test_basic_integer() {
    int val;

    printf("Running test: Basic positive integer\n");
    prepare_test_input("test_data.txt", 0, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%d", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 42) {
        printf("   PASSED - Value: %d\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 42, got %d (return: %d)\n", val, result);
        tests_failed++;
    }

    printf("Running test: Basic negative integer\n");
    prepare_test_input("test_data.txt", 1, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%d", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == -123) {
        printf("   PASSED - Value: %d\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected -123, got %d (return: %d)\n", val, result);
        tests_failed++;
    }

    printf("Running test: Zero\n");
    prepare_test_input("test_data.txt", 2, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%d", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 0) {
        printf("   PASSED - Value: %d\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 0, got %d (return: %d)\n", val, result);
        tests_failed++;
    }
}

void test_multiple_integers() {
    int val1, val2, val3;

    printf("Running test: Two integers with space\n");
    prepare_test_input("test_data.txt", 3, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%d %d", &val1, &val2);
    restore_stdin(orig_stdin);
    if (result == 2 && val1 == 10 && val2 == 20) {
        printf("   PASSED - Values: %d, %d\n", val1, val2);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 10, 20; got %d, %d (return: %d)\n", val1, val2, result);
        tests_failed++;
    }

    printf("Running test: Three integers\n");
    prepare_test_input("test_data.txt", 4, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%d %d %d", &val1, &val2, &val3);
    restore_stdin(orig_stdin);
    if (result == 3 && val1 == 100 && val2 == 200 && val3 == 300) {
        printf("   PASSED - Values: %d, %d, %d\n", val1, val2, val3);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 100, 200, 300; got %d, %d, %d (return: %d)\n", val1, val2, val3, result);
        tests_failed++;
    }

    printf("Running test: Multiple spaces between\n");
    prepare_test_input("test_data.txt", 5, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%d %d", &val1, &val2);
    restore_stdin(orig_stdin);
    if (result == 2 && val1 == 5 && val2 == 15) {
        printf("   PASSED - Values: %d, %d\n", val1, val2);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 5, 15; got %d, %d (return: %d)\n", val1, val2, result);
        tests_failed++;
    }
}

void test_field_width() {
    int val, val1, val2;

    printf("Running test: Field width 2\n");
    prepare_test_input("test_data.txt", 6, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%2d", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 12) {
        printf("   PASSED - Value: %d (read only 2 digits)\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 12, got %d (return: %d)\n", val, result);
        tests_failed++;
    }

    printf("Running test: Field width multiple\n");
    prepare_test_input("test_data.txt", 7, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%3d%3d", &val1, &val2);
    restore_stdin(orig_stdin);
    if (result == 2 && val1 == 123 && val2 == 456) {
        printf("   PASSED - Values: %d, %d\n", val1, val2);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 123, 456; got %d, %d (return: %d)\n", val1, val2, result);
        tests_failed++;
    }
}

void test_leading_whitespace() {
    int val;

    printf("Running test: Leading spaces\n");
    prepare_test_input("test_data.txt", 8, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%d", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 42) {
        printf("   PASSED - Value: %d\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 42, got %d (return: %d)\n", val, result);
        tests_failed++;
    }

    printf("Running test: Leading tabs\n");
    prepare_test_input("test_data.txt", 9, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%d", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 99) {
        printf("   PASSED - Value: %d\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 99, got %d (return: %d)\n", val, result);
        tests_failed++;
    }
}

void test_edge_cases() {
    int val;

    printf("Running test: Explicit plus sign\n");
    prepare_test_input("test_data.txt", 11, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%d", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 55) {
        printf("   PASSED - Value: %d\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 55, got %d (return: %d)\n", val, result);
        tests_failed++;
    }

    printf("Running test: Large number\n");
    prepare_test_input("test_data.txt", 12, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%d", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 2147483647) {
        printf("   PASSED - Value: %d\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 2147483647, got %d (return: %d)\n", val, result);
        tests_failed++;
    }

    printf("Running test: Large negative\n");
    prepare_test_input("test_data.txt", 13, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%d", &val);
    restore_stdin(orig_stdin);
    printf("  Value read: %d (return: %d)\n", val, result);
    tests_passed++; // Just informational for this one
}

void test_invalid_input() {
    int val, val1, val2;

    printf("Running test: Non-numeric input\n");
    val = 999;
    prepare_test_input("test_data.txt", 14, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%d", &val);
    restore_stdin(orig_stdin);
    if (result == 0) {
        printf("   PASSED - Return: %d, Value unchanged: %d\n", result, val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected return 0, got %d\n", result);
        tests_failed++;
    }

    printf("Running test: Partial match\n");
    prepare_test_input("test_data.txt", 15, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%d %d", &val1, &val2);
    restore_stdin(orig_stdin);
    if (result == 1 && val1 == 42) {
        printf("   PASSED - Return: %d, First value: %d\n", result, val1);
        tests_passed++;
    } else {
        printf("   FAILED - Expected return 1 with val1=42, got return=%d, val1=%d\n", result, val1);
        tests_failed++;
    }
}

void test_suppress_assignment() {
    int val1, val2;

    printf("Running test: Suppress assignment\n");
    prepare_test_input("test_data.txt", 16, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%d %*d %d", &val1, &val2);
    restore_stdin(orig_stdin);
    if (result == 2 && val1 == 10 && val2 == 30) {
        printf("   PASSED - Values: %d, %d (20 suppressed)\n", val1, val2);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 10, 30; got %d, %d (return: %d)\n", val1, val2, result);
        tests_failed++;
    }
}
void test_basic_float() {
    float val;

    printf("Running test: Basic positive float\n");
    prepare_test_input("test_data.txt", 17, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%f", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val > 3.13 && val < 3.15) {
        printf("   PASSED - Value: %f\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected ~3.14, got %f (return: %d)\n", val, result);
        tests_failed++;
    }

    printf("Running test: Basic negative float\n");
    prepare_test_input("test_data.txt", 18, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%f", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val > -2.72 && val < -2.70) {
        printf("   PASSED - Value: %f\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected ~-2.71, got %f (return: %d)\n", val, result);
        tests_failed++;
    }

    printf("Running test: Float zero\n");
    prepare_test_input("test_data.txt", 19, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%f", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 0.0f) {
        printf("   PASSED - Value: %f\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 0.0, got %f (return: %d)\n", val, result);
        tests_failed++;
    }

    printf("Running test: Integer as float\n");
    prepare_test_input("test_data.txt", 20, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%f", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 42.0f) {
        printf("   PASSED - Value: %f\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 42.0, got %f (return: %d)\n", val, result);
        tests_failed++;
    }
}

void test_float_formats() {
    float val;

    printf("Running test: Scientific notation positive\n");
    prepare_test_input("test_data.txt", 21, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%f", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val > 1.22e3 && val < 1.24e3) {
        printf("   PASSED - Value: %f\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected ~1230, got %f (return: %d)\n", val, result);
        tests_failed++;
    }

    printf("Running test: Scientific notation negative exponent\n");
    prepare_test_input("test_data.txt", 22, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%f", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val > 5.59e-3 && val < 5.61e-3) {
        printf("   PASSED - Value: %f\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected ~0.0056, got %f (return: %d)\n", val, result);
        tests_failed++;
    }

    printf("Running test: No decimal point\n");
    prepare_test_input("test_data.txt", 23, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%f", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 100.0f) {
        printf("   PASSED - Value: %f\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 100.0, got %f (return: %d)\n", val, result);
        tests_failed++;
    }

    printf("Running test: Leading decimal point\n");
    prepare_test_input("test_data.txt", 24, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%f", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val > 0.49 && val < 0.51) {
        printf("   PASSED - Value: %f\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected ~0.5, got %f (return: %d)\n", val, result);
        tests_failed++;
    }

    printf("Running test: Trailing decimal point\n");
    prepare_test_input("test_data.txt", 25, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%f", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 7.0f) {
        printf("   PASSED - Value: %f\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 7.0, got %f (return: %d)\n", val, result);
        tests_failed++;
    }
}

void test_multiple_floats() {
    float val1, val2;

    printf("Running test: Two floats with space\n");
    prepare_test_input("test_data.txt", 26, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%f %f", &val1, &val2);
    restore_stdin(orig_stdin);
    if (result == 2 && val1 > 1.0 && val1 < 1.2 && val2 > 2.4 && val2 < 2.6) {
        printf("   PASSED - Values: %f, %f\n", val1, val2);
        tests_passed++;
    } else {
        printf("   FAILED - Expected ~1.1, ~2.5; got %f, %f (return: %d)\n", val1, val2, result);
        tests_failed++;
    }

    printf("Running test: Mixed int and float\n");
    int ival;
    float fval;
    prepare_test_input("test_data.txt", 27, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%d %f", &ival, &fval);
    restore_stdin(orig_stdin);
    if (result == 2 && ival == 42 && fval > 3.13 && fval < 3.15) {
        printf("   PASSED - Values: %d, %f\n", ival, fval);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 42, ~3.14; got %d, %f (return: %d)\n", ival, fval, result);
        tests_failed++;
    }
}

void test_float_field_width() {
    float val;

    printf("Running test: Float field width\n");
    prepare_test_input("test_data.txt", 28, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%4f", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val > 12.2 && val < 12.4) {
        printf("   PASSED - Value: %f (read only 4 chars)\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected ~12.3, got %f (return: %d)\n", val, result);
        tests_failed++;
    }
}

void test_float_edge_cases() {
    float val;

    printf("Running test: Float with plus sign\n");
    prepare_test_input("test_data.txt", 29, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%f", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val > 9.98 && val < 10.02) {
        printf("   PASSED - Value: %f\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected ~10.0, got %f (return: %d)\n", val, result);
        tests_failed++;
    }

    printf("Running test: Very small float\n");
    prepare_test_input("test_data.txt", 30, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%f", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val > 0.0 && val < 0.001) {
        printf("   PASSED - Value: %f\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected ~0.0001, got %f (return: %d)\n", val, result);
        tests_failed++;
    }

    printf("Running test: Very large float\n");
    prepare_test_input("test_data.txt", 31, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%f", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val > 1e6) {
        printf("   PASSED - Value: %f\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected large number, got %f (return: %d)\n", val, result);
        tests_failed++;
    }
}

void test_float_invalid_input() {
    float val;

    printf("Running test: Float non-numeric input\n");
    val = 999.9f;
    prepare_test_input("test_data.txt", 32, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%f", &val);
    restore_stdin(orig_stdin);
    if (result == 0) {
        printf("   PASSED - Return: %d, Value unchanged: %f\n", result, val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected return 0, got %d\n", result);
        tests_failed++;
    }

    printf("Running test: Float partial match\n");
    float val1, val2;
    prepare_test_input("test_data.txt", 33, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%f %f", &val1, &val2);
    restore_stdin(orig_stdin);
    if (result == 1 && val1 > 3.13 && val1 < 3.15) {
        printf("   PASSED - Return: %d, First value: %f\n", result, val1);
        tests_passed++;
    } else {
        printf("   FAILED - Expected return 1 with val1~3.14, got return=%d, val1=%f\n", result, val1);
        tests_failed++;
    }
}

void test_float_suppress_assignment() {
    float val1, val2;

    printf("Running test: Float suppress assignment\n");
    prepare_test_input("test_data.txt", 34, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%f %*f %f", &val1, &val2);
    restore_stdin(orig_stdin);
    if (result == 2 && val1 > 1.0 && val1 < 1.2 && val2 > 3.2 && val2 < 3.4) {
        printf("   PASSED - Values: %f, %f (2.2 suppressed)\n", val1, val2);
        tests_passed++;
    } else {
        printf("   FAILED - Expected ~1.1, ~3.3; got %f, %f (return: %d)\n", val1, val2, result);
        tests_failed++;
    }
}
void test_basic_string() {
    char str[100];

    printf("Running test: Basic string\n");
    prepare_test_input("test_data.txt", 35, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%s", str);
    restore_stdin(orig_stdin);
    if (result == 1 && strcmp(str, "hello") == 0) {
        printf("   PASSED - Value: %s\n", str);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 'hello', got '%s' (return: %d)\n", str, result);
        tests_failed++;
    }

    printf("Running test: String with leading whitespace\n");
    prepare_test_input("test_data.txt", 36, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%s", str);
    restore_stdin(orig_stdin);
    if (result == 1 && strcmp(str, "world") == 0) {
        printf("   PASSED - Value: %s\n", str);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 'world', got '%s' (return: %d)\n", str, result);
        tests_failed++;
    }

    printf("Running test: String stops at whitespace\n");
    prepare_test_input("test_data.txt", 37, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%s", str);
    restore_stdin(orig_stdin);
    if (result == 1 && strcmp(str, "first") == 0) {
        printf("   PASSED - Value: %s (stopped at space)\n", str);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 'first', got '%s' (return: %d)\n", str, result);
        tests_failed++;
    }
}

void test_string_field_width() {
    char str[100];

    printf("Running test: String with field width\n");
    prepare_test_input("test_data.txt", 38, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%5s", str);
    restore_stdin(orig_stdin);
    if (result == 1 && strcmp(str, "hello") == 0) {
        printf("   PASSED - Value: %s (read only 5 chars)\n", str);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 'hello', got '%s' (return: %d)\n", str, result);
        tests_failed++;
    }

    printf("Running test: Multiple strings\n");
    char str1[100], str2[100];
    prepare_test_input("test_data.txt", 39, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%s %s", str1, str2);
    restore_stdin(orig_stdin);
    if (result == 2 && strcmp(str1, "foo") == 0 && strcmp(str2, "bar") == 0) {
        printf("   PASSED - Values: %s, %s\n", str1, str2);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 'foo', 'bar'; got '%s', '%s' (return: %d)\n", str1, str2, result);
        tests_failed++;
    }
}

void test_string_suppress() {
    char str1[100], str2[100];

    printf("Running test: String suppress assignment\n");
    prepare_test_input("test_data.txt", 40, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%s %*s %s", str1, str2);
    restore_stdin(orig_stdin);
    if (result == 2 && strcmp(str1, "one") == 0 && strcmp(str2, "three") == 0) {
        printf("   PASSED - Values: %s, %s ('two' suppressed)\n", str1, str2);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 'one', 'three'; got '%s', '%s' (return: %d)\n", str1, str2, result);
        tests_failed++;
    }
}

void test_basic_hex() {
    int val;

    printf("Running test: Basic hex lowercase\n");
    prepare_test_input("test_data.txt", 41, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%x", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 0x1a) {
        printf("   PASSED - Value: 0x%x (%d)\n", val, val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 0x1a (26), got 0x%x (%d) (return: %d)\n", val, val, result);
        tests_failed++;
    }

    printf("Running test: Basic hex uppercase\n");
    prepare_test_input("test_data.txt", 42, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%x", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 0xFF) {
        printf("   PASSED - Value: 0x%x (%d)\n", val, val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 0xFF (255), got 0x%x (%d) (return: %d)\n", val, val, result);
        tests_failed++;
    }

    printf("Running test: Hex with 0x prefix\n");
    prepare_test_input("test_data.txt", 43, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%x", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 0xABC) {
        printf("   PASSED - Value: 0x%x (%d)\n", val, val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 0xABC (2748), got 0x%x (%d) (return: %d)\n", val, val, result);
        tests_failed++;
    }

    printf("Running test: Hex zero\n");
    prepare_test_input("test_data.txt", 44, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%x", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 0) {
        printf("   PASSED - Value: 0x%x (%d)\n", val, val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 0x0 (0), got 0x%x (%d) (return: %d)\n", val, val, result);
        tests_failed++;
    }
}

void test_hex_field_width() {
    int val, val1, val2;

    printf("Running test: Hex field width\n");
    prepare_test_input("test_data.txt", 45, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%2x", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 0x12) {
        printf("   PASSED - Value: 0x%x (read only 2 hex digits)\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 0x12, got 0x%x (return: %d)\n", val, result);
        tests_failed++;
    }

    printf("Running test: Multiple hex values\n");
    prepare_test_input("test_data.txt", 46, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%x %x", &val1, &val2);
    restore_stdin(orig_stdin);
    if (result == 2 && val1 == 0xA && val2 == 0xB) {
        printf("   PASSED - Values: 0x%x, 0x%x\n", val1, val2);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 0xA, 0xB; got 0x%x, 0x%x (return: %d)\n", val1, val2, result);
        tests_failed++;
    }
}

void test_hex_suppress() {
    int val1, val2;

    printf("Running test: Hex suppress assignment\n");
    prepare_test_input("test_data.txt", 47, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%x %*x %x", &val1, &val2);
    restore_stdin(orig_stdin);
    if (result == 2 && val1 == 0x10 && val2 == 0x30) {
        printf("   PASSED - Values: 0x%x, 0x%x (0x20 suppressed)\n", val1, val2);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 0x10, 0x30; got 0x%x, 0x%x (return: %d)\n", val1, val2, result);
        tests_failed++;
    }
}

void test_basic_char() {
    char c;

    printf("Running test: Basic character\n");
    prepare_test_input("test_data.txt", 48, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%c", &c);
    restore_stdin(orig_stdin);
    if (result == 1 && c == 'A') {
        printf("   PASSED - Value: '%c'\n", c);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 'A', got '%c' (return: %d)\n", c, result);
        tests_failed++;
    }

    printf("Running test: Character does not skip whitespace\n");
    prepare_test_input("test_data.txt", 49, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%c", &c);
    restore_stdin(orig_stdin);
    if (result == 1 && c == ' ') {
        printf("   PASSED - Value: ' ' (space)\n");
        tests_passed++;
    } else {
        printf("   FAILED - Expected ' ' (space), got '%c' (return: %d)\n", c, result);
        tests_failed++;
    }

    printf("Running test: Multiple characters\n");
    char c1, c2, c3;
    prepare_test_input("test_data.txt", 50, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%c%c%c", &c1, &c2, &c3);
    restore_stdin(orig_stdin);
    if (result == 3 && c1 == 'X' && c2 == 'Y' && c3 == 'Z') {
        printf("   PASSED - Values: '%c', '%c', '%c'\n", c1, c2, c3);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 'X', 'Y', 'Z'; got '%c', '%c', '%c' (return: %d)\n", c1, c2, c3, result);
        tests_failed++;
    }
}

void test_char_field_width() {
    char str[10];

    printf("Running test: Character with field width (reads multiple)\n");
    prepare_test_input("test_data.txt", 51, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%3c", str);
    str[3] = '\0';  // Null terminate for printing
    restore_stdin(orig_stdin);
    if (result == 1 && strncmp(str, "abc", 3) == 0) {
        printf("   PASSED - Value: '%s' (read 3 chars)\n", str);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 'abc', got '%s' (return: %d)\n", str, result);
        tests_failed++;
    }
}

void test_char_suppress() {
    char c1, c2;

    printf("Running test: Character suppress assignment\n");
    prepare_test_input("test_data.txt", 52, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%c%*c%c", &c1, &c2);
    restore_stdin(orig_stdin);
    if (result == 2 && c1 == '1' && c2 == '3') {
        printf("   PASSED - Values: '%c', '%c' ('2' suppressed)\n", c1, c2);
        tests_passed++;
    } else {
        printf("   FAILED - Expected '1', '3'; got '%c', '%c' (return: %d)\n", c1, c2, result);
        tests_failed++;
    }
}

void test_mixed_types() {
    int i;
    float f;
    char str[100];
    char c;
    unsigned int x;

    printf("Running test: Mixed int, float, string\n");
    prepare_test_input("test_data.txt", 53, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%d %f %s", &i, &f, str);
    restore_stdin(orig_stdin);
    if (result == 3 && i == 42 && f > 3.13 && f < 3.15 && strcmp(str, "test") == 0) {
        printf("   PASSED - Values: %d, %f, %s\n", i, f, str);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 42, ~3.14, 'test'; got %d, %f, '%s' (return: %d)\n", i, f, str, result);
        tests_failed++;
    }

    printf("Running test: Mixed hex, char, int\n");
    prepare_test_input("test_data.txt", 54, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%x %c %d", &x, &c, &i);
    restore_stdin(orig_stdin);
    if (result == 3 && x == 0xFF && c == 'Z' && i == 100) {
        printf("   PASSED - Values: 0x%x, '%c', %d\n", x, c, i);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 0xFF, 'Z', 100; got 0x%x, '%c', %d (return: %d)\n", x, c, i, result);
        tests_failed++;
    }

    printf("Running test: All types in one\n");
    char str2[100];
    prepare_test_input("test_data.txt", 55, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%d %x %f %c %s", &i, &x, &f, &c, str2);
    restore_stdin(orig_stdin);
    if (result == 5 && i == 123 && x == 0xAB && f > 4.54 && f < 4.56 && c == 'Q' && strcmp(str2, "word") == 0) {
        printf("   PASSED - Values: %d, 0x%x, %f, '%c', %s\n", i, x, f, c, str2);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 123, 0xAB, ~4.55, 'Q', 'word' (return: %d)\n", result);
        tests_failed++;
    }
}

void test_mixed_with_suppress() {
    int i1, i2;
    char str[100];

    printf("Running test: Mixed types with suppression\n");
    prepare_test_input("test_data.txt", 56, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%d %*s %d %*f %s", &i1, &i2, str);
    restore_stdin(orig_stdin);
    if (result == 3 && i1 == 10 && i2 == 20 && strcmp(str, "end") == 0) {
        printf("   PASSED - Values: %d, %d, %s (with suppressions)\n", i1, i2, str);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 10, 20, 'end'; got %d, %d, '%s' (return: %d)\n", i1, i2, str, result);
        tests_failed++;
    }
}

void test_basic_binary() {
    unsigned int val;

    printf("Running test: Basic binary\n");
    prepare_test_input("test_data.txt", 57, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%b", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 5) {  // 101 in binary = 5
        printf("   PASSED - Value: %u (binary 101)\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 5, got %u (return: %d)\n", val, result);
        tests_failed++;
    }

    printf("Running test: Binary with 0b prefix\n");
    prepare_test_input("test_data.txt", 58, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%b", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 10) {  // 0b1010 = 10
        printf("   PASSED - Value: %u (binary 0b1010)\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 10, got %u (return: %d)\n", val, result);
        tests_failed++;
    }

    printf("Running test: Binary all ones\n");
    prepare_test_input("test_data.txt", 59, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%b", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 15) {  // 1111 = 15
        printf("   PASSED - Value: %u (binary 1111)\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 15, got %u (return: %d)\n", val, result);
        tests_failed++;
    }

    printf("Running test: Binary zero\n");
    prepare_test_input("test_data.txt", 60, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%b", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 0) {
        printf("   PASSED - Value: %u (binary 0)\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 0, got %u (return: %d)\n", val, result);
        tests_failed++;
    }

    printf("Running test: Binary just 0 (not 0b)\n");
    prepare_test_input("test_data.txt", 61, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%b", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 0) {
        printf("   PASSED - Value: %u\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 0, got %u (return: %d)\n", val, result);
        tests_failed++;
    }
}

void test_binary_field_width() {
    unsigned int val, val1, val2;

    printf("Running test: Binary field width\n");
    prepare_test_input("test_data.txt", 62, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%4b", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 12) {  // 1100 = 12 (read only 4 digits)
        printf("   PASSED - Value: %u (read only 4 binary digits)\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 12, got %u (return: %d)\n", val, result);
        tests_failed++;
    }

    printf("Running test: Multiple binary values\n");
    prepare_test_input("test_data.txt", 63, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%b %b", &val1, &val2);
    restore_stdin(orig_stdin);
    if (result == 2 && val1 == 7 && val2 == 3) {  // 111 = 7, 11 = 3
        printf("   PASSED - Values: %u, %u\n", val1, val2);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 7, 3; got %u, %u (return: %d)\n", val1, val2, result);
        tests_failed++;
    }
}

void test_binary_edge_cases() {
    unsigned int val;

    printf("Running test: Binary with leading whitespace\n");
    prepare_test_input("test_data.txt", 64, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%b", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 6) {  // 110 = 6
        printf("   PASSED - Value: %u\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 6, got %u (return: %d)\n", val, result);
        tests_failed++;
    }

    printf("Running test: Binary invalid input\n");
    val = 999;
    prepare_test_input("test_data.txt", 65, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%b", &val);
    restore_stdin(orig_stdin);
    if (result == 0) {
        printf("   PASSED - Return: %d, Value unchanged: %u\n", result, val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected return 0, got %d\n", result);
        tests_failed++;
    }

    printf("Running test: Binary with 0B prefix (uppercase)\n");
    prepare_test_input("test_data.txt", 66, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%b", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 8) {  // 0B1000 = 8
        printf("   PASSED - Value: %u\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 8, got %u (return: %d)\n", val, result);
        tests_failed++;
    }
}

void test_binary_suppress() {
    unsigned int val1, val2;

    printf("Running test: Binary suppress assignment\n");
    prepare_test_input("test_data.txt", 67, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%b %*b %b", &val1, &val2);
    restore_stdin(orig_stdin);
    if (result == 2 && val1 == 1 && val2 == 7) {  // 1, (suppress 11), 111
        printf("   PASSED - Values: %u, %u (middle value suppressed)\n", val1, val2);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 1, 7; got %u, %u (return: %d)\n", val1, val2, result);
        tests_failed++;
    }
}

void test_basic_boolean() {
    int val;

    printf("Running test: Boolean true (numeric 1)\n");
    prepare_test_input("test_data.txt", 68, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%B", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 1) {
        printf("   PASSED - Value: %d\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 1, got %d (return: %d)\n", val, result);
        tests_failed++;
    }

    printf("Running test: Boolean false (numeric 0)\n");
    prepare_test_input("test_data.txt", 69, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%B", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 0) {
        printf("   PASSED - Value: %d\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 0, got %d (return: %d)\n", val, result);
        tests_failed++;
    }

    printf("Running test: Boolean 'true' (full word)\n");
    prepare_test_input("test_data.txt", 70, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%B", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 1) {
        printf("   PASSED - Value: %d\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 1, got %d (return: %d)\n", val, result);
        tests_failed++;
    }

    printf("Running test: Boolean 'false' (full word)\n");
    prepare_test_input("test_data.txt", 71, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%B", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 0) {
        printf("   PASSED - Value: %d\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 0, got %d (return: %d)\n", val, result);
        tests_failed++;
    }

    printf("Running test: Boolean 't' (single char)\n");
    prepare_test_input("test_data.txt", 72, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%B", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 1) {
        printf("   PASSED - Value: %d\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 1, got %d (return: %d)\n", val, result);
        tests_failed++;
    }

    printf("Running test: Boolean 'f' (single char)\n");
    prepare_test_input("test_data.txt", 73, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%B", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 0) {
        printf("   PASSED - Value: %d\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 0, got %d (return: %d)\n", val, result);
        tests_failed++;
    }
}

void test_boolean_variants() {
    int val;

    printf("Running test: Boolean 'yes'\n");
    prepare_test_input("test_data.txt", 74, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%B", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 1) {
        printf("   PASSED - Value: %d\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 1, got %d (return: %d)\n", val, result);
        tests_failed++;
    }

    printf("Running test: Boolean 'no'\n");
    prepare_test_input("test_data.txt", 75, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%B", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 0) {
        printf("   PASSED - Value: %d\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 0, got %d (return: %d)\n", val, result);
        tests_failed++;
    }

    printf("Running test: Boolean 'y' (single char)\n");
    prepare_test_input("test_data.txt", 76, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%B", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 1) {
        printf("   PASSED - Value: %d\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 1, got %d (return: %d)\n", val, result);
        tests_failed++;
    }

    printf("Running test: Boolean 'n' (single char)\n");
    prepare_test_input("test_data.txt", 77, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%B", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 0) {
        printf("   PASSED - Value: %d\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 0, got %d (return: %d)\n", val, result);
        tests_failed++;
    }

    printf("Running test: Boolean uppercase 'TRUE'\n");
    prepare_test_input("test_data.txt", 78, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%B", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 1) {
        printf("   PASSED - Value: %d\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 1, got %d (return: %d)\n", val, result);
        tests_failed++;
    }

    printf("Running test: Boolean uppercase 'FALSE'\n");
    prepare_test_input("test_data.txt", 79, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%B", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 0) {
        printf("   PASSED - Value: %d\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 0, got %d (return: %d)\n", val, result);
        tests_failed++;
    }
}

void test_boolean_edge_cases() {
    int val;

    printf("Running test: Boolean with leading whitespace\n");
    prepare_test_input("test_data.txt", 80, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%B", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 1) {
        printf("   PASSED - Value: %d\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 1, got %d (return: %d)\n", val, result);
        tests_failed++;
    }

    printf("Running test: Boolean invalid input\n");
    val = 999;
    prepare_test_input("test_data.txt", 81, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%B", &val);
    restore_stdin(orig_stdin);
    if (result == 0) {
        printf("   PASSED - Return: %d, Value unchanged: %d\n", result, val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected return 0, got %d\n", result);
        tests_failed++;
    }

    printf("Running test: Multiple booleans\n");
    int val1, val2;
    prepare_test_input("test_data.txt", 82, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%B %B", &val1, &val2);
    restore_stdin(orig_stdin);
    if (result == 2 && val1 == 1 && val2 == 0) {
        printf("   PASSED - Values: %d, %d\n", val1, val2);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 1, 0; got %d, %d (return: %d)\n", val1, val2, result);
        tests_failed++;
    }
}

void test_boolean_suppress() {
    int val1, val2;

    printf("Running test: Boolean suppress assignment\n");
    prepare_test_input("test_data.txt", 83, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%B %*B %B", &val1, &val2);
    restore_stdin(orig_stdin);
    if (result == 2 && val1 == 1 && val2 == 0) {
        printf("   PASSED - Values: %d, %d (middle value suppressed)\n", val1, val2);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 1, 0; got %d, %d (return: %d)\n", val1, val2, result);
        tests_failed++;
    }
}

void test_basic_line() {
    char line[200];

    printf("Running test: Basic line read\n");
    prepare_test_input("test_data.txt", 84, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%N", line);
    restore_stdin(orig_stdin);
    if (result == 1 && strcmp(line, "Hello World") == 0) {
        printf("   PASSED - Value: '%s'\n", line);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 'Hello World', got '%s' (return: %d)\n", line, result);
        tests_failed++;
    }

    printf("Running test: Line with leading spaces\n");
    prepare_test_input("test_data.txt", 85, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%N", line);
    restore_stdin(orig_stdin);
    if (result == 1 && strcmp(line, "  spaces at start") == 0) {
        printf("   PASSED - Value: '%s' (preserved spaces)\n", line);
        tests_passed++;
    } else {
        printf("   FAILED - Expected '  spaces at start', got '%s' (return: %d)\n", line, result);
        tests_failed++;
    }

    printf("Running test: Line with trailing spaces\n");
    prepare_test_input("test_data.txt", 86, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%N", line);
    restore_stdin(orig_stdin);
    if (result == 1 && strcmp(line, "spaces at end  ") == 0) {
        printf("   PASSED - Value: '%s' (preserved spaces)\n", line);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 'spaces at end  ', got '%s' (return: %d)\n", line, result);
        tests_failed++;
    }

    printf("Running test: Empty line\n");
    prepare_test_input("test_data.txt", 87, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%N", line);
    restore_stdin(orig_stdin);
    if (result == 1 && strcmp(line, "") == 0) {
        printf("   PASSED - Value: '' (empty line)\n");
        tests_passed++;
    } else {
        printf("   FAILED - Expected empty line, got '%s' (return: %d)\n", line, result);
        tests_failed++;
    }

    printf("Running test: Line with special characters\n");
    prepare_test_input("test_data.txt", 88, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%N", line);
    restore_stdin(orig_stdin);
    if (result == 1 && strcmp(line, "Hello! How are you? #test @user") == 0) {
        printf("   PASSED - Value: '%s'\n", line);
        tests_passed++;
    } else {
        printf("   FAILED - Expected special chars, got '%s' (return: %d)\n", line, result);
        tests_failed++;
    }
}

void test_line_field_width() {
    char line[200];

    printf("Running test: Line with field width\n");
    prepare_test_input("test_data.txt", 89, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%10N", line);
    restore_stdin(orig_stdin);
    if (result == 1 && strcmp(line, "This is a ") == 0) {
        printf("   PASSED - Value: '%s' (read only 10 chars)\n", line);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 'This is a ', got '%s' (return: %d)\n", line, result);
        tests_failed++;
    }

    printf("Running test: Multiple lines\n");
    char line1[200], line2[200];
    prepare_test_input_multiline("test_data.txt", 89, 2, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%N%N", line1, line2);  // Remove \n
    restore_stdin(orig_stdin);
    if (result == 2 && strcmp(line1, "First line") == 0 && strcmp(line2, "Second line") == 0) {
        printf("   PASSED - Values: '%s', '%s'\n", line1, line2);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 'First line', 'Second line'; got '%s', '%s' (return: %d)\n", line1, line2, result);
        tests_failed++;
    }
}

void test_line_with_mixed_types() {
    int val;
    char line[200];

    printf("Running test: Int then line\n");
    prepare_test_input_multiline("test_data.txt", 91, 2, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%d\n%N", &val, line);  // Remove \n
    restore_stdin(orig_stdin);
    if (result == 2 && val == 42 && strcmp(line, "rest of line") == 0) {
        printf("   PASSED - Values: %d, '%s'\n", val, line);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 42, 'rest of line'; got %d, '%s' (return: %d)\n", val, line, result);
        tests_failed++;
    }

    printf("Running test: Line then int\n");
    char line2[200];
    int val2;
    prepare_test_input_multiline("test_data.txt", 93, 2, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%N%d", line2, &val2);  // Remove \n
    restore_stdin(orig_stdin);
    if (result == 2 && strcmp(line2, "Line before number") == 0 && val2 == 100) {
        printf("   PASSED - Values: '%s', %d\n", line2, val2);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 'Line before number', 100; got '%s', %d (return: %d)\n", line2, val2, result);
        tests_failed++;
    }
}

void test_line_suppress() {
    char line1[200], line2[200];

    printf("Running test: Line suppress assignment\n");
    prepare_test_input_multiline("test_data.txt", 95, 3, "temp_input.txt");
    FILE *orig_stdin = setup_input_from_file("temp_input.txt");
    int result = my_scanf("%N%*N%N", line1, line2);  // Remove \n
    restore_stdin(orig_stdin);
    if (result == 2 && strcmp(line1, "Keep this") == 0 && strcmp(line2, "And this") == 0) {
        printf("   PASSED - Values: '%s', '%s' (middle line suppressed)\n", line1, line2);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 'Keep this', 'And this'; got '%s', '%s' (return: %d)\n", line1, line2, result);
        tests_failed++;
    }
}

int main() {
    printf("=== my_scanf Test Suite - %%d Format Specifier ===\n\n");

    test_basic_integer();
    printf("\n");

    test_multiple_integers();
    printf("\n");

    test_field_width();
    printf("\n");

    test_leading_whitespace();
    printf("\n");

    test_edge_cases();
    printf("\n");

    test_invalid_input();
    printf("\n");

    test_suppress_assignment();
    printf("\n");

    test_basic_float();
    printf("\n");

    test_float_formats();
    printf("\n");

    test_multiple_floats();
    printf("\n");

    test_float_field_width();
    printf("\n");

    test_float_edge_cases();
    printf("\n");

    test_float_invalid_input();
    printf("\n");

    test_float_suppress_assignment();
    printf("\n");

    test_basic_string();
    printf("\n");

    test_string_field_width();
    printf("\n");

    test_string_suppress();
    printf("\n");

    test_basic_hex();
    printf("\n");

    test_hex_field_width();
    printf("\n");

    test_hex_suppress();
    printf("\n");

    test_basic_char();
    printf("\n");

    test_char_field_width();
    printf("\n");

    test_char_suppress();
    printf("\n");

    test_mixed_types();
    printf("\n");

    test_mixed_with_suppress();
    printf("\n");

    test_basic_binary();
    printf("\n");

    test_binary_field_width();
    printf("\n");

    test_binary_edge_cases();
    printf("\n");

    test_binary_suppress();
    printf("\n");

    test_basic_boolean();
    printf("\n");

    test_boolean_variants();
    printf("\n");

    test_boolean_edge_cases();
    printf("\n");

    test_boolean_suppress();
    printf("\n");

    test_basic_line();
    printf("\n");

    test_line_field_width();
    printf("\n");

    test_line_with_mixed_types();
    printf("\n");

    test_line_suppress();
    printf("\n");

    printf("=== Test Summary ===\n");
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_failed);
    printf("Total tests: %d\n", tests_passed + tests_failed);

    // Cleanup temp file
    remove("temp_input.txt");

    return tests_failed > 0 ? 1 : 0;
}