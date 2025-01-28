#include "pci.h"

#define PCI_CONFIG_ADDRESS_PORT 0x0CF8
#define PCI_CONFIG_DATA_PORT 0x0CFC

#define PCI_VENDOR_ID_OFFSET 0x00
#define PCI_DEVICE_ID_OFFSET 0x02
#define PCI_BAR0_OFFSET 0x10
#define PCI_MSIX_CAP_OFFSET 0x70

#define PCI_MAX_BUSES 256
#define PCI_MAX_DEVICES 32
#define PCI_MAX_FUNCTIONS 8

void outl(uint16_t port, uint32_t value) {
    __asm__ volatile("outl %0, %1"
                     :  // No output operands
                     : "a"(value), "Nd"(port));
}

uint32_t inl(uint16_t port) {
    uint32_t value;
    __asm__ volatile("inl %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

uint32_t pci_read_config(uint32_t address) {
    outl(PCI_CONFIG_ADDRESS_PORT, address);
    return inl(PCI_CONFIG_DATA_PORT);
}

void pci_write_config(uint32_t address, uint32_t value) {
    outl(PCI_CONFIG_ADDRESS_PORT, address);
    outl(PCI_CONFIG_DATA_PORT, value);
}

uint16_t getVID(uint8_t bus, uint8_t device, uint8_t function) {
    uint32_t address =
        PCI_CONFIG_ADDRESS(bus, device, function, PCI_VENDOR_ID_OFFSET);
    return pci_read_config(address) & 0xFFFF;
}

uint16_t getDID(uint8_t bus, uint8_t device, uint8_t function) {
    uint32_t address =
        PCI_CONFIG_ADDRESS(bus, device, function, PCI_DEVICE_ID_OFFSET);
    return (pci_read_config(address) >> 16) & 0xFFFF;
}

uint32_t getBAR0(uint8_t bus, uint8_t device, uint8_t function) {
    uint32_t address =
        PCI_CONFIG_ADDRESS(bus, device, function, PCI_BAR0_OFFSET);
    return pci_read_config(address);
}

void writeMSIXAddress(uint8_t bus, uint8_t device, uint8_t function,
                      uint64_t address) {
    uint32_t lower = (uint32_t)(address & 0xFFFFFFFF);
    uint32_t upper = (uint32_t)((address >> 32) & 0xFFFFFFFF);

    pci_write_config(
        PCI_CONFIG_ADDRESS(bus, device, function, PCI_MSIX_CAP_OFFSET), lower);
    pci_write_config(
        PCI_CONFIG_ADDRESS(bus, device, function, PCI_MSIX_CAP_OFFSET + 4),
        upper);
}

void writeMSIXData(uint8_t bus, uint8_t device, uint8_t function,
                   uint32_t data) {
    pci_write_config(
        PCI_CONFIG_ADDRESS(bus, device, function, PCI_MSIX_CAP_OFFSET + 8),
        data);
}

void configureMSIXCapability(uint8_t bus, uint8_t device, uint8_t function,
                             uint64_t tableOffset, uint64_t pbaOffset) {
    writeMSIXAddress(bus, device, function, tableOffset);
    writeMSIXAddress(bus, device, function, pbaOffset);
}

void initMSIXMessageControl(uint8_t bus, uint8_t device, uint8_t function) {
    uint32_t messageControl = 0x8000;  // Enable MSI-X
    pci_write_config(
        PCI_CONFIG_ADDRESS(bus, device, function, PCI_MSIX_CAP_OFFSET + 4),
        messageControl);
}

void initializeMSIXMessageControl(uint8_t bus, uint8_t device,
                                  uint8_t function) {
    uint32_t address =
        PCI_CONFIG_ADDRESS(bus, device, function, PCI_MSIX_CAP_OFFSET);
    uint32_t message_control = pci_read_config(address);
    message_control |= (1 << 0);  // Enable MSI-X
    pci_write_config(address, message_control);
}

void setupMSIXPendingArray(uint8_t bus, uint8_t device, uint8_t function,
                           uint64_t address, uint32_t size) {
    writeMSIXAddress(bus, device, function, address);
    writeMSIXData(bus, device, function, size);
}

void enableMSIX(uint8_t bus, uint8_t device, uint8_t function) {
    initializeMSIXMessageControl(bus, device, function);
}

void initializeInterruptModeration(uint8_t bus, uint8_t device, uint8_t function,
                                   uint32_t interval) {
    // Write to an imaginary Interrupt Moderation Register (IMR)
    pci_write_config(PCI_CONFIG_ADDRESS(bus, device, function, 0xA0), interval);
}

void pci_enumerate() {
    print_colored("Enumerating PCI Devices...",
                  (COLOR_YELLOW << 8) | COLOR_BLACK);
    newline();

    for (uint32_t bus = 0; bus < PCI_MAX_BUSES; bus++) {
        for (uint32_t device = 0; device < PCI_MAX_DEVICES; device++) {
            for (uint32_t function = 0; function < PCI_MAX_FUNCTIONS;
                 function++) {
                uint32_t address = PCI_CONFIG_ADDRESS(bus, device, function,
                                                      PCI_VENDOR_ID_OFFSET);
                uint32_t data = pci_read_config(address);
                uint16_t vendor_id = data & 0xFFFF;

                if (vendor_id != 0xFFFF) {  // Valid device
                    uint16_t device_id = (data >> 16) & 0xFFFF;

                    print_colored("Found PCI Device: Bus ",
                                  (COLOR_GREEN << 8) | COLOR_BLACK);
                    print_i(bus);
                    print_colored(" Device ", (COLOR_GREEN << 8) | COLOR_BLACK);
                    print_i(device);
                    print_colored(" Function ",
                                  (COLOR_GREEN << 8) | COLOR_BLACK);
                    print_i(function);
                    print_colored(" - Vendor ID: 0x",
                                  (COLOR_WHITE << 8) | COLOR_BLACK);
                    print_hex(vendor_id);
                    print_colored(" Device ID: 0x",
                                  (COLOR_WHITE << 8) | COLOR_BLACK);
                    print_hex(device_id);
                    newline();
                }
            }
        }
    }
}