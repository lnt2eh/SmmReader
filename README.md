# SMMapper

This project provides a kernel-mode driver for mapping and interacting with System Management RAM (SMRAM) and the bootloader at memory address `0x7C00`. Additionally, a user-mode application is included to search for a specific opcode (`E6 B2`), which corresponds to the instruction `out 0xB2, AL`. This project is designed to explore low-level Windows internals, hardware interactions, and system management features.

## Features

- **SMRAM Mapping**: Maps and accesses SMRAM to retrieve segment selector values from the SMRAM state-save area.
- **Bootloader Mapping**: Maps and validates the bootloader at the address `0x7C00`, checking for the `0x55AA` signature.
- **SMI Trigger**: Sends a byte (`0x12`) to the I/O port `0xB2` to trigger a System Management Interrupt (SMI).
- **Opcode Search**: A user-mode application that searches for the `E6 B2` opcode (the `out 0xB2, AL` instruction) in a binary file.

## Prerequisites

Before using the project, make sure you have the following:

- **Windows OS**: The kernel-mode driver is designed for Windows systems.
- **Visual Studio**: Required to compile the driver code.
- **Kernel Debugging Tools**: Recommended to test and debug the driver.
- **User-mode Application**: A Windows environment where you can compile and run the user-mode application.

## Usage

### Kernel Driver

1. **Compile the Driver**: Open the `WDM Empty Driver` project in Visual Studio.
2. **Build the Driver**: Compile the project into a `.sys` driver file.
3. **Load the Driver**: Load the `SmmReader.sys` driver using **OsrLoader**.
4. **Debugging**: Use **DebugView** to monitor and view the kernel-generated logs and information during driver execution.


### User-Mode Application

1. **Compile the Application**: Open the `FirmReader` project in a C development environment (such as Visual Studio) and build it to generate the `FirmReader.exe` executable.

2. **Run the Application**: Execute the program from the command line or through your development environment. You must provide the path to the binary file that you want to analyze as an argument.
   
   Example usage:
   ```bash
   FirmReader.exe "C:\path\to\your\binary.bin"


