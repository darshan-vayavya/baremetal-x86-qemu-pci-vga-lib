# VGA Bare-metal x86 QEMU APIs

These APIs are useful in directly interacting your bare-metal code with QEMU's VGA display system connected on PCI bus.

The APIs are written in C and can be used with [x86 QEMU Bare-Metal Toolkit](https://gitlab.vayavyalabs.com:8000/thisisthedarshan/x86-QEMU-Bare-Metal) to build and run x86 Bare-metal binaries.

> TLDR; Jump to the [Function Definitions](#function-definitions) to get started

## Basic overview of VGA

### 1. **What is VGA?**

VGA stands for **Video Graphics Array**, which is a display standard that was introduced by IBM in 1987. It defines the resolution, color depth, and method of displaying images on a screen. VGA was one of the most popular display standards for computer monitors before the advent of newer technologies like SVGA, XGA, and beyond.

VGA operates using a **raster graphics** display system, meaning that it represents images as a grid of pixels, where each pixel corresponds to a single point on the screen.

### 2. **How VGA Display Works**

A VGA display works by **scanning through rows and columns** of pixels, rendering an image based on the data provided by a computer. The VGA controller sends signals to the monitor, telling it which pixel to turn on or off, and what color to display.

- **Horizontal and Vertical Synchronization (Sync):** The screen is divided into a grid, and the display system scans it line by line (horizontally) and frame by frame (vertically). The sync signals tell the monitor when to start a new row or when to start a new frame.
  
- **Resolution:** VGA supports several resolutions, but the most common is 640x480 pixels. Higher resolutions (like 800x600, 1024x768) require more advanced technologies (SVGA, for example).

- **Refresh Rate:** VGA typically runs at 60 Hz, meaning the screen is refreshed 60 times per second. During each refresh cycle, the monitor scans the entire grid of pixels to display the new image.

### 3. **16-bit Color and Characters in VGA**

- **Color Depth (16-bit Color):** VGA typically supports 8-bit color (256 colors) in its basic configuration. However, with extended modes, it can support 16-bit color, which allows for 65,536 different colors. In 16-bit color, each pixel is represented by 16 bits: 5 bits for red, 5 bits for green, and 5 bits for blue, and 1 bit for unused data (often used for transparency or other options).

- **Characters:** VGA doesn't just display images; it can also display characters (text). Text mode in VGA uses a font of fixed-width characters (like 8x8 pixels per character) and is typically organized in a 25-line by 80-column grid on the screen. Each character is stored in video memory as a specific value corresponding to a character in the ASCII or extended character set.

### 4. **How VGA Controller Interprets Rows-Columns**

The VGA controller interprets the screen as a grid of rows and columns (pixels or characters depending on the mode). Here’s how it works:

- **Pixel-based Graphics Mode:** In graphics mode, each pixel is addressed individually in memory. The VGA controller uses memory locations to map out the position of each pixel on the screen, and when the memory is updated, the display changes.
  
  The video memory is typically divided into **frame buffers** where each pixel's color is stored. The address of each pixel is calculated based on its row and column, and the VGA controller fetches the corresponding color data to render on the screen.

- **Text Mode:** In text mode, each character in the screen is stored in memory as a reference to the font set. If you write the ASCII value of a character at a particular memory location, the controller will draw that character at the corresponding row and column.

### 5. **What is the Programming Aspect of VGA?**

When programming for VGA, you're essentially interacting with its **video memory** and **VGA controller**. Here's an overview of how to write to rows and columns using the VGA controller:

- **Video Memory:** VGA typically uses a block of memory (often called **framebuffer** or **video memory**) where each pixel’s data is stored. For text mode, each memory location represents one character (in 2 bytes, one for the character and one for its attributes), while in graphics mode, each location holds the color data for a pixel.

  For example, in 640x480 graphics mode, you have 640 x 480 pixels. Each pixel can have a value representing its color. For 256-color mode, you'd store a single byte (0-255) for each pixel, which maps to a color in the VGA palette.

- **Direct Memory Access (DMA):** The CPU interacts with the VGA controller by writing data to video memory. Writing to a specific row/column is done by addressing the appropriate memory locations for that pixel. The controller reads from these locations to draw the screen.

  In text mode, writing to a specific character cell is done by placing the ASCII value of the character into a specific memory location. For example, to display the letter 'A' at row 1, column 1, you'd place the value of 'A' (ASCII 65) in the memory location corresponding to that position.

- **Example Programming Steps:**
  1. **Set the video mode:** Set the VGA controller to a specific video mode, like 320x200 pixels (graphics mode) or 80x25 characters (text mode).
  2. **Access memory:** Write data to the video memory or frame buffer.
  3. **Directly manipulate pixels:** If you're in graphics mode, you can calculate the memory address of a pixel using the formula: `pixel_address = (y * width) + x` where `x` is the column and `y` is the row.
  4. **Modify color data:** For each pixel, store the color value in the corresponding memory location.

---
Good resource:

1. [Ben Eater - Video Card Video on YouTube™️](https://www.youtube.com/watch?v=l7rce6IQDWs) ***Best one in my opinion***
2. [VGA Basics - Milwaukee School of Engineering](https://faculty-web.msoe.edu/johnsontimoj/EE3921/files3921/vga_basics.pdf)
3. [VGA Display Controller - DIGILENT™️](https://digilent.com/reference/learn/programmable-logic/tutorials/vga-display-congroller/start)
4. [TinyVGA - Microcontroller VGA Projects](http://www.tinyvga.com/)

## **Function Definitions**

---

### **VGA Debugging Functions**

1. **`print`**  
   Displays a string at the current cursor position with specified foreground and background colors. Automatically handles special characters such as newline (`\n`) and tab (`\t`).  
   **Prototype:**  

   ```c
   void print(const char *s);
   ```

2. **`print_i`**  
   Displays an integer at the current cursor position with specified foreground and background colors.  
   **Prototype:**  

   ```c
   void print_i(long int value);
   ```

3. **`print_on`**  
   Writes a string to a specific line in VGA text mode without moving the cursor.  
   **Prototype:**  

   ```c
   void print_on(u8 line_number, const char *s);
   ```

4. **`set_cursor`**  
   Sets the VGA cursor to a specific (x, y) position.  
   **Prototype:**  

   ```c
   void set_cursor(int x, int y);
   ```

5. **`clear`**  
   Clears the entire VGA text mode screen and fills it with the specified background color.  
   **Prototype:**  

   ```c
   void clear(VGA_Color bg);
   ```

6. **`clear_line`**  
   Clears a specific line on the VGA screen by filling it with spaces and the current background color.  
   **Prototype:**  

   ```c
   void clear_line(int line);
   ```

7. **`print_colored`**  
   Prints a string with a combined 8-bit VGA color for foreground and background starting at the current cursor position.  
   **Prototype:**  

   ```c
   void print_colored(const char *string, uint8_t color);
   ```

8. **`print_hex`**
   Prints a hex equivalent of a 32-bit number
   **Prototype:**

   ```c
   static void print_hex(uint32_t value);
   ```

9. **`newline`**
   Increments the line number.
   **Prototype:**

   ```c
   static void newline();
   ```

---

### **Color Encoding for VGA Text Mode**

In VGA text mode, each character is represented by two bytes:

- **1st byte:** ASCII character to display.
- **2nd byte:** 8-bit color attribute:
  - **Foreground color:** Lower 4 bits.
  - **Background color:** Upper 4 bits.

---

### **VGA Color Table**

| Color Name         | Value | Example Display |
|--------------------|-------|------------------|
| Black              | 0x   | <span style="color:#000000">Dark (no color)</span> |
| Blue               | 0x   | <span style="color:#000001">Deep Blue      </span> |
| Green              | 0x   | <span style="color:#000002">Medium Green   </span> |
| Cyan (Light Blue)  | 0x   | <span style="color:#000003">Aqua/Teal      </span> |
| Red                | 0x   | <span style="color:#000004">Crimson        </span> |
| Magenta (Purple)   | 0x   | <span style="color:#000005">Pink/Purple    </span> |
| Brown (Dark Yellow)| 0x   | <span style="color:#000006">Brownish Tone  </span> |
| Light Gray         | 0x   | <span style="color:#000007">Soft White     </span> |
| Dark Gray          | 0x   | <span style="color:#000008">Dull Gray      </span> |
| Light Blue         | 0x   | <span style="color:#000009">Brighter Blue  </span> |
| Light Green        | 0x   | <span style="color:#00000A">Lime Green     </span> |
| Light Cyan         | 0x   | <span style="color:#00000B">Sky Blue       </span> |
| Light Red          | 0x   | <span style="color:#00000C">Bright Red     </span> |
| Light Magenta      | 0x   | <span style="color:#00000D">Fuchsia Pink   </span> |
| Yellow             | 0x   | <span style="color:#00000E">Bright Yellow  </span> |
| White              | 0x   | <span style="color:#00000F">Bright White   </span> |

---

### **Valid ENUM Values**

```c
COLOR_BLACK, COLOR_BLUE, COLOR_GREEN, COLOR_CYAN, COLOR_RED,
COLOR_MAGENTA, COLOR_BROWN, COLOR_LIGHT_GRAY, COLOR_DARK_GRAY,
COLOR_LIGHT_BLUE, COLOR_LIGHT_GREEN, COLOR_LIGHT_CYAN, COLOR_LIGHT_RED,
COLOR_LIGHT_MAGENTA, COLOR_YELLOW, COLOR_WHITE
```
