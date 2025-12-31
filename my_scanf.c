#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>

int read_int(va_list args, int width, char size_modifier, int suppress) {
    int c;
    int sign = 1;
    long long value = 0;
    int digit_count = 0;
    int chars_read = 0;

    // Consume all leading whitespace
    while ((c = getchar()) != EOF && isspace(c)) {}

    if (c == EOF) {
        return 0;  // Failed to read anything
    }

    // Check for optional sign
    if (c == '+' || c == '-') {
        if (c == '-') {
            sign = -1;
        }
        chars_read++;

        // Check if we've hit width limit after just the sign
        // Don't check if the width limit is 0 ie unlimited
        if (width > 0 && chars_read >= width) {
            return 0;  // No digits read, just a sign
        }
        // We can still read in more digits, go to the next one
        c = getchar();
    }

    // Read digits
    while (c != EOF && isdigit(c)) {
        // Convert string numbers to values
        value = value * 10 + (c - '0');
        digit_count++;
        chars_read++;

        // Check width limit after each digit read in, unless width is 0 ie unlimited 
        if (width > 0 && chars_read >= width) {
            break;
        }

        c = getchar();
    }

    // Put back the non-digit character we just read (if not EOF)
    if (c != EOF && !isdigit(c)) {
        ungetc(c, stdin);
    }

    // Check if we actually read any digits
    if (digit_count == 0) {
        return 0;  // Failed - no digits found
    }

    // Apply sign (optional)
    value *= sign;

    // Get the pointer (next argument from va_list) where we should store the result we read and store based on size modifier
    // Do not store the value if assignment suppression
    if (!suppress) {
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
        // Successful conversion
        return 1;
    }
}

int read_float(va_list args, int width, char size_modifier) {
    return 0;
}

int read_hex(va_list args, int width, char size_modifier) {
    return 0;
}

int read_char(va_list args, int width) {
    return 0;
}

int read_string(va_list args, int width) {
    return 0;
}

int parse_format_string(const char *format, va_list args) {
    int successful = 0;
    int i = 0;

    while (format[i] != '\0') {
        // Check for literal characters or whitespace
        if (format[i] != '%') {
            // Consume any amount of whitespace from input stream
            if (isspace(format[i])) {
                int c;
                while ((c = getchar()) != EOF && isspace(c)) {
                }
                // When you find a non whitespace character return it to the stream
                // so it can be read by the next format specifier
                if (c != EOF) {
                    ungetc(c, stdin);
                }
                // Move to the next character in the format string
                i++;
                continue;
            }

            // Match literal characters exactly
            int c = getchar();
            if (c != format[i]) {
                // Mismatch - matching failure
                if (c != EOF) {
                    ungetc(c, stdin);
                }
                return successful;
            }
            // Move to the next character in the format string
            i++;
            continue;
        }
        // At this point we have a '%' so see what follows it to determine which format specifier to use
        i++;

        // Check for '%%' (literal %)
        if (format[i] == '%') {
            int c = getchar();
            // If you didn't find the literal % in the input stream, return immediately
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
        // Will read from the input stream, but not store the value
        int suppress = 0;
        if (format[i] == '*') {
            suppress = 1;
            i++;
        }

        // Parse width (optional modifier)
        int width = 0;
        // if there is a digit following the % it is a width modifier
        // if no digit the loop doesn't run so width stays at 0 meaning unlimited
        while (isdigit(format[i])) {
            // Convert string numbers to values
            // ascii value of the number - the ascii value of 0 = the number
            // * 10 to account for the place when a new digit is added to the right
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

        // Determine which format specifier to use
        char specifier = format[i];
        int success = 0;

        switch (specifier) {
            case 'd':
                success = read_int(args, width, size_modifier, suppress);
                break;
            case 'f':
                success = read_float(args, width, specifier);
                break;
            case 'x':
            case 'X':
                success = read_hex(args, width, specifier);
                break;
            case 'c':
                success = read_char(args, width);
                break;
            case 's':
                success = read_string(args, width);
                break;

            default:
                // Unknown format specifier fails and returns however many successful conversions have occurred up to this point
                    return successful;
        }

        // If we successfully read something, and it's not suppressed, increment count
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
//
// int main() {
//     // Test 1: Simple integers
//     printf("Test 1: Enter two integers (e.g., 10 20): ");
//     int a, b;
//     int result = my_scanf("%d %d", &a, &b);
//     printf("Read %d items: a=%d, b=%d\n\n", result, a, b);
//
//     // Test 2: With width modifier
//     printf("Test 2: Enter a number (will read max 3 digits): ");
//     int c;
//     result = my_scanf("%3d", &c);
//     printf("Read %d items: c=%d\n\n", result, c);
//
//     // Test 3: Negative numbers
//     printf("Test 3: Enter a negative number: ");
//     int d;
//     result = my_scanf("%d", &d);
//     printf("Read %d items: d=%d\n\n", result, d);
//
//     return 0;
// }