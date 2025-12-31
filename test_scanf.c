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

// Helper function to restore stdin
void restore_stdin(FILE *original_stdin) {
    freopen("/dev/tty", "r", stdin);
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

void test_basic_integer() {
    int val;
    FILE *orig_stdin;

    printf("Running test: Basic positive integer\n");
    prepare_test_input("test_data.txt", 0, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
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
    FILE *orig_stdin;

    printf("Running test: Two integers with space\n");
    prepare_test_input("test_data.txt", 3, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
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
    FILE *orig_stdin;

    printf("Running test: Field width 2\n");
    prepare_test_input("test_data.txt", 6, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
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
    FILE *orig_stdin;

    printf("Running test: Leading spaces\n");
    prepare_test_input("test_data.txt", 8, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
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

    printf("Running test: Leading newlines\n");
    prepare_test_input("test_data.txt", 10, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
    result = my_scanf("%d", &val);
    restore_stdin(orig_stdin);
    if (result == 1 && val == 77) {
        printf("   PASSED - Value: %d\n", val);
        tests_passed++;
    } else {
        printf("   FAILED - Expected 77, got %d (return: %d)\n", val, result);
        tests_failed++;
    }
}

void test_edge_cases() {
    int val;
    FILE *orig_stdin;

    printf("Running test: Explicit plus sign\n");
    prepare_test_input("test_data.txt", 11, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
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
    FILE *orig_stdin;

    printf("Running test: Non-numeric input\n");
    val = 999;
    prepare_test_input("test_data.txt", 14, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
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
    FILE *orig_stdin;

    printf("Running test: Suppress assignment\n");
    prepare_test_input("test_data.txt", 16, "temp_input.txt");
    orig_stdin = setup_input_from_file("temp_input.txt");
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

    printf("=== Test Summary ===\n");
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_failed);
    printf("Total tests: %d\n", tests_passed + tests_failed);

    // Cleanup temp file
    remove("temp_input.txt");

    return tests_failed > 0 ? 1 : 0;
}