# PCI Bare-metal x86 QEMU APIs

These APIs are useful in directly interacting your bare-metal code with QEMU's PCI bus/subsystem.

The APIs are written in C and can be used with [x86 QEMU Bare-Metal Toolkit](https://gitlab.vayavyalabs.com:8000/thisisthedarshan/x86-QEMU-Bare-Metal) to build and run x86 Bare-metal binaries.

> TLDR; Jump to the [Function Definitions](#function-definitions) to get started

## Overview of PCI

Let's dive into **PCI** (Peripheral Component Interconnect), which is an older standard for connecting peripheral devices to a computer's motherboard. It was introduced by Intel in 1992 and became a very popular standard for many years before being replaced by PCIe (PCI Express). Here's an overview of PCI:

### 1. **What is PCI?**

PCI stands for **Peripheral Component Interconnect**. It's a standard for connecting peripheral devices (such as graphics cards, network cards, sound cards, and storage controllers) to a computer's central processing unit (CPU) and memory.

PCI was designed to provide a **high-speed data path** between the CPU and peripheral devices. It allows the computer's motherboard to support multiple peripheral devices simultaneously, enabling things like expansion slots for video cards, network cards, and other add-in cards.

### 2. **How PCI Works:**

PCI operates as a **bus-based system**, which means that multiple devices can share the same data lines to communicate with the CPU and memory. A bus is a set of physical and logical connections used for transmitting data between components.

When a device is inserted into a PCI slot, it gets access to the PCI bus, and the system can communicate with the device.

#### Key Features of PCI

- **32-bit or 64-bit bus width**:
  - The **32-bit** version of PCI was the most common and could transfer 32 bits (4 bytes) of data per clock cycle. It runs at 33 MHz or 66 MHz, offering data transfer rates of **133 MB/s (at 33 MHz)** or **266 MB/s (at 66 MHz)**.
  - The **64-bit** version, which is less common, could transfer 64 bits (8 bytes) per clock cycle. At 66 MHz, it provided transfer rates of up to **533 MB/s**.

- **Bus Mastering**:
  - PCI devices can act as "bus masters." This means that instead of waiting for the CPU to control communication, devices can directly initiate data transfers to and from memory. This feature greatly improves performance by reducing CPU overhead.

- **Plug-and-Play**:
  - PCI was one of the first standards to support **plug-and-play** functionality, meaning devices could be added or removed without needing to reboot the computer. The system automatically detected and configured devices.

### 3. **PCI Bus Architecture:**

The PCI bus has several key components:

- **Bus**: The shared data path used by devices to send and receive data.
- **Address Lines**: These lines are used to determine which device or memory location is being accessed.
- **Data Lines**: These are used to carry the data being transferred.
- **Control Lines**: These lines control the operations of the bus, such as read/write, memory-mapped I/O, and interrupt signaling.

The PCI bus is a **parallel bus**, meaning data is transmitted over multiple channels at the same time. However, this results in slower performance as the bus speed is limited by the maximum number of parallel signals.

### 4. **PCI Slots and Devices:**

PCI devices are typically connected via **PCI slots** on the motherboard. These slots provide both **power** and a **data path** for the connected devices. PCI slots typically come in two sizes:

- **32-bit PCI slots**: These are the original slots and are shorter in length.
- **64-bit PCI slots**: These are longer and are designed for more bandwidth-intensive devices.

The devices themselves can be any of a wide variety of peripherals, such as:

- **Network interface cards (NICs)**
- **Graphics cards**
- **Sound cards**
- **SCSI controllers**
- **Storage controllers (IDE, SATA, etc.)**
- **TV tuners**

### 5. **PCI Configuration Space:**

Each PCI device has a **configuration space**, which is a memory area that stores information about the device, such as its vendor ID, device ID, class code, and other attributes. This space is used by the system's firmware (such as BIOS) and operating system to **configure** the device and allocate resources like I/O ports, memory ranges, and IRQs (interrupt request lines).

#### Key Registers in PCI Configuration Space

- **Vendor ID**: Identifies the manufacturer of the device.
- **Device ID**: Identifies the specific device model.
- **Command and Status**: Control and status flags for the device.
- **Base Address Registers (BARs)**: Used to specify memory and I/O addresses for device resources.
- **Interrupt Line**: Specifies which IRQ line the device uses for interrupts.
- **Class Code**: Classifies the device type (e.g., video, audio, storage, etc.).

### 6. **PCI vs. PCIe:**

While PCI was a revolutionary standard for its time, it has several limitations compared to **PCIe (PCI Express)**, which was introduced in the early 2000s. Some key differences:

- **Bandwidth**: PCIe provides much higher bandwidth compared to PCI. PCIe can support data rates in the **gigabits per second** range, whereas PCI's maximum theoretical bandwidth is much lower (133 MB/s for 32-bit at 33 MHz).
- **Serial vs Parallel**: PCI is a **parallel bus**, meaning that multiple devices share the same data lines. PCIe, on the other hand, is a **serial bus**, with point-to-point connections between devices, leading to faster and more scalable communication.
- **Number of Devices**: PCIe supports more devices, thanks to its ability to create **multiple lanes** (1x, 4x, 8x, 16x, etc.), whereas PCI is limited to a fixed number of devices per bus.

### 7. **Legacy PCI Usage:**

While PCI has been mostly replaced by PCIe in modern systems, you can still find **legacy PCI slots** in many motherboards today, especially for specialized hardware like sound cards, older graphics cards, or network cards. However, **PCIe** is now the dominant standard for expansion cards in modern computers.

### 8. **Power Management in PCI:**

PCI also supports basic **power management** features, such as:

- **Power states**: Devices and the system as a whole can enter different power states (e.g., active, suspend, and off) to save power when devices are not in use.
- **Wake-on-LAN**: Some PCI devices, like network interface cards (NICs), can support features like Wake-on-LAN, which allows the system to be powered on remotely via network activity.

## Overview of MSI and MSI-X

**MSI** (Message Signaled Interrupts) and **MSI-X** (Message Signaled Interrupts eXtended) are modern methods for handling interrupts in computer systems. They were introduced to address limitations with the traditional **legacy interrupt system** (such as the older **IRQ lines** used with PCI devices).

Here's an overview of both:

### 1. **Message Signaled Interrupts (MSI):**

MSI is an interrupt delivery mechanism that uses **memory writes** instead of dedicated physical interrupt lines (IRQs). This is in contrast to the traditional method of interrupts, which involved devices sending signals through physical pins on the motherboard (e.g., IRQ 0, IRQ 1, etc.).

#### Key Features of MSI

- **No IRQ lines**: Instead of using physical pins to signal interrupts, MSI sends a message via a memory write operation. The device writes a specific value to a predefined memory address, which causes the CPU to recognize an interrupt and take appropriate action.
- **Efficient Interrupt Delivery**: MSI allows devices to signal interrupts more efficiently, without needing dedicated interrupt lines or a complex interrupt controller.
- **Reduced Interrupt Latency**: By signaling an interrupt via memory writes, MSI can lower interrupt delivery latency compared to legacy interrupt methods, improving overall system performance.
- **Scalability**: MSI allows for a large number of interrupts without worrying about the limitations of physical interrupt lines. Devices can request multiple interrupts without requiring additional IRQ lines or hardware resources.

#### How MSI Works

When a device needs to signal an interrupt, it writes to a specific memory address that is mapped to an interrupt vector. This memory write is treated as an interrupt signal, which causes the CPU to handle the interrupt. This is done using a **system memory address** (typically part of the device’s **BAR**—Base Address Register).

In the MSI method:

1. **The Device** writes to a memory-mapped address in the system's address space.
2. **The CPU** is alerted to the interrupt via the memory write. The system then invokes the interrupt handler for that device.

This approach eliminates the need for physical IRQ lines and reduces the interrupt bottleneck caused by shared IRQs, offering a cleaner, more scalable solution.

### 2. **Message Signaled Interrupts eXtended (MSI-X):**

MSI-X is an extension of MSI that allows even more flexibility and capabilities. It provides improvements over MSI in several ways, especially in terms of scalability, which makes it suitable for modern multi-core processors and high-performance devices.

#### Key Features of MSI-X

- **Multiple Interrupt Vectors**: MSI supports up to 32 interrupts for a device, whereas **MSI-X** allows up to **2048 interrupt vectors** per device. This makes MSI-X suitable for devices like high-speed network cards, storage controllers, and other peripherals that require a high number of interrupt sources.
- **Per-Core Interrupts**: MSI-X can target specific CPU cores, making it easier to distribute interrupts across multiple processors in multi-core systems. This allows devices to utilize the full processing power of multi-core systems without overloading a single core.
- **Scalability**: With MSI-X, you can have a large number of interrupts with more flexible distribution, which is important in high-performance, modern computing environments.
- **Interrupt Steering**: MSI-X also supports "interrupt steering," allowing interrupts to be directed to specific CPU cores, making better use of the system's multi-core architecture and improving performance by distributing interrupt handling across multiple processors.

#### How MSI-X Works

MSI-X improves upon the original MSI method by introducing the following features:

1. **Multiple Interrupt Vectors**: Each device with MSI-X can be programmed to generate multiple interrupts. The device’s memory-mapped registers define the vector table, which can hold up to 2048 different interrupt vectors.
2. **Vector Control**: With MSI-X, each interrupt vector can be controlled independently. This allows for finer-grained interrupt management, which is important for devices that need to handle many events simultaneously.
3. **Direct CPU Core Assignment**: MSI-X allows the assignment of interrupt vectors to specific processor cores. This means that you can optimize how interrupts are processed, reducing interrupt handling bottlenecks and improving the overall system performance.
4. **PCIe Support**: MSI-X is used extensively with **PCI Express (PCIe)** devices. The higher bandwidth and more flexible interrupt delivery system make MSI-X especially well-suited for modern high-performance PCIe peripherals.

### 3. **Comparison Between MSI and MSI-X:**

| **Feature**                | **MSI**                               | **MSI-X**                                      |
|----------------------------|---------------------------------------|------------------------------------------------|
| **Number of Interrupts**    | Up to 32 interrupts per device        | Up to 2048 interrupts per device               |
| **Interrupt Steering**      | No                                    | Yes, interrupts can be targeted to specific CPU cores |
| **Scalability**             | Limited to 32 interrupts per device   | Scalable to hundreds or thousands of interrupts |
| **Target Audience**         | General use, low- to mid-performance devices | High-performance devices, such as network cards, storage controllers |
| **Compatibility**           | Supported in PCI, PCIe devices        | Primarily used in PCIe devices                  |

### 4. **Why MSI and MSI-X Are Used:**

- **Reduces Interrupt Bottlenecks**: Traditional IRQ lines (especially with shared IRQs) can cause contention, where multiple devices compete for the same IRQ, slowing down the interrupt handling process. MSI and MSI-X remove this contention by using memory-based signaling, which allows for a large number of interrupts without conflicts.
- **Improved Performance**: By allowing devices to trigger interrupts via memory writes and enabling the distribution of interrupts to different processor cores, MSI and MSI-X can reduce the interrupt overhead and improve overall system performance, especially in multi-core systems.
- **Scalability**: With MSI-X, the number of interrupts per device can be expanded far beyond the 32 interrupt limit of MSI, making it suitable for devices that need many interrupt vectors (like high-speed network cards or other specialized hardware).

### 5. **Applications of MSI and MSI-X:**

- **Network Interface Cards (NICs)**: High-performance NICs, especially in servers or data centers, can benefit from MSI-X because they often need to handle many simultaneous interrupts from the network.
- **Storage Controllers**: Devices like RAID controllers or NVMe storage devices that have multiple queues can use MSI-X to handle a large number of interrupts efficiently.
- **High-Speed Peripherals**: Devices like GPUs, FPGAs, or other high-performance PCIe peripherals can use MSI-X to achieve better performance by distributing interrupts across multiple CPU cores.

### 6. **PCI Configuration for MSI/MSI-X:**

- **MSI Setup**: To enable MSI for a device, the system must configure certain registers in the PCI configuration space. The device needs to be configured to use MSI, and the system needs to allocate an interrupt vector that can be mapped to the device’s interrupt.
  
- **MSI-X Setup**: MSI-X requires more complex configuration since the device can use multiple interrupt vectors. The device’s **MSI-X table** contains information about the interrupt vectors, and the system needs to allocate memory and map these interrupt vectors properly.

MSI and MSI-X are critical technologies for improving interrupt handling in high-performance systems, and they are especially beneficial for devices that generate a large number of interrupts (like network cards, storage controllers, and GPUs).

## **Including**

```c
#include <pci.h>
```

## **Function Definitions**

- **`getVID`**  
   Reads the Vendor ID (VID) of a PCI device.  
   **Prototype:**  

   ```c
   uint16_t getVID(uint8_t PCI_Bus, uint8_t PCI_DeviceNumber, uint8_t FunctionNumber);
   ```

- **`getDID`**  
   Reads the Device ID (DID) of a PCI device.  
   **Prototype:**  

   ```c
   uint16_t getDID(uint8_t PCI_Bus, uint8_t PCI_DeviceNumber, uint8_t FunctionNumber);
   ```

- **`getBAR0`**  
   Reads the value of BAR0 (Base Address Register 0) of a PCI device.  
   **Prototype:**  

   ```c
   uint32_t getBAR0(uint8_t PCI_Bus, uint8_t PCI_DeviceNumber, uint8_t FunctionNumber);
   ```

- **`writeMSIXAddress`**  
   Writes to the MSI-X Message Table Address.  
   **Prototype:**  

   ```c
   void writeMSIXAddress(uint8_t PCI_Bus, uint8_t PCI_DeviceNumber, uint8_t FunctionNumber, uint64_t address);
   ```

- **`writeMSIXData`**  
   Writes to the MSI-X Message Table Data.  
   **Prototype:**  

   ```c
   void writeMSIXData(uint8_t PCI_Bus, uint8_t PCI_DeviceNumber, uint8_t FunctionNumber, uint32_t data);
   ```

- **`setupMSIXPendingArray`**  
   Allocates and initializes the MSI-X Pending Bit Array.  
   **Prototype:**  

   ```c
   void setupMSIXPendingArray(uint8_t PCI_Bus, uint8_t PCI_DeviceNumber, uint8_t FunctionNumber, uint64_t address, uint32_t size);
   ```

- **`configureMSIXCapability`**  
   Configures the MSI-X Capability Structure by setting the Table Offset, PBA Offset, and enabling MSI-X.  
   **Prototype:**  

   ```c
   void configureMSIXCapability(uint8_t PCI_Bus, uint8_t PCI_DeviceNumber, uint8_t FunctionNumber, uint64_t tableOffset, uint64_t pbaOffset);
   ```

- **`initializeMSIXMessageControl`**  
   Initializes the Message Control register of the MSI-X Capability Structure.  
   **Prototype:**  

   ```c
   void initializeMSIXMessageControl(uint8_t PCI_Bus, uint8_t PCI_DeviceNumber, uint8_t FunctionNumber);
   ```

- **`enableMSIX`**  
   Enables MSI-X by setting the MSI-X Enable flag in the MSI-X Capability Structure Message Control register.  
   **Prototype:**  

   ```c
   void enableMSIX(uint8_t PCI_Bus, uint8_t PCI_DeviceNumber, uint8_t FunctionNumber);
   ```

- **`initializeInterruptModeration`**  
    Initializes the Interval field of the Interrupt Moderation register with the target interrupt moderation rate.  
    **Prototype:**  

    ```c
    void initializeInterruptModeration(uint8_t PCI_Bus, uint8_t PCI_DeviceNumber, uint8_t FunctionNumber, uint32_t interval);
    ```

- **`pci_read_config`**  
   Reads a 32-bit value from the PCI configuration space.  
   **Prototype:**  

   ```c
   uint32_t pci_read_config(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);
   ```

- **`pci_write_config`**  
   Writes a 32-bit value to the PCI configuration space.  
   **Prototype:**  

   ```c
   void pci_write_config(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value);
   ```

- **`configureMSIXCapability`**
   Sets up the MSI-X Pending Bit Array (PBA) for the specified PCI device.
   **Prototype**

   ```c
   void configureMSIXCapability(uint8_t bus, uint8_t device, uint8_t function,
                             uint32_t cap_offset, uint64_t tableOffset, uint64_t pbaOffset) 
   ```

- **`enumerate_pci_devices`**  

   Enumerates all PCI devices on the bus and stores their details for debugging.  
   **Prototype:**  

   ```c
   void enumerate_pci_devices();
   ```

- **`print_pci_capabilities`**
   Prints capabilities of a given device onto VGA.
   **Prototype:**  

   ```c
   void print_pci_capabilities(uint8_t bus, uint8_t device, uint8_t function);
   ```
