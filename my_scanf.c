#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>

int read_int(va_list *args, int width, char size_modifier, int suppress) {
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
            short *ptr = va_arg(*args, short*);
            *ptr = (short)value;
        } else if (size_modifier == 'H') {
            // char (hh modifier - using 'H' to represent)
            signed char *ptr = va_arg(*args, signed char*);
            *ptr = (signed char)value;
        } else if (size_modifier == 'l') {
            // long
            long *ptr = va_arg(*args, long*);
            *ptr = (long)value;
        } else if (size_modifier == 'L') {
            // long long (ll modifier - using 'L' to represent)
            long long *ptr = va_arg(*args, long long*);
            *ptr = value;
        } else {
            // regular int (default)
            int *ptr = va_arg(*args, int*);
            *ptr = (int)value;
        }
    }
    return 1;
}

int read_float(va_list *args, int width, char size_modifier, int suppress) {
    int c;
    int chars_read = 0;
    int sign = 1;

    double int_part = 0.0;
    double frac_part = 0.0;
    double frac_divisor = 1.0;

    int saw_digit = 0;
    int saw_fraction = 0;
    int saw_exponent = 0;

    int exp_sign = 1;
    int exponent = 0;

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

    // 3) Integer part digits
    while (c != EOF && isdigit(c)) {
        saw_digit = 1;
        int_part = int_part * 10.0 + (c - '0');
        chars_read++;

        if (width > 0 && chars_read >= width) {
            break;
        }

        c = getchar();
    }

    // 4) Fractional part (optional)
    if (c == '.' && (width == 0 || chars_read < width)) {
        saw_fraction = 1;
        chars_read++;

        c = getchar();

        while (c != EOF && isdigit(c) && (width == 0 || chars_read < width)) {
            saw_digit = 1;
            frac_part = frac_part * 10.0 + (c - '0');
            frac_divisor *= 10.0;
            chars_read++;

            if (width > 0 && chars_read >= width) break;

            c = getchar();
        }
    }

    // 5) Exponent part (optional)
    if ((c == 'e' || c == 'E') && (width == 0 || chars_read < width)) {
        saw_exponent = 1;
        chars_read++;

        c = getchar();

        // exponent optional sign
        if (c == '+' || c == '-') {
            if (c == '-') exp_sign = -1;
            chars_read++;
            c = getchar();
        }

        // exponent digits
        int exp_digits = 0;
        while (c != EOF && isdigit(c) && (width == 0 || chars_read < width)) {
            exponent = exponent * 10 + (c - '0');
            exp_digits++;
            chars_read++;

            if (width > 0 && chars_read >= width) break;

            c = getchar();
        }

        if (exp_digits == 0) {
            // malformed exponent → stop parsing at 'e'
            // push back offending char
            if (c != EOF) ungetc(c, stdin);
            // ignore exponent entirely
            exponent = 0;
            saw_exponent = 0;
        }
    }

    // 6) push back extra character if it is NOT part of the float
    if (c != EOF && !isspace(c)) {
        // same “don’t push last digit on width-limit” rule as %d
        if (!isdigit(c) && c != '.' && c != 'e' && c != 'E' && c != '+' && c != '-') {
            ungetc(c, stdin);
        }
    }

    // 7) If no digits seen at all -> fail
    if (!saw_digit) {
        return 0;
    }

    // 8) Build final value
    double value = int_part;

    if (saw_fraction) {
        value += frac_part / frac_divisor;
    }

    value *= sign;

    // exponent math
    if (saw_exponent && exponent != 0) {
        double pow10 = 1.0;
        for (int i = 0; i < exponent; i++) {
            pow10 *= 10.0;
        }
        if (exp_sign == -1)
            value /= pow10;
        else
            value *= pow10;
    }
    if (!suppress) {
        // 9) Store using size modifier
        if (size_modifier == 'l') {
            double *ptr = va_arg(*args, double*);
            *ptr = value;
        } else if (size_modifier == 'L') {
            long double *ptr = va_arg(*args, long double*);
            *ptr = (long double)value;
        } else {
            float *ptr = va_arg(*args, float*);
            *ptr = (float)value;
        }
    }
    return 1;
}

int read_hex(va_list *args, int width, char size_modifier) {
    return 0;
}

int read_char(va_list *args, int width) {
    return 0;
}

int read_string(va_list *args, int width) {
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
                success = read_int(&args, width, size_modifier, suppress);
                break;
            case 'f':
                success = read_float(&args, width, size_modifier, suppress);
                break;
            case 'x':
            case 'X':
                success = read_hex(&args, width, size_modifier);
                break;
            case 'c':
                success = read_char(&args, width);
                break;
            case 's':
                success = read_string(&args, width);
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



// int main(void) {
//     float f1;
//     double d1;
//     long double ld1;
//
//     printf("=== Testing %%f with my_scanf ===\n\n");
//
//     // 1) basic float
//     printf("Test 1: enter a float (example: 3.14): ");
//     int count = my_scanf("%f", &f1);
//     printf("  return value = %d, f1 = %f\n\n", count, f1);
//
//     // 2) leading/trailing spaces and sign
//     printf("Test 2: enter a signed float with spaces (example:   -2.5  ): ");
//     count = my_scanf("%f", &f1);
//     printf("  return value = %d, f1 = %f\n\n", count, f1);
//
//     // 3) number with no integer part (.5)
//     printf("Test 3: enter fractional-only (example: .5): ");
//     count = my_scanf("%f", &f1);
//     printf("  return value = %d, f1 = %.10g\n\n", count, f1);
//
//     // 4) number with no fractional digits (5.)
//     printf("Test 4: enter with trailing dot (example: 5.): ");
//     count = my_scanf("%f", &f1);
//     printf("  return value = %d, f1 = %.10g\n\n", count, f1);
//
//     // 5) scientific notation
//     printf("Test 5: enter exponent notation (example: 1.23e3): ");
//     count = my_scanf("%f", &f1);
//     printf("  return value = %d, f1 = %.10g\n\n", count, f1);
//
//     // 6) double with %lf
//     printf("Test 6: enter a double using %%lf (example: 123.456): ");
//     count = my_scanf("%lf", &d1);
//     printf("  return value = %d, d1 = %.15g\n\n", count, d1);
//
//     // 7) long double with %Lf
//     printf("Test 7: enter a long double using %%Lf (example: 0.000123): ");
//     count = my_scanf("%Lf", &ld1);
//     printf("  return value = %d, ld1 = %.18Lg\n\n", count, ld1);
//
//     // 8) multiple values in one call
//     printf("Test 8: enter three floats separated by spaces (example: 1.1 2.2 3.3): ");
//     float a, b, c;
//     count = my_scanf("%f %f %f", &a, &b, &c);
//     printf("  return value = %d, values = %.6g, %.6g, %.6g\n\n", count, a, b, c);
//
//     // 9) invalid input test
//     printf("Test 9: enter something invalid for a float (example: abc): ");
//     f1 = 123.45f;  // sentinel to see if it changes
//     count = my_scanf("%f", &f1);
//     printf("  return value = %d, f1 (should be unchanged or unspecified) = %.10g\n\n", count, f1);
//
//     printf("=== Done ===\n");
//     return 0;
// }
