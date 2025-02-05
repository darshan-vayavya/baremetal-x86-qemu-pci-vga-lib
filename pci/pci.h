#ifndef _DSP_PCI_H_
#define _DSP_PCI_H_
#include <stdbool.h>
#include <stdint.h>
#include <vga.h>
#define PCI_CONFIG_ADDRESS_PORT 0x0CF8
#define PCI_CONFIG_DATA_PORT 0x0CFC

#define PCI_VENDOR_ID_OFFSET 0x00
#define PCI_DEVICE_ID_OFFSET 0x02
#define PCI_STATUS_OFFSET 0x04
#define PCI_BAR0_OFFSET 0x10
#define PCI_CAPABILITIES_OFFSET 0x34

#define PCI_MSIX_CAP_OFFSET 0x70
#define MSIX_CAP_ID 0x11
#define PCI_STATUS_CAP_LIST 0x34
#define PCI_MSIX_CONTROL_OFFSET 0x02
#define PCI_MSIX_TABLE_OFFSET 0x04
#define PCI_MSIX_PBA_OFFSET 0x08
#define MSIX_ENABLE (1 << 15)
#define MSIX_FUNCTION_MASK (1 << 14)
// Size of each MSI-X table entry
#define MSIX_TABLE_ENTRY_SIZE 16

#define PCI_MAX_BUSES 256
#define PCI_MAX_DEVICES 32
#define PCI_MAX_FUNCTIONS 8
/* Macro to write to the PCI configuration address port */
#define PCI_CONFIG_ADDRESS(bus, dev, func, offset)                \
    (0x80000000 | ((bus) << 16) | ((dev) << 11) | ((func) << 8) | \
     ((offset) & 0xFC))

/**
 * @brief Writes a 32-bit value to an I/O port.
 * This function uses inline assembly to write a 32-bit value to a specified
 * I/O port, which is typical for interacting with hardware registers in
 * bare-metal systems.
 * @param port The I/O port address to write to.
 * @param value The 32-bit value to write to the port.
 */
static inline void outl(uint16_t port, uint32_t value);

/**
 * @brief Reads a 32-bit value from an I/O port.
 * This function uses inline assembly to read a 32-bit value from a specified
 * I/O port, which is essential for reading values from hardware registers in
 * bare-metal systems.
 * @param port The I/O port address to read from.
 * @return The 32-bit value read from the port.
 */
static inline uint32_t inl(uint16_t port);

/**
 * @brief Reads from the PCI configuration space.
 * This function reads a 32-bit value from a specified PCI device's configuration
 * space. It is used to retrieve data such as the Vendor ID, Device ID, and other
 * device-specific information from the configuration registers.
 * @param address The address of the PCI configuration register to read from.
 * @return The 32-bit value read from the PCI configuration register.
 */
uint32_t pci_read_config(uint32_t address);

/**
 * @brief Writes a 32-bit value to the PCI configuration space.
 * This function writes a 32-bit value to a specified PCI device's configuration
 * space. It is used to configure or modify device-specific parameters like Base
 * Address Registers (BARs).
 * @param address The address of the PCI configuration register to write to.
 * @param value The 32-bit value to write to the PCI configuration register.
 */
void pci_write_config(uint32_t address, uint32_t value);

/**
 * @brief Reads the Vendor ID (VID) of a PCI device.
 * This function reads the Vendor ID from the PCI configuration space of a
 * specific device. The Vendor ID is a unique identifier assigned to the
 * manufacturer of the device.
 * @param bus The bus number of the PCI device.
 * @param device The device number on the bus.
 * @param function The function number of the device.
 * @return The 16-bit Vendor ID of the device.
 */
uint16_t getVID(uint8_t bus, uint8_t device, uint8_t function);

/**
 * @brief Reads the Device ID (DID) of a PCI device.
 * This function reads the Device ID from the PCI configuration space of a
 * specific device. The Device ID uniquely identifies the specific device model
 * from the manufacturer.
 * @param bus The bus number of the PCI device.
 * @param device The device number on the bus.
 * @param function The function number of the device.
 * @return The 16-bit Device ID of the device.
 */
uint16_t getDID(uint8_t bus, uint8_t device, uint8_t function);

/**
 * @brief Reads the Base Address Register 0 (BAR0) of a PCI device.
 * This function reads the value of BAR0 from the PCI configuration space of a
 * specific device. BAR0 is typically used to hold the base address of the
 * device's memory or I/O space.
 * @param bus The bus number of the PCI device.
 * @param device The device number on the bus.
 * @param function The function number of the device.
 * @return The 32-bit value of BAR0.
 */
