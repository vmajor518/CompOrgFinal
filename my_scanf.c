#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>

int read_int(va_list args, int width, char size_modifier) {
    int c;
    int sign = 1;
    long long value = 0;
    int digit_count = 0;
    int chars_read = 0;

    // Skip leading whitespace
    while ((c = getchar()) != EOF && isspace(c)) {
        // Just consume whitespace
    }

    if (c == EOF) {
        return 0;  // Failed to read anything
    }

    // Check for sign
    if (c == '+' || c == '-') {
        if (c == '-') {
            sign = -1;
        }
        chars_read++;

        // Check if we've hit width limit
        if (width > 0 && chars_read >= width) {
            return 0;  // No digits read, just a sign
        }

        c = getchar();
    }

    // Read digits
    while (c != EOF && isdigit(c)) {
        value = value * 10 + (c - '0');
        digit_count++;
        chars_read++;

        // Check width limit
        if (width > 0 && chars_read >= width) {
            break;
        }

        c = getchar();
    }

    // Put back the non-digit character we just read (if not EOF)
    if (c != EOF) {
        ungetc(c, stdin);
    }

    // Check if we read any digits
    if (digit_count == 0) {
        return 0;  // Failed - no digits found
    }

    // Apply sign
    value *= sign;

    // Get the pointer where we should store the result
    // and store based on size modifier
    if (size_modifier == 'h') {
        // short
        short *ptr = va_arg(args, short*);
        *ptr = (short)value;
    } else if (size_modifier == 'H') {
        // char (hh modifier - using 'H' to represent)
        signed char *ptr = va_arg(args, signed char*);
        *ptr = (signed char)value;
    } else if (size_modifier == 'l') {
        // long
        long *ptr = va_arg(args, long*);
        *ptr = (long)value;
    } else if (size_modifier == 'L') {
        // long long (ll modifier - using 'L' to represent)
        long long *ptr = va_arg(args, long long*);
        *ptr = value;
    } else {
        // regular int (default)
        int *ptr = va_arg(args, int*);
        *ptr = (int)value;
    }

    return 1;
}

int parse_format_string(const char *format, va_list args) {
    int successful = 0;
    int i = 0;

    while (format[i] != '\0') {
        if (format[i] != '%') {
            // Skip any amount of whitespace in input
            if (isspace(format[i])) {
                int c;
                while ((c = getchar()) != EOF && isspace(c)) {
                }
                if (c != EOF) {
                    ungetc(c, stdin);
                }
                i++;
                continue;
            }

            // Match character exactly
            int c = getchar();
            if (c != format[i]) {
                // Mismatch - matching failure
                if (c != EOF) {
                    ungetc(c, stdin);
                }
                return successful;
            }
            i++;
            continue;
        }

        // We have a '%'
        i++;

        // Check for '%%' (literal %)
        if (format[i] == '%') {
            int c = getchar();
            if (c != '%') {
                if (c != EOF) {
                    ungetc(c, stdin);
                }
                return successful;
            }
            i++;
            continue;
        }

        // Check for assignment suppression '*'
        int suppress = 0;
        if (format[i] == '*') {
            suppress = 1;
            i++;
        }

        // Parse width (optional number)
        int width = 0;
        while (isdigit(format[i])) {
            width = width * 10 + (format[i] - '0');
            i++;
        }

        // Parse size modifier (h, hh, l, ll, L)
        char size_modifier = '\0';
        if (format[i] == 'h') {
            i++;
            if (format[i] == 'h') {
                size_modifier = 'H';  // hh
                i++;
            } else {
                size_modifier = 'h';  // h
            }
        } else if (format[i] == 'l') {
            i++;
            if (format[i] == 'l') {
                size_modifier = 'L';  // ll
                i++;
            } else {
                size_modifier = 'l';  // l
            }
        } else if (format[i] == 'L') {
            size_modifier = 'L';  // long double (for %f)
            i++;
        }

        // Parse conversion specifier
        char specifier = format[i];
        int success = 0;

        switch (specifier) {
            case 'd':
            case 'i':
                success = read_int(args, width, size_modifier);
            break;

            default:
                // Unknown format specifier
                    return successful;
        }

        // If we successfully read something and it's not suppressed, increment count
        if (success && !suppress) {
            successful++;
        } else if (!success) {
            // Matching failure - return current count
            return successful;
        }

        i++;
    }

    return successful;
}

int my_scanf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    int result = parse_format_string(format, args);

    va_end(args);
    return result;
}

int main() {
    int x, y;

    printf("Enter one integer: ");
    int result = my_scanf("%d", &x);
    printf("Successfully read %d items\n", result);
    printf("x = %d\n", x);

    printf("Enter two integers: ");
    result = my_scanf("%d %d", &x, &y);
    printf("Successfully read %d items\n", result);
    printf("x = %d, y = %d\n", x, y);

    return 0;
}