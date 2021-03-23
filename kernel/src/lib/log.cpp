#include <lib/log.hpp>

#include <terminal.hpp>

#include <climits>
#include <cstdarg>
#include <lib/string.hpp>

static void print(const char* data, u32 length) {
    for (u32 i = 0; i < length; i++) {
        Terminal::instance->putChar(data[i]);
    }
}

void printf(const char* __restrict format, ...) {
    va_list parameters;
    va_start(parameters, format);

    while (*format != '\0') {
        if (format[0] != '%' || format[1] == '%') {
            if (format[0] == '%') {
                format++;
            }
            u32 amount = 1;
            while (format[amount] && format[amount] != '%') {
                amount++;
            }
            print(format, amount);
            format += amount;
            continue;
        }

        const char* format_begun_at = format++;

        if (*format == 'c') {
            format++;
            char c = (char) va_arg(parameters, int);
            print(&c, sizeof(c));
        } else if (*format == 's') {
            format++;
            const char* str = va_arg(parameters, const char*);
            u32 len = strlen(str);
            print(str, len);
        } else if (*format == 'd') {
            format++;
            long long n = va_arg(parameters, long long);
            printk("%d", (u32) n);
            int numChars = 0;
            if (n < 0) {
                n = -n;
                numChars++;
                print("-", 1);
            }

            int temp = n;
            do {
                numChars++;
                temp /= 10;
            } while (temp);

            print(n > 0 ? intToString((u32) n) : intToString((int) n), numChars);
        } else if (*format == 'x') {
            format++;
            long long n = va_arg(parameters, long long);
            int numChars = 0;
            if (n < 0) {
                n = -n;
                numChars++;
                print("-", 1);
            }

            printk("\n\n%s\n\n", intToHex(n));

            char* s = intToHex(n);
            print(s, strlen(s));
        } else {
            format = format_begun_at;
            u32 len = strlen(format);
            print(format, len);
            format += len;
        }
    }

    va_end(parameters);
}

void log(int level, const char* __restrict format, ...) {
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
        default:
            break;
    }

    va_list parameters;
    va_start(parameters, format);

    while (*format != '\0') {
        if (format[0] != '%' || format[1] == '%') {
            if (format[0] == '%') {
                format++;
            }
            u32 amount = 1;
            while (format[amount] && format[amount] != '%') {
                amount++;
            }
            print(format, amount);
            format += amount;
            continue;
        }

        const char* format_begun_at = format++;

        if (*format == 'c') {
            format++;
            char c = (char) va_arg(parameters, int);
            print(&c, sizeof(c));
        } else if (*format == 's') {
            format++;
            const char* str = va_arg(parameters, const char*);
            u32 len = strlen(str);
            print(str, len);
        } else if (*format == 'd') {
            format++;
            // TODO: find another way to recognise if parameter is signed or unsigned
            u32 n = va_arg(parameters, u32);
            int numChars = 0;
            //            if (n < 0) {
            //                n = -n;
            //                numChars++;
            //                print("-", 1);
            //            }

            int temp = n;
            do {
                numChars++;
                temp /= 10;
            } while (temp);

            print(intToString((u32) n), numChars);
        } else if (*format == 'x') {
            format++;
            u32 n = va_arg(parameters, u32);
            int numChars = 0;
            //            if (n < 0) {
            //                n = -n;
            //                numChars++;
            //                print("-", 1);
            //            }

            int temp = n;
            do {
                numChars++;
                temp /= 10;
            } while (temp);

            print(intToHex(n), numChars);
        } else {
            format = format_begun_at;
            u32 len = strlen(format);
            print(format, len);
            format += len;
        }
    }

    va_end(parameters);

    Terminal::instance->putChar('\n');
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
            // TODO: find another way to recognise if parameter is signed or unsigned
            u32 n = va_arg(parameters, u32);
            int numChars = 0;
            //            if (n < 0) {
            //                n = -n;
            //                numChars++;
            //                rawPrintk("-", 1);
            //            }

            int temp = n;
            do {
                numChars++;
                temp /= 10;
            } while (temp);

            rawPrintk(intToString((u32) n), numChars);
        } else if (*format == 'x') {
            // TODO: this may cause page fault when heap is not initialized

            format++;
            u32 n = va_arg(parameters, u32);
            int numChars = 0;
            //            if (n < 0) {
            //                n = -n;
            //                numChars++;
            //                rawPrintk("-", 1);
            //            }

            int temp = n;
            do {
                numChars++;
                temp /= 10;
            } while (temp);

            rawPrintk(intToHex(n), numChars);
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

void hexdump(void* ptr, int buflen) {
    u8* buf = (u8*) ptr;
    int i;
    int j;
    for (i = 0; i < buflen; i += 16) {
        printk("%x: ", i);

        for (j = 0; j < 16; j++) {
            if (i + j < buflen) {
                printk("%x ", buf[i + j]);
            } else {
                printk("   ");
            }
        }

        printk("\n");
    }
}