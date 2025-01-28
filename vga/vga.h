/**
 * @file vga.h
 * @author Darshan(@thisisthedarshan) <darshanp@vayavyalabs.com>
 * Released under MIT License
 * You should have received a copy of the MIT License along with this program.
 * If not, see <https://opensource.org/licenses/MIT>.
 * @details This Library is useful with bare-metal c code to use VGA on QEMU.
 * More details about the working of this library can be found on the wiki:
 * <https://gitlab.vayavyalabs.com:8000/thisisthedarshan/baremetal-x86-qemu-pci-vga-lib/-/wikis/home>
 */
#ifndef _DSP_VGA_H_
#define _DSP_VGA_H_

#include <stdint.h>

/* Define VGA Constants */
#define COLS (80)
#define ROWS (25)
#define VGA_BASE (0xB8000)  // VGA MMIO Base Address in QEMU

/* Custom typedef for data types */
typedef uint8_t u8;
typedef uint16_t u16;

u16 *const video = (u16 *)VGA_BASE;

/* The ENUM containing 8-bit color. VGA uses 16-bit color, 8-bit for foreground
 * and background each */
typedef enum {
    COLOR_BLACK = 0x0,
    COLOR_BLUE = 0x1,
    COLOR_GREEN = 0x2,
    COLOR_CYAN = 0x3,
    COLOR_RED = 0x4,
    COLOR_MAGENTA = 0x5,
    COLOR_BROWN = 0x6,
    COLOR_LIGHT_GRAY = 0x7,
    COLOR_DARK_GRAY = 0x8,
    COLOR_LIGHT_BLUE = 0x9,
    COLOR_LIGHT_GREEN = 0xA,
    COLOR_LIGHT_CYAN = 0xB,
    COLOR_LIGHT_RED = 0xC,
    COLOR_LIGHT_MAGENTA = 0xD,
    COLOR_YELLOW = 0xE,
    COLOR_WHITE = 0xF
} VGA_Color;

/* Keeps track of the current cursor position */
static u8 cursor_x = 0;
static u8 cursor_y = 0;

/* Main functions */

/**
 * @brief Display a character at cursor position with specified colors
 * @param x X position to put character at
 * @param y Y position to put character at
 * @param fg The foreground color to use
 * @param fg The background color to use
 * @param c The character to print
 */
void putc(u8 x, u8 y, VGA_Color fg, VGA_Color bg, char c);

/**
 * @brief Clear the screen with a specified background color
 * @param bg The background color to use/set
 */
void clear(VGA_Color bg);

/**
 * @brief Handle special characters and move the cursor automatically
 * @param fg The foreground color of the character
 * @param bg The background color of the character
 * @param c The character to print
 */
void print_char(VGA_Color fg, VGA_Color bg, char c);

/**
 * @brief Display a string at the current cursor position
 * @param s The string to print
 * @param fg The foreground color of the character
 * @param bg The background color of the character
 */
void print(const char *s, VGA_Color fg, VGA_Color bg);

/**
 * @brief Display an integer at the current cursor position
 * @param value The value to print
 * @param fg The foreground color of the character
 * @param bg The background color of the character
 */
void print_i(long int value, VGA_Color fg, VGA_Color bg);

/**
 * @brief Display a string at a specific line without moving the cursor
 * @param line_number The line number to put the text on
 * @param s The string message to print
 * @param fg The foreground color of the character
 * @param bg The background color of the character
 */
void print_on(u8 line_number, const char *s, VGA_Color fg, VGA_Color bg);

/**
 * @brief Prints a colored string starting at the current cursor position.
 * @param string Pointer to the null-terminated string to be displayed.
 * @param color 8-bit VGA color code combining foreground and background colors.
 */
void print_colored(const char *string, uint8_t color);

/**
 * @brief Clears the specified line on the VGA text display.
 * @param line Line number to clear (0 to 24).
 */
void clear_line(int line);

/**
 * @brief Sets the cursor to the specified coordinates on the VGA display.
 * @param x Horizontal coordinate (0 to 79).
 * @param y Vertical coordinate (0 to 24).
 */
void set_cursor(int x, int y);

#endif