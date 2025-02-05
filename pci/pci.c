#include <pci.h>
#include <vga.h>

static inline void outl(uint16_t port, uint32_t value) {
    __asm__ volatile(
        "movw %w1, %%dx\n\t"  // Move port to DX
        "movl %0, %%eax\n\t"  // Move value to EAX
        "outl %%eax, %%dx"    // Output EAX to DX port
        :
        : "r"(value), "r"(port)
        : "%eax", "%dx");
}

static inline uint32_t inl(uint16_t port) {
    uint32_t value;
    __asm__ volatile(
        "movw %w1, %%dx\n\t"   // Move port to DX
        "inl %%dx, %%eax\n\t"  // Input from DX port to EAX
        "movl %%eax, %0"       // Move EAX to output variable
        : "=r"(value)
        : "r"(port)
        : "%eax", "%dx");
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
                      uint32_t cap_offset, uint32_t entry_index,
                      uint64_t address) {
    uint32_t lower = (uint32_t)(address & 0xFFFFFFFF);
    uint32_t upper = (uint32_t)((address >> 32) & 0xFFFFFFFF);
    uint32_t entry_offset = cap_offset + MSIX_TABLE_ENTRY_SIZE * entry_index;

    pci_write_config(PCI_CONFIG_ADDRESS(bus, device, function, entry_offset),
                     lower);
    pci_write_config(PCI_CONFIG_ADDRESS(bus, device, function, entry_offset + 4),
                     upper);
}

void writeMSIXData(uint8_t bus, uint8_t device, uint8_t function,
                   uint32_t cap_offset, uint32_t entry_index, uint32_t data) {
    uint32_t entry_offset = cap_offset + MSIX_TABLE_ENTRY_SIZE * entry_index + 8;
    pci_write_config(PCI_CONFIG_ADDRESS(bus, device, function, entry_offset),
                     data);
}

bool checkMSIXCapability(uint8_t bus, uint8_t device, uint8_t function,
                         uint32_t *cap_offset) {
    uint32_t status =
        pci_read_config(PCI_CONFIG_ADDRESS(bus, device, function, 0x06));
    if (!(status & (1 << 4))) {
        return false;
    }

    uint32_t cap_ptr = pci_read_config(PCI_CONFIG_ADDRESS(bus, device, function,
                                                          PCI_STATUS_CAP_LIST)) &
                       0xFF;
    while (cap_ptr) {
        uint32_t cap_id =
            pci_read_config(PCI_CONFIG_ADDRESS(bus, device, function, cap_ptr)) &
            0xFF;
        if (cap_id == MSIX_CAP_ID) {
            *cap_offset = cap_ptr;
            return true;
        }
        cap_ptr = (pci_read_config(
                       PCI_CONFIG_ADDRESS(bus, device, function, cap_ptr + 1)) &
                   0xFF);
    }
    return false;
}

void initializeMSIXMessageControl(uint8_t bus, uint8_t device, uint8_t function,
                                  uint32_t cap_offset, uint16_t num_vectors) {
    uint32_t address = PCI_CONFIG_ADDRESS(bus, device, function,
                                          cap_offset + PCI_MSIX_CONTROL_OFFSET);
    uint32_t message_control = pci_read_config(address);

    // Set Table Size and Enable MSI-X
    message_control |= ((num_vectors - 1) & 0x7FF);  // Table Size (bits 10:0)
    message_control |= MSIX_ENABLE;
    pci_write_config(address, message_control);
}

void setupMSIXPendingArray(uint8_t bus, uint8_t device, uint8_t function,
                           uint32_t table_base, uint32_t pba_base,
                           uint32_t num_vectors) {
    for (uint32_t k = 0; k < num_vectors; k++) {
        uint64_t entry_address = table_base + (k * MSIX_TABLE_ENTRY_SIZE);
        uint64_t pba_qword_address = pba_base + (k / 64) * 8;
        uint32_t qword_bit = k % 64;

        // Example: Write the Message Address and Data for the table entry
        pci_write_config(entry_address,
                         (0x80000000 + k));  // Address for the vector entry
        pci_write_config(entry_address + 4,
                         0x1000 + k);  // Example data per vector

        // Marking the corresponding Pending Bit for Qword access
        uint32_t pba_value = pci_read_config(pba_qword_address);
        pba_value |= (1 << qword_bit);
        pci_write_config(pba_qword_address, pba_value);
    }
}

void setupMSIXPendingArrayWithDwordAccess(uint8_t bus, uint8_t device,
                                          uint8_t function, uint32_t pba_base,
                                          uint32_t num_vectors) {
    for (uint32_t k = 0; k < num_vectors; k++) {
        uint64_t pba_dword_address = pba_base + (k / 32) * 4;
        uint32_t dword_bit = k;

        // Mark the corresponding Pending Bit for DWORD access
        uint32_t pba_value = pci_read_config(pba_dword_address);
        pba_value |= (1 << dword_bit);
        pci_write_config(pba_dword_address, pba_value);
    }
}