uint32_t getBAR0(uint8_t bus, uint8_t device, uint8_t function);

/**
 * @brief Initializes the MSI-X Message Control register for the specified PCI
 * device. This function writes the appropriate values to the MSI-X Message
 * Control register to enable MSI-X and set the number of vectors in the MSI-X
 * Table. The MSI-X capability is first checked before enabling the
 * functionality.
 * @param bus        The bus number where the PCI device is located.
 * @param device     The device number of the PCI device.
 * @param function   The function number of the PCI device.
 * @param cap_offset The offset of the MSI-X capability in the PCI configuration
 * space.
 * @param num_vectors The number of MSI-X vectors to initialize (must be > 0).
 */
void initializeMSIXMessageControl(uint8_t bus, uint8_t device, uint8_t function,
                                  uint32_t cap_offset, uint16_t num_vectors);

/**
 * @brief Sets up the MSI-X Pending Bit Array (PBA) for the specified PCI device.
 * This function writes the initial values for the MSI-X Table entries and
 * updates the Pending Bit Array for each interrupt vector.
 * Each Pending Bit is set according to the corresponding entry in the MSI-X
 * Table.
 * @param bus        The bus number where the PCI device is located.
 * @param device     The device number of the PCI device.
 * @param function   The function number of the PCI device.
 * @param table_base The base address of the MSI-X Table.
 * @param pba_base   The base address of the MSI-X Pending Bit Array.
 * @param num_vectors The number of MSI-X vectors to configure.
 */
void setupMSIXPendingArray(uint8_t bus, uint8_t device, uint8_t function,
                           uint32_t table_base, uint32_t pba_base,
                           uint32_t num_vectors);

/**
 * @brief Checks whether the given PCI device supports the MSI-X capability.
 * This function checks the PCI configuration space for the MSI-X capability,
 * and if found, it returns the offset where the MSI-X capability is located.
 * @param bus         The bus number where the PCI device is located.
 * @param device      The device number of the PCI device.
 * @param function    The function number of the PCI device.
 * @param cap_offset  Pointer to store the offset of the MSI-X capability if
 * found.
 * @return true if the PCI device supports MSI-X, false otherwise.
 */
bool checkMSIXCapability(uint8_t bus, uint8_t device, uint8_t function,
                         uint32_t *cap_offset);

/**
 * @brief Sets up the MSI-X Pending Bit Array using DWORD access for the
 * specified PCI device. This function updates the Pending Bit Array with DWORD
 * accesses. Each Pending Bit in the array is set based on the MSI-X Table entry,
 * using 32-bit access for the Pending Bit Array.
 * @param bus         The bus number where the PCI device is located.
 * @param device      The device number of the PCI device.
 * @param function    The function number of the PCI device.
 * @param pba_base    The base address of the MSI-X Pending Bit Array.
 * @param num_vectors The number of MSI-X vectors to configure.
 */
void setupMSIXPendingArrayWithDwordAccess(uint8_t bus, uint8_t device,
                                          uint8_t function, uint32_t pba_base,
                                          uint32_t num_vectors);

/**
 * @brief Enables MSI-X for the specified PCI device and initializes the MSI-X
 * Table and Pending Bit Array. This function checks for the MSI-X capability and
 * then enables MSI-X by writing to the Message Control register. It also sets up
 * the MSI-X Table entries and updates the Pending Bit Array for the specified
 * number of vectors.
 * @param bus        The bus number where the PCI device is located.
 * @param device     The device number of the PCI device.
 * @param function   The function number of the PCI device.
 * @param num_vectors The number of MSI-X vectors to configure and enable.
 */
void enableMSIX(uint8_t bus, uint8_t device, uint8_t function,
                uint32_t num_vectors);

/**
 * @brief Enumerates all PCI devices on the system.
 * This function scans all PCI buses, devices, and functions, and prints
 * information about any valid PCI devices it finds, such as the Vendor ID,
 * Device ID, and the bus/device/function numbers.
 */
void pci_enumerate();

/**
 * @brief Find all the capabilities of the given PCI device
 * @param bus The bus on which the device lives
 * @param device The device to read from
 * @param function the function of the device to read
 */
void print_pci_capabilities(uint8_t bus, uint8_t device, uint8_t function);

/**
 * @brief Prints the capabilities of a PCI device based on its capability id
 * @param cap_id The capability ID
 */
void print_capability_name(uint8_t cap_id);

#endif