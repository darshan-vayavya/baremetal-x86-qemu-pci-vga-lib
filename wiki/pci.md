# PCI Library Wiki

## Introduction

The PCI library, defined in `pci.h`, provides functions to interact with PCI devices in QEMU. It allows bare-metal programs to read and write to the PCI configuration space, handle MSI-X interrupts, enumerate PCI devices, and manage device capabilities. This library is essential for low-level programming tasks that require direct hardware interaction, such as operating system development or driver creation.

## Functions Overview

The PCI library includes the following functions:

- `outl(port, value)`: Writes a 32-bit value to an I/O port.
- `inl(port)`: Reads a 32-bit value from an I/O port.
- `pci_read_config(address)`: Reads from the PCI configuration space.
- `pci_write_config(address, value)`: Writes to the PCI configuration space.
- `getVID(bus, device, function)`: Reads the Vendor ID of a PCI device.
- `getDID(bus, device, function)`: Reads the Device ID of a PCI device.
- `getBAR0(bus, device, function)`: Reads the Base Address Register 0 of a PCI device.
- `writeMSIXAddress(bus, device, function, cap_offset, entry_index, address)`: Writes to the MSI-X Message Table Address.
- `writeMSIXData(bus, device, function, cap_offset, entry_index, data)`: Writes to the MSI-X Message Table Data.
- `initializeMSIXMessageControl(bus, device, function, cap_offset, num_vectors)`: Initializes the MSI-X Message Control register.
- `setupMSIXPendingArray(bus, device, function, table_base, pba_base, num_vectors)`: Sets up the MSI-X Pending Bit Array.
- `checkMSIXCapability(bus, device, function, cap_offset)`: Checks if the PCI device supports MSI-X.
- `setupMSIXPendingArrayWithDwordAccess(bus, device, function, pba_base, num_vectors)`: Sets up the MSI-X Pending Bit Array using DWORD access.
- `configureMSIXCapability(bus, device, function, cap_offset, tableOffset, pbaOffset)`: Configures the MSI-X Capability Structure.
- `enableMSIX(bus, device, function, num_vectors)`: Enables MSI-X for the specified PCI device.
- `pci_enumerate()`: Enumerates all PCI devices on the system.
- `print_pci_capabilities(bus, device, function)`: Finds and prints all capabilities of the given PCI device.
- `print_capability_name(cap_id)`: Prints the name of a capability based on its ID.

## Detailed Function Descriptions

### `static inline void outl(uint16_t port, uint32_t value)`

- **Description**: Writes a 32-bit value to the specified I/O port using inline assembly.
- **Parameters**:
  - `port`: The I/O port address.
  - `value`: The 32-bit value to write.
- **Returns**: None

### `static inline uint32_t inl(uint16_t port)`

- **Description**: Reads a 32-bit value from the specified I/O port using inline assembly.
- **Parameters**:
  - `port`: The I/O port address.
- **Returns**: The 32-bit value read from the port.

### `uint32_t pci_read_config(uint32_t address)`

- **Description**: Reads a 32-bit value from the specified PCI configuration register.
- **Parameters**:
  - `address`: The address of the PCI configuration register.
- **Returns**: The 32-bit value read from the register.

### `void pci_write_config(uint32_t address, uint32_t value)`

- **Description**: Writes a 32-bit value to the specified PCI configuration register.
- **Parameters**:
  - `address`: The address of the PCI configuration register.
  - `value`: The 32-bit value to write.
- **Returns**: None

### `uint16_t getVID(uint8_t bus, uint8_t device, uint8_t function)`

- **Description**: Reads the Vendor ID from the PCI configuration space of the specified device.
- **Parameters**:
  - `bus`: The bus number.
  - `device`: The device number.
  - `function`: The function number.
- **Returns**: The 16-bit Vendor ID.

### `uint16_t getDID(uint8_t bus, uint8_t device, uint8_t function)`

- **Description**: Reads the Device ID from the PCI configuration space of the specified device.
- **Parameters**:
  - `bus`: The bus number.
  - `device`: The device number.
  - `function`: The function number.
- **Returns**: The 16-bit Device ID.

### `uint32_t getBAR0(uint8_t bus, uint8_t device, uint8_t function)`

- **Description**: Reads the Base Address Register 0 (BAR0) from the PCI configuration space.
- **Parameters**:
  - `bus`: The bus number.
  - `device`: The device number.
  - `function`: The function number.
- **Returns**: The 32-bit value of BAR0.

### `void writeMSIXAddress(uint8_t bus, uint8_t device, uint8_t function, uint32_t cap_offset, uint32_t entry_index, uint64_t address)`

- **Description**: Writes the specified address to the MSI-X Message Table entry at the given index.
- **Parameters**:
  - `bus`: The bus number.
  - `device`: The device number.
  - `function`: The function number.
  - `cap_offset`: The offset of the MSI-X capability structure.
  - `entry_index`: The index of the MSI-X Table entry.
  - `address`: The 64-bit address to write.
- **Returns**: None

