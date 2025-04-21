# VGA Library Wiki

## Introduction

The VGA library, defined in `vga.h`, provides functions to interact with the VGA text mode in QEMU. This library enables bare-metal programs to display text on the screen, set foreground and background colors, clear the screen, manage cursor positions, and handle special characters. It is designed to work seamlessly with QEMU’s emulated VGA hardware, making it ideal for low-level programming tasks such as operating system development or firmware creation.

## Functions Overview

The VGA library includes the following functions:

- `putc(x, y, fg, bg, c)`: Displays a character at a specific position with specified colors.
- `clear()`: Clears the entire screen with black color.
- `print_char(fg, bg, c)`: Handles special characters and automatically moves the cursor.
- `print(s)`: Displays a string at the current cursor position.
- `show(s)`: An alternate name for `print`.
- `print_i(value)`: Displays an integer at the current cursor position.
- `print_on(line_number, s)`: Displays a string at a specific line without moving the cursor.
- `print_colored(string, textColor, background)`: Prints a colored string starting at the current cursor position.
- `clear_line(line)`: Clears the specified line on the VGA text display.
- `set_cursor(x, y)`: Sets the cursor to the specified coordinates.
- `print_hex(value)`: Prints the hexadecimal representation of a 32-bit value.
- `newline()`: Moves the cursor to the next line.

## Detailed Function Descriptions

### `void putc(u8 x, u8 y, VGA_Color fg, VGA_Color bg, char c)`

- **Description**: Displays the character `c` at the position `(x, y)` with the specified foreground color `fg` and background color `bg`.
- **Parameters**:
  - `x`: The column position (0 to 79).
  - `y`: The row position (0 to 24).
  - `fg`: The foreground color (from `VGA_Color` enum).
  - `bg`: The background color (from `VGA_Color` enum).
  - `c`: The character to display.
- **Returns**: None

### `void clear()`

- **Description**: Clears the entire screen by setting all characters to spaces with black background and foreground colors.
- **Parameters**: None
- **Returns**: None

### `void print_char(VGA_Color fg, VGA_Color bg, char c)`

- **Description**: Prints the character `c` at the current cursor position with the specified colors and handles special characters like newline (`\n`).
- **Parameters**:
  - `fg`: The foreground color.
  - `bg`: The background color.
  - `c`: The character to print.
- **Returns**: None

### `void print(const char *s)`

- **Description**: Prints the null-terminated string `s` starting at the current cursor position.
- **Parameters**:
  - `s`: The string to print.
- **Returns**: None

### `void show(const char *s)`

- **Description**: An alternate name for `print`, prints the string `s` at the current cursor position.
- **Parameters**:
  - `s`: The string to print.
- **Returns**: None

### `void print_i(long int value)`

- **Description**: Prints the integer `value` at the current cursor position.
- **Parameters**:
  - `value`: The integer to print.
- **Returns**: None

### `void print_on(u8 line_number, const char *s)`

- **Description**: Prints the string `s` on the specified line number without moving the cursor.
- **Parameters**:
  - `line_number`: The line number (0 to 24) to print the string.
  - `s`: The string to print.
- **Returns**: None

### `void print_colored(const char *string, VGA_Color textColor, VGA_Color background)`

- **Description**: Prints the string `string` with the specified text and background colors starting at the current cursor position.
- **Parameters**:
  - `string`: The string to print.
  - `textColor`: The foreground color.
  - `background`: The background color.
- **Returns**: None

### `void clear_line(int line)`

- **Description**: Clears the specified line by setting all characters on that line to spaces with black colors.
- **Parameters**:
  - `line`: The line number (0 to 24) to clear.
- **Returns**: None

### `void set_cursor(int x, int y)`

- **Description**: Moves the cursor to the specified position `(x, y)`.
- **Parameters**:
  - `x`: The column position (0 to 79).
  - `y`: The row position (0 to 24).
- **Returns**: None

### `void print_hex(uint32_t value)`

- **Description**: Prints the hexadecimal representation of the 32-bit value `value` at the current cursor position.
- **Parameters**:
  - `value`: The 32-bit value to print in hex.
- **Returns**: None

### `void newline()`

- **Description**: Moves the cursor to the beginning of the next line.
- **Parameters**: None
- **Returns**: None

## Usage Example

Below is a simple example that demonstrates how to use the VGA library to clear the screen, set the cursor, and print a colored string:

```c
#include "vga.h"

int main() {
    clear();
    set_cursor(0, 0);
    print_colored("Hello, World!", COLOR_WHITE, COLOR_BLUE);
    return 0;
}
```

## Tips

- **Screen Dimensions**: The VGA text mode in QEMU is 80 columns by 25 rows. Ensure your coordinates stay within these limits.
- **Color Usage**: Use the `VGA_Color` enum to specify colors for text and background.
- **Cursor Management**: Use `set_cursor` to position the cursor manually, or let functions like `print_char` handle it automatically.
- **QEMU Configuration**: Ensure QEMU is launched with VGA support, such as `-vga std`.