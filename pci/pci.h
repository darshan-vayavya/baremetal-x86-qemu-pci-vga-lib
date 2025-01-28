#ifndef _DSP_PCI_H_
#define _DSP_PCI_H_
#include <stdint.h>

#include "vga.h"

#define PCI_CONFIG_ADDRESS_PORT 0x0CF8
#define PCI_CONFIG_DATA_PORT 0x0CFC

#define PCI_VENDOR_ID_OFFSET 0x00
#define PCI_DEVICE_ID_OFFSET 0x02
#define PCI_BAR0_OFFSET 0x10
#define PCI_MSIX_CAP_OFFSET 0x70

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
void outl(uint16_t port, uint32_t value);

/**
 * @brief Reads a 32-bit value from an I/O port.
 * This function uses inline assembly to read a 32-bit value from a specified
 * I/O port, which is essential for reading values from hardware registers in
 * bare-metal systems.
 * @param port The I/O port address to read from.
 * @return The 32-bit value read from the port.
 */
uint32_t inl(uint16_t port);

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
 * @brief Writes the MSI-X Message Table Address to a PCI device.
 * This function writes a 64-bit address to the MSI-X Message Table Address field
 * of the device's MSI-X capability structure.
 * @param bus The bus number of the PCI device.
 * @param device The device number on the bus.
 * @param function The function number of the device.
 * @param address The 64-bit address to write to the MSI-X Message Table Address.
 */
void writeMSIXAddress(uint8_t bus, uint8_t device, uint8_t function,
                      uint64_t address);

/**
 * @brief Writes the MSI-X Message Table Data to a PCI device.
 * This function writes a 32-bit value to the MSI-X Message Table Data field of
 * the device's MSI-X capability structure.
 * @param bus The bus number of the PCI device.
 * @param device The device number on the bus.
 * @param function The function number of the device.
 * @param data The 32-bit data value to write to the MSI-X Message Table Data.
 */
void writeMSIXData(uint8_t bus, uint8_t device, uint8_t function, uint32_t data);

/**
 * @brief Configures the MSI-X Capability Structure by setting the Table Offset
 * and PBA Offset.
 * This function writes the Table and PBA offsets into the MSI-X Capability
 * Structure for the device, which is used for interrupt handling in the PCI
 * device.
 * @param bus The bus number of the PCI device.
 * @param device The device number on the bus.
 * @param function The function number of the device.
 * @param tableOffset The offset for the MSI-X Table.
 * @param pbaOffset The offset for the Pending Bit Array.
 */
void configureMSIXCapability(uint8_t bus, uint8_t device, uint8_t function,
                             uint64_t tableOffset, uint64_t pbaOffset);

/**
 * @brief Initializes the Message Control register of the MSI-X Capability
 * Structure.
 * This function enables MSI-X in the device by setting the MSI-X Enable bit in
 * the Message Control register.
 * @param bus The bus number of the PCI device.
 * @param device The device number on the bus.
 * @param function The function number of the device.
 */
void initMSIXMessageControl(uint8_t bus, uint8_t device, uint8_t function);

/**
 * @brief Initializes the Message Control register of the MSI-X Capability
 * Structure.
 * This function enables MSI-X in the device by setting the MSI-X Enable bit in
 * the Message Control register.
 * @param bus The bus number of the PCI device.
 * @param device The device number on the bus.
 * @param function The function number of the device.
 */
void initializeMSIXMessageControl(uint8_t bus, uint8_t device, uint8_t function);

/**
 * @brief Sets up the MSI-X Pending Array for a PCI device.
 * This function writes the base address and size of the Pending Array into the
 * MSI-X capability structure.
 * @param bus The bus number of the PCI device.
 * @param device The device number on the bus.
 * @param function The function number of the device.
 * @param address The base address for the Pending Array.
 * @param size The size of the Pending Array.
 */
void setupMSIXPendingArray(uint8_t bus, uint8_t device, uint8_t function,
                           uint64_t address, uint32_t size);

/**
 * @brief Enables MSI-X for a PCI device.
 * This function initializes the MSI-X Message Control register to enable MSI-X
 * interrupts for the device.
 * @param bus The bus number of the PCI device.
 * @param device The device number on the bus.
 * @param function The function number of the device.
 */
void enableMSIX(uint8_t bus, uint8_t device, uint8_t function);

/**
 * @brief Initializes the Interrupt Moderation register for a PCI device.
 * This function writes an interval value to an imaginary Interrupt Moderation
 * Register (IMR) of the device. This register controls the rate at which
 * interrupts are generated for the device.
 * @param bus The bus number of the PCI device.
 * @param device The device number on the bus.
 * @param function The function number of the device.
 * @param interval The interval for interrupt moderation.
 */
void initializeInterruptModeration(uint8_t bus, uint8_t device, uint8_t function,
                                   uint32_t interval);

/**
 * @brief Enumerates all PCI devices on the system.
 * This function scans all PCI buses, devices, and functions, and prints
 * information about any valid PCI devices it finds, such as the Vendor ID,
 * Device ID, and the bus/device/function numbers.
 */
void pci_enumerate();

#endif