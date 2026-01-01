#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>

int read_int(const va_list args, int width, char size_modifier, int suppress) {
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
    }
    return 1;
}

int read_float(const va_list args, int width, char size_modifier, int suppress) {
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
            double *ptr = va_arg(args, double*);
            *ptr = value;
        } else if (size_modifier == 'L') {
            long double *ptr = va_arg(args, long double*);
            *ptr = (long double)value;
        } else {
            float *ptr = va_arg(args, float*);
            *ptr = (float)value;
        }
    }
    return 1;
}

int read_hex(const va_list args, int width, char size_modifier, int suppress) {
    int c;
    unsigned long long value = 0;
    int digit_count = 0;
    int chars_read = 0;

    // Consume all leading whitespace
    while ((c = getchar()) != EOF && isspace(c)) {}

    if (c == EOF) {
        return 0;  // Failed to read anything
    }

    // Optional: handle 0x or 0X prefix
    if (c == '0') {
        chars_read++;

        // Check if we've hit width limit after just the '0'
        // Don't check if the width limit is 0 ie unlimited
        if (width > 0 && chars_read >= width) {
            // Just read a '0', treat it as a hex digit
            value = 0;
            digit_count = 1;
            // Continue to store the value below
        } else {
            // We can still read more, check for 'x' or 'X'
            int next = getchar();
            if (next == 'x' || next == 'X') {
                // 0x prefix found, this counts toward width but not as a digit
                chars_read++;

                // Check width limit after the prefix
                if (width > 0 && chars_read >= width) {
                    return 0;  // No hex digits read, just "0x"
                }

                // Continue reading hex digits
                c = getchar();
            } else {
                // Just a leading 0, process it as a hex digit
                ungetc(next, stdin);
                // c is already '0', will be processed below
                value = 0;
                digit_count = 1;
                c = getchar();
            }
        }
    }

    // Read hexadecimal digits
    while (c != EOF) {
        int hex_value = -1;

        if (c >= '0' && c <= '9') {
            hex_value = c - '0';
        } else if (c >= 'a' && c <= 'f') {
            hex_value = c - 'a' + 10;
        } else if (c >= 'A' && c <= 'F') {
            hex_value = c - 'A' + 10;
        } else {
            // Not a hex digit, put it back
            ungetc(c, stdin);
            break;
        }

        // If we found a valid hex digit, add it to value
        if (hex_value != -1) {
            value = value * 16 + hex_value;
            digit_count++;
            chars_read++;

            // Check width limit after each digit read in, unless width is 0 ie unlimited
            if (width > 0 && chars_read >= width) {
                break;
            }
        }

        c = getchar();
    }

    // Check if we actually read any hex digits
    if (digit_count == 0) {
        return 0;  // Failed - no hex digits found
    }

    // Get the pointer (next argument from va_list) where we should store the result and store based on size modifier
    // Do not store the value if assignment suppression
    if (!suppress) {
        if (size_modifier == 'h') {
            // unsigned short
            unsigned short *ptr = va_arg(args, unsigned short*);
            *ptr = (unsigned short)value;
        } else if (size_modifier == 'H') {
            // unsigned char (hh modifier - using 'H' to represent)
            unsigned char *ptr = va_arg(args, unsigned char*);
            *ptr = (unsigned char)value;
        } else if (size_modifier == 'l') {
            // unsigned long
            unsigned long *ptr = va_arg(args, unsigned long*);
            *ptr = (unsigned long)value;
        } else if (size_modifier == 'L') {
            // unsigned long long (ll modifier - using 'L' to represent)
            unsigned long long *ptr = va_arg(args, unsigned long long*);
            *ptr = value;
        } else {
            // regular unsigned int (default)
            unsigned int *ptr = va_arg(args, unsigned int*);
            *ptr = (unsigned int)value;
        }
    }

    return 1;  // Successfully read 1 item
}

int read_char(const va_list args, int width, int suppress) {
    if (width == 0) {
        width = 1;  // Default: read 1 character
    }

    char *dest = NULL;
    if (!suppress) {
        dest = va_arg(args, char*);
    }

    int chars_read = 0;

    // Read exactly 'width' characters (or until EOF)
    for (int i = 0; i < width; i++) {
        int c = getchar();

        if (c == EOF) {
            // If we hit EOF before reading all requested chars,
            // scanf fails and returns EOF (or the count so far)
            break;
        }

        if (!suppress) {
            dest[chars_read] = (char)c;
        }
        chars_read++;
    }

    // Return 1 only if we successfully read at least one character
    return chars_read > 0 ? 1 : 0;
}

int read_string(const va_list args, int width, int suppress) {
    int c;
    int chars_read = 0;

    // Consume all leading whitespace
    while ((c = getchar()) != EOF && isspace(c)) {}

    if (c == EOF) {
        return 0;  // Failed to read anything
    }

    // Get the pointer where we should store the result
    // Only get it if we're not suppressing assignment
    char *dest = NULL;
    if (!suppress) {
        dest = va_arg(args, char*);
    }

    // Read non-whitespace characters
    while (c != EOF && !isspace(c)) {
        // Store the character if not suppressing
        if (!suppress) {
            dest[chars_read] = (char)c;
        }
        chars_read++;

        // Check width limit after each character read in, unless width is 0 ie unlimited
        if (width > 0 && chars_read >= width) {
            break;
        }

        c = getchar();
    }

    // Put back the whitespace character we just read (if not EOF)
    if (c != EOF && !isspace(c)) {
        ungetc(c, stdin);
    }

    // Check if we actually read any characters
    if (chars_read == 0) {
        return 0;  // Failed - no characters found
    }

    // Null-terminate the string if not suppressed
    if (!suppress) {
        dest[chars_read] = '\0';
    }

    return 1;  // Successfully read 1 item
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
                success = read_float(args, width, size_modifier, suppress);
                break;
            case 'x':
            case 'X':
                success = read_hex(args, width, size_modifier, suppress);
                break;
            case 'c':
                success = read_char(args, width, suppress);
                break;
            case 's':
                success = read_string(args, width, suppress);
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