### `void writeMSIXData(uint8_t bus, uint8_t device, uint8_t function, uint32_t cap_offset, uint32_t entry_index, uint32_t data)`

- **Description**: Writes the specified data to the MSI-X Message Table entry at the given index.
- **Parameters**:
  - `bus`: The bus number.
  - `device`: The device number.
  - `function`: The function number.
  - `cap_offset`: The offset of the MSI-X capability structure.
  - `entry_index`: The index of the MSI-X Table entry.
  - `data`: The 32-bit data to write.
- **Returns**: None

### `void initializeMSIXMessageControl(uint8_t bus, uint8_t device, uint8_t function, uint32_t cap_offset, uint16_t num_vectors)`

- **Description**: Initializes the MSI-X Message Control register to enable MSI-X and set the number of vectors.
- **Parameters**:
  - `bus`: The bus number.
  - `device`: The device number.
  - `function`: The function number.
  - `cap_offset`: The offset of the MSI-X capability structure.
  - `num_vectors`: The number of MSI-X vectors to initialize.
- **Returns**: None

### `void setupMSIXPendingArray(uint8_t bus, uint8_t device, uint8_t function, uint32_t table_base, uint32_t pba_base, uint32_t num_vectors)`

- **Description**: Sets up the MSI-X Pending Bit Array for the specified device.
- **Parameters**:
  - `bus`: The bus number.
  - `device`: The device number.
  - `function`: The function number.
  - `table_base`: The base address of the MSI-X Table.
  - `pba_base`: The base address of the MSI-X Pending Bit Array.
  - `num_vectors`: The number of MSI-X vectors.
- **Returns**: None

### `bool checkMSIXCapability(uint8_t bus, uint8_t device, uint8_t function, uint32_t *cap_offset)`

- **Description**: Checks if the PCI device supports MSI-X and retrieves the capability offset.
- **Parameters**:
  - `bus`: The bus number.
  - `device`: The device number.
  - `function`: The function number.
  - `cap_offset`: Pointer to store the offset of the MSI-X capability.
- **Returns**: `true` if MSI-X is supported, `false` otherwise.

### `void setupMSIXPendingArrayWithDwordAccess(uint8_t bus, uint8_t device, uint8_t function, uint32_t pba_base, uint32_t num_vectors)`

- **Description**: Sets up the MSI-X Pending Bit Array using DWORD access.
- **Parameters**:
  - `bus`: The bus number.
  - `device`: The device number.
  - `function`: The function number.
  - `pba_base`: The base address of the MSI-X Pending Bit Array.
  - `num_vectors`: The number of MSI-X vectors.
- **Returns**: None

### `void configureMSIXCapability(uint8_t bus, uint8_t device, uint8_t function, uint32_t cap_offset, uint64_t tableOffset, uint64_t pbaOffset)`

- **Description**: Configures the MSI-X capability structure with the provided table and PBA offsets.
- **Parameters**:
  - `bus`: The bus number.
  - `device`: The device number.
  - `function`: The function number.
  - `cap_offset`: The offset of the MSI-X capability structure.
  - `tableOffset`: The address of the MSI-X Message Table.
  - `pbaOffset`: The address of the MSI-X Pending Bit Array.
- **Returns**: None

### `void enableMSIX(uint8_t bus, uint8_t device, uint8_t function, uint32_t num_vectors)`

- **Description**: Enables MSI-X for the specified PCI device and configures the specified number of vectors.
- **Parameters**:
  - `bus`: The bus number.
  - `device`: The device number.
  - `function`: The function number.
  - `num_vectors`: The number of MSI-X vectors to enable.
- **Returns**: None

### `void pci_enumerate()`

- **Description**: Scans all PCI buses, devices, and functions, and prints information about valid PCI devices.
- **Parameters**: None
- **Returns**: None

### `void print_pci_capabilities(uint8_t bus, uint8_t device, uint8_t function)`

- **Description**: Finds and prints all capabilities of the specified PCI device.
- **Parameters**:
  - `bus`: The bus number.
  - `device`: The device number.
  - `function`: The function number.
- **Returns**: None

### `void print_capability_name(uint8_t cap_id)`

- **Description**: Prints the name of the capability based on its ID.
- **Parameters**:
  - `cap_id`: The capability ID.
- **Returns**: None

## Usage Example

Below is a simple example that demonstrates how to enumerate all PCI devices and print their Vendor and Device IDs:

```c
#include "pci.h"

int main() {
    pci_enumerate();
    return 0;
}
```

## Tips

- **PCI Configuration**: Ensure that the PCI configuration address and data ports are correctly defined for QEMU (typically `0xCF8` and `0xCFC`).
- **Device Limits**: PCI supports up to 256 buses, 32 devices per bus, and 8 functions per device.
- **MSI-X Handling**: Use the provided functions to properly configure and enable MSI-X interrupts for supported devices.
- **QEMU Configuration**: Launch QEMU with appropriate options to emulate PCI devices, such as `-device` for specific hardware.