void enableMSIX(uint8_t bus, uint8_t device, uint8_t function,
                uint32_t num_vectors) {
    uint32_t msix_cap_offset;
    if (!checkMSIXCapability(bus, device, function, &msix_cap_offset)) {
        print("MSI-X capability not found on the device.");
        return;
    }

    print("MSI-X capability found. Initializing...");

    uint32_t table_base = 0x80000000;  // Example base address
    uint32_t pba_base = 0x90000000;    // Example base for PBA

    initializeMSIXMessageControl(bus, device, function, msix_cap_offset,
                                 num_vectors);
    setupMSIXPendingArray(bus, device, function, table_base, pba_base,
                          num_vectors);
}

void configureMSIXCapability(uint8_t bus, uint8_t device, uint8_t function,
                             uint32_t cap_offset, uint64_t tableOffset,
                             uint64_t pbaOffset) {
    writeMSIXAddress(bus, device, function, cap_offset, PCI_MSIX_TABLE_OFFSET,
                     tableOffset);
    writeMSIXAddress(bus, device, function, cap_offset, PCI_MSIX_PBA_OFFSET,
                     pbaOffset);
}

void pci_enumerate() {
    print_colored("Enumerating PCI Devices...", COLOR_WHITE, COLOR_BLACK);
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

                    print_colored("Found PCI Device: Bus ", COLOR_WHITE,
                                  COLOR_BLACK);
                    newline();
                    print_i(bus);
                    print_colored(" Device ", COLOR_WHITE, COLOR_BLACK);
                    print_i(device);
                    print_colored(" Function ", COLOR_WHITE, COLOR_BLACK);
                    print_i(function);
                    newline();
                    print_colored(" - Vendor ID: 0x", COLOR_GREEN, COLOR_BLACK);
                    print_hex(vendor_id);
                    print_colored(" Device ID: 0x", COLOR_YELLOW, COLOR_BLACK);
                    print_hex(device_id);
                    newline();
                }
            }
        }
    }
}

void print_pci_capabilities(uint8_t bus, uint8_t device, uint8_t function) {
    uint32_t address;
    uint32_t value;
    uint8_t status_and_cap_ptr_offset;
    uint8_t capability_id;
    uint8_t next_cap_ptr;

    address = PCI_CONFIG_ADDRESS(bus, device, function, PCI_VENDOR_ID_OFFSET);

    if ((value & 0xFFFF) == 0xFFFF) {  // Invalid device
        print("Device doesn't exist!");
        return;
    }

    // Calculate and read the Status register to find the capabilities list
    // pointer
    address = PCI_CONFIG_ADDRESS(
        bus, device, function,
        PCI_STATUS_OFFSET);  // Offset 0x04 for Status Register
    value = pci_read_config(address);
    // Check if the Capabilities List bit is set (bit 4 of the Status register)
    if (!((value >> 16) & (1 << 4))) {
        print("No PCI capabilities list available.\n");
        return;
    }

    // Read the Capabilities Pointer (offset 0x34 for standard PCI)
    address = PCI_CONFIG_ADDRESS(bus, device, function, PCI_CAPABILITIES_OFFSET);
    value = pci_read_config(address);
    status_and_cap_ptr_offset = value & 0xFF;

    print("Capabilities List:\n");

    while (status_and_cap_ptr_offset != 0) {
        // Read capability structure at the current offset
        address =
            PCI_CONFIG_ADDRESS(bus, device, function, status_and_cap_ptr_offset);
        value = pci_read_config(address);

        capability_id = value & 0xFF;
        next_cap_ptr = (value >> 8) & 0xFF;

        // Print the capability ID and the current offset
        print("Capability ID: ");
        print_hex(capability_id);
        print_capability_name(capability_id);
        print("at offset ");
        print_hex(status_and_cap_ptr_offset);
        newline();

        // Move to the next capability
        status_and_cap_ptr_offset = next_cap_ptr;
    }
}

void print_capability_name(uint8_t cap_id) {
    switch (cap_id) {
        case 0:
            print(" (Capability ID) ");
            break;
        case 0x01:
            print(" (Power Management) ");
            break;
        case 0x02:
            print(" (Accelerated Graphics Port) ");
            break;
        case 0x03:
            print(" (Vital Product Data) ");
            break;
        case 0x04:
            print(" (Slot Identification) ");
            break;
        case 0x05:
            print(" (Message Signalled Interrupts) ");
            break;
        case 0x06:
            print(" (CompactPCI HotSwap) ");
            break;
        case 0x07:
            print(" (PCI-X) ");
            break;
        case 0x08:
            print(" (HyperTransport) ");
            break;
        case 0x09:
            print(" (Vendor-Specific) ");
            break;
        case 0x0A:
            print(" (Debug port) ");
            break;
        case 0x0B:
            print(" (CompactPCI Central Resource Control) ");
            break;
        case 0x0C:
            print(" (PCI Standard Hot-Plug Controller) ");
            break;
        case 0x0D:
            print(" (Bridge subsystem vendor/pcie_device ID) ");
            break;
        case 0x0E:
            print(" (AGP Target PCI-PCI bridge) ");
            break;
        case 0x0F:
            print(" (Secure pcie_device) ");
            break;
        case 0x10:
            print(" (PCI Express) ");
            break;
        case 0x11:
            print(" (MSI-X) ");
            break;
        case 0x12:
            print(" (SATA Data/Index Conf.) ");
            break;
        case 0x13:
            print(" (PCI Advanced Features) ");
            break;
        case 0x1F:
            print(" (PTM Extended Capability) ");
            break;
        default:
            print(" (Unknown) ");
            break;
    }
}
