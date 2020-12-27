#include <lib/log.hpp>

#include <terminal.hpp>

#include <climits>
#include <cstdarg>
#include <lib/string.hpp>

#define INT32_MAX_DIGITS 12
#define INT32_MAX_HEX_DIGITS 8

static void print(const char* data, u32 length) {
    for (u32 i = 0; i < length; i++) {
        Terminal::instance->putChar(data[i]);
    }
}

int log(int level, const char* __restrict format, ...) {
    switch (level) {
        case 0:
            print("[", 1);
            Terminal::instance->setColor(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
            print(" LOG ", 5);
            Terminal::instance->setColor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
            print("] ", 2);
            break;
        case 1:
            print("[", 1);
            Terminal::instance->setColor(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
            print(" WARN ", 6);
            Terminal::instance->setColor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
            print("] ", 2);
            break;
        case 2:
            print("[", 1);
            Terminal::instance->setColor(VGA_COLOR_RED, VGA_COLOR_BLACK);
            print(" ERR ", 6);
            Terminal::instance->setColor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
            print("] ", 2);
            break;
    }

    va_list parameters;
    va_start(parameters, format);

    int written = 0;

    while (*format != '\0') {
        u32 maxrem = INT_MAX - written;

        if (format[0] != '%' || format[1] == '%') {
            if (format[0] == '%') {
                format++;
            }
            u32 amount = 1;
            while (format[amount] && format[amount] != '%') {
                amount++;
            }
            if (maxrem < amount) {
                return -1;
            }
            print(format, amount);
            format += amount;
            written += amount;
            continue;
        }

        const char* format_begun_at = format++;

        if (*format == 'c') {
            format++;
            char c = (char) va_arg(parameters, int);
            if (!maxrem) {
                return -1;
            }
            print(&c, sizeof(c));
            written++;
        } else if (*format == 's') {
            format++;
            const char* str = va_arg(parameters, const char*);
            u32 len = strlen(str);
            if (maxrem < len) {
                return -1;
            }
            print(str, len);
            written += len;
        } else if (*format == 'd') {
            format++;
            int n = va_arg(parameters, int);

            char str[INT32_MAX_DIGITS + 1] = {0};
            char* str_ptr = str;

            intToString(n, str_ptr);
            u32 len = strlen(str);

            if (maxrem < len) {
                return -1;
            }

            print(str, len);

            written += len;
        } else if (*format == 'x') {
            format++;
            int n = va_arg(parameters, int);

            char str[INT32_MAX_HEX_DIGITS + 1] = {};
            char* str_ptr = str;

            intToHex(n, str_ptr);
            u32 len = strlen(str);

            if (maxrem < len) {
                return -1;
            }

            print(str, len);

            written += len;

        } else {
            format = format_begun_at;
            u32 len = strlen(format);
            if (maxrem < len) {
                return -1;
            }
            print(format, len);
            written += len;
            format += len;
        }
    }

    va_end(parameters);

    Terminal::instance->putChar('\n');

    return 0;
}

static bool rawPrintk(const char* data, u32 length) {
    const auto* bytes = (const u8*) data;
    Port8Bit port(0x3F8);

    for (u32 i = 0; i < length; i++) {
        port.write(bytes[i]);
    }

    return true;
}

int printk(const char* __restrict format, ...) {
    va_list parameters;
    va_start(parameters, format);

    int written = 0;

    while (*format != '\0') {
        u32 maxrem = INT_MAX - written;

        if (format[0] != '%' || format[1] == '%') {
            if (format[0] == '%') {
                format++;
            }
            u32 amount = 1;
            while (format[amount] && format[amount] != '%') {
                amount++;
            }
            if (maxrem < amount) {
                return -1;
            }
            rawPrintk(format, amount);
            format += amount;
            written += amount;
            continue;
        }

        const char* format_begun_at = format++;

        if (*format == 'c') {
            format++;
            char c = (char) va_arg(parameters, int);
            if (!maxrem) {
                return -1;
            }
            rawPrintk(&c, sizeof(c));
            written++;
        } else if (*format == 's') {
            format++;
            const char* str = va_arg(parameters, const char*);
            u32 len = strlen(str);
            if (maxrem < len) {
                return -1;
            }
            rawPrintk(str, len);
            written += len;
        } else if (*format == 'd') {
            format++;
            int n = va_arg(parameters, int);

            char str[INT32_MAX_DIGITS + 1] = {0};
            char* str_ptr = str;

            intToString(n, str_ptr);
            u32 len = strlen(str);

            if (maxrem < len) {
                return -1;
            }

            rawPrintk(str, len);

            written += len;
        } else if (*format == 'x') {
            format++;
            int n = va_arg(parameters, int);

            char str[INT32_MAX_HEX_DIGITS + 1] = {};
            char* str_ptr = str;

            intToHex(n, str_ptr);
            u32 len = strlen(str);

            if (maxrem < len) {
                return -1;
            }

            rawPrintk(str, len);

            written += len;

        } else {
            format = format_begun_at;
            u32 len = strlen(format);
            if (maxrem < len) {
                return -1;
            }
            rawPrintk(format, len);
            written += len;
            format += len;
        }
    }

    va_end(parameters);

    return written;
}