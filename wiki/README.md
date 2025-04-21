# Bare-metal x86 QEMU PCI VGA Library Wiki

## Introduction

This library provides a set of C functions and interfaces for interacting with emulated PCI devices and VGA text mode in a bare-metal environment, specifically designed for use with QEMU's i386 system emulation (`qemu-system-i386`). It enables developers to write low-level software, such as operating systems or firmware, that can enumerate and configure PCI devices and manipulate VGA text mode within the QEMU emulator. The library is lightweight, written in C, and tailored to work seamlessly with QEMU's emulated hardware.

## Features

- **VGA Text Mode Support**: Functions to display text, set colors, clear the screen, and manage cursor positions.
- **PCI Device Interaction**: Functions to read and write to the PCI configuration space, handle MSI-X interrupts, and enumerate PCI devices.

## Getting Started

1. Clone the repository:

   ```bash
   git clone https://github.com/darshan-vayavya/baremetal-x86-qemu-pci-vga-lib.git
   ```

2. Include the library headers (`vga.h` and `pci.h`) in your project.
3. Link against the library when compiling your code.
4. Use the provided functions to interact with VGA and PCI devices in QEMU.

## Library Documentation

- [VGA Library Wiki](vga.md): Detailed documentation for the VGA text mode library.
- [PCI Library Wiki](pci.md): Detailed documentation for the PCI device interaction library.

## Usage Examples

- **VGA Example**: Clear the screen and print a colored string.

  ```c
  #include "vga.h"

  int main() {
      clear();
      set_cursor(0, 0);
      print_colored("Hello, World!", COLOR_WHITE, COLOR_BLUE);
      return 0;
  }
  ```

- **PCI Example**: Enumerate all PCI devices.

  ```c
  #include "pci.h"

  int main() {
      pci_enumerate();
      return 0;
  }
  ```

## Troubleshooting

- Ensure QEMU is correctly installed and configured for i386 emulation.
- Verify that your compiler is set up to target the i386 architecture.
- Check compatibility with your version of QEMU, as emulation features may vary.
- For VGA text mode, ensure QEMU is launched with VGA support, such as `-vga std`.

## Contributing

- Follow the coding style in the existing code.
- Submit pull requests with clear descriptions of changes.
- Ensure your code compiles and runs correctly in QEMU.

## License

This project is licensed under the [MIT License](https://opensource.org/licenses/MIT).
