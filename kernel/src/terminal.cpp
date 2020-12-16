#include <lib/string.hpp>
#include <terminal.hpp>

Terminal::Terminal() : commandPort(0x3D4), dataPort(0x3D5) {
    instance = this;

    width = 80;
    height = 25;
    x = 0;
    y = 0;
    color = vgaColor(7, 0);
    memory = (u16*) 0xC00B8000;
}

void Terminal::clear() {
    for (u32 i = 0; i < height; i++) {
        for (u32 j = 0; j < width; j++) {
            memory[i * width + j] = vgaEntry(' ', color);
        }
    }
}

void Terminal::putChar(char ch) {
    if (ch == '\n') {
        x = 0;
        y++;
    }

    if (ch >= ' ') {
        memory[y * width + x] = vgaEntry(ch, color);

        x++;
    }

    if (x >= width) {
        x = 0;
        y++;
    }

    scroll();
    updateCursor();
}

void Terminal::write(char* str) {
    auto len = strlen(str);

    for (u32 i = 0; i < len; i++) {
        putChar(str[i]);
    }
}

void Terminal::scroll() {
    if (y >= height) {
        for (auto i = 0; i < width * height; i++) {
            if ((i - width) < 0) {
                continue;
            }

            memory[i - width] = memory[i];
        }

        for (auto i = 0; i < width; i++) {
            memory[(height - 1) * width + i] = vgaEntry(' ', color);
        }

        y--;
        x = 0;
    }
}

void Terminal::updateCursor() {
    auto pos = y * width + x;

    commandPort.write(0x0F);
    dataPort.write(pos & 0xFF);
    commandPort.write(0x0E);
    dataPort.write((pos >> 8) & 0xFF);
}

void Terminal::setColor(u8 foreground, u8 background) {
    color = vgaColor(foreground, background);
}
