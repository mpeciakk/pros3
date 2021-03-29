#ifndef __TERMINAL_HPP
#define __TERMINAL_HPP

#include <Lib/Types.hpp>
#include <Hardware/Port.hpp>
#include <Lib/Singleton.hpp>

#define VGA_COLOR_BLACK 0
#define VGA_COLOR_BLUE 1
#define VGA_COLOR_GREEN 2
#define VGA_COLOR_CYAN 3
#define VGA_COLOR_RED 4
#define VGA_COLOR_MAGENTA 5
#define VGA_COLOR_BROWN 6
#define VGA_COLOR_LIGHT_GREY 7
#define VGA_COLOR_DARK_GREY 8
#define VGA_COLOR_LIGHT_BLUE 9
#define VGA_COLOR_LIGHT_GREEN 10
#define VGA_COLOR_LIGHT_CYAN 11
#define VGA_COLOR_LIGHT_RED 12
#define VGA_COLOR_LIGHT_MAGENTA 13
#define VGA_COLOR_LIGHT_BROWN 14
#define VGA_COLOR_WHITE 15

class Terminal : public Singleton<Terminal> {
public:
    Terminal();
    ~Terminal() = default;

    void clear();
    void putChar(char ch);
    void write(char* str);

    void scroll();
    void updateCursor();

    void setColor(u8 foreground, u8 background);

    static inline u8 vgaColor(u8 foreground, u8 background) {
        return foreground | background << 4;
    }

    static inline u16 vgaEntry(char ch, u8 color) {
        return (u16) ch | (u16) color << 8;
    }

private:
    u8 width;
    u8 height;
    u8 x;
    u8 y;
    u8 color;

    u16* memory;

    Port8Bit commandPort;
    Port8Bit dataPort;
};

#endif