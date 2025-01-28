#include "vga.h"

void putc(u8 x, u8 y, VGA_Color fg, VGA_Color bg, char c) {
    if (x >= COLS || y >= ROWS) return;

    u16 value = (bg << 12) | (fg << 8) | c;
    video[y * COLS + x] = value;
}

void clear(VGA_Color bg) {
    cursor_x = 0;
    cursor_y = 0;
    for (u8 y = 0; y < ROWS; y++)
        for (u8 x = 0; x < COLS; x++) putc(x, y, bg, bg, ' ');
}

void print_char(VGA_Color fg, VGA_Color bg, char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\t') {
        cursor_x += 4 - (cursor_x % 4);
    } else {
        putc(cursor_x, cursor_y, fg, bg, c);
        cursor_x++;
    }

    // Handle wrapping and scrolling
    if (cursor_x >= COLS) {
        cursor_x = 0;
        cursor_y++;
    }
    if (cursor_y >= ROWS) {
        cursor_y = ROWS - 1;
        // Scroll up by copying rows up one line
        for (u8 y = 1; y < ROWS; y++) {
            for (u8 x = 0; x < COLS; x++) {
                video[(y - 1) * COLS + x] = video[y * COLS + x];
            }
        }
        // Clear the last row
        for (u8 x = 0; x < COLS; x++) {
            putc(x, ROWS - 1, bg, bg, ' ');
        }
    }
}

void print(const char *s, VGA_Color fg, VGA_Color bg) {
    for (; *s; s++) {
        print_char(fg, bg, *s);
    }
}

void print_i(long int value, VGA_Color fg, VGA_Color bg) {
    char buffer[12];  // Enough to hold "-2147483648\0"
    char *ptr = buffer + sizeof(buffer) - 1;
    *ptr = '\0';
    int is_negative = value < 0;
    if (is_negative) value = -value;

    do {
        *--ptr = '0' + (value % 10);
        value /= 10;
    } while (value);

    if (is_negative) {
        *--ptr = '-';
    }
    print(ptr, fg, bg);
}

void print_on(u8 line_number, const char *s, VGA_Color fg, VGA_Color bg) {
    if (line_number >= ROWS) return;
    cursor_x = 0;
    cursor_y = line_number;
    print(s, fg, bg);
}

void print_colored(const char *string, uint8_t color) {
    for (; *string; string++) {
        if (*string == '\n') {
            cursor_x = 0;
            cursor_y++;
        } else {
            u16 value = (color << 8) | *string;
            video[cursor_y * COLS + cursor_x] = value;
            cursor_x++;
        }

        if (cursor_x >= COLS) {
            cursor_x = 0;
            cursor_y++;
        }

        if (cursor_y >= ROWS) {
            cursor_y = ROWS - 1;
            // Scroll up by moving all rows up
            for (u8 y = 1; y < ROWS; y++) {
                for (u8 x = 0; x < COLS; x++) {
                    video[(y - 1) * COLS + x] = video[y * COLS + x];
                }
            }
            // Clear the last row
            for (u8 x = 0; x < COLS; x++) {
                video[(ROWS - 1) * COLS + x] = (color << 8) | ' ';
            }
        }
    }
}

void clear_line(int line) {
    if (line < 0 || line >= ROWS) return;

    for (int x = 0; x < COLS; x++) {
        video[line * COLS + x] = (0x0 << 12) | ' ';  // Default black background
    }
}

void set_cursor(int x, int y) {
    if (x >= 0 && x < COLS && y >= 0 && y < ROWS) {
        cursor_x = x;
        cursor_y = y;
    }
}

static void print_hex(uint32_t value) {
    char buffer[9];
    for (int i = 7; i >= 0; i--) {
        int nibble = (value >> (i * 4)) & 0xF;
        buffer[7 - i] = nibble < 10 ? ('0' + nibble) : ('A' + (nibble - 10));
    }
    buffer[8] = '\0';
    print(buffer, COLOR_GREEN, COLOR_BLACK);
}

static void newline() { print_char(COLOR_GREEN, COLOR_BLACK, '\n'); }