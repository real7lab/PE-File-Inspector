# FileInspector

## Overview

**FileInspector** is a command-line tool written in C++ that analyzes Windows Portable Executable (PE) files and extracts key information such as:

* Imported DLLs
* Exported functions
* File sections with details including name, virtual address (VA), size, and permissions

The tool relies on the `PEParser` class to parse and validate the PE file.

### Developed by

* **real7lab**
* [Discord Server](https://discord.gg/visionn)

## Features

* Checks if the provided file is a valid PE.
* Lists imported DLLs from the PE file.
* Lists exported functions.
* Displays sections with name, virtual address, size, and permissions.
* Accepts the file path as a command-line argument or interactively.

## Installation

### Prerequisites

* C++ compiler (supports C++11 or later)

### Steps

1. Compile the project:

```bash
g++ FileInspector.cpp PEParser.cpp -o FileInspector
```

2. Run the program:

```bash
./FileInspector [path_to_pe_file]
```

If no path is provided as an argument, you will be prompted to enter it manually.

## Output Example

```
[~] Imported DLLs:
 - KERNEL32.dll
 - USER32.dll

[~] Exported Functions:
 - Function1
 - Function2

[~] Sections:
 - .text | VA: 0x1000 | Size: 0x6000 | Perms: RX
 - .data | VA: 0x7000 | Size: 0x2000 | Perms: RW
```

## Error Handling

* If the file is invalid or the path is incorrect, an error message will be shown.
* If no path is provided or the input is empty, the program will exit with an error.

## Troubleshooting

* **"Invalid file" error:**
  Make sure the specified file is a valid PE file (EXE or DLL).

* **No output or crashes:**
  Verify you have read permissions on the file and that the file path is correct.

## License

This project is developed by [real7lab](https://discord.gg/visionn).
