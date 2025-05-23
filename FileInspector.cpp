// FileInspector.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include "PEParser.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Inserisci la path del file PE: ";
        std::string path;
        std::getline(std::cin, path);

        if (path.empty()) {
            std::cout << "[!] Path non valida." << std::endl;
            return 1;
        }

        PEParser parser(path);

        if (!parser.isValid()) {
            std::cout << "[!] File is not a valid PE file." << std::endl;
            return 1;
        }

        std::cout << "[~] Imported DLLs:" << std::endl;
        for (const auto& dll : parser.getImportedDLLs()) {
            std::cout << " - " << dll << std::endl;
        }

        std::cout << "\n[~] Exported Functions:" << std::endl;
        for (const auto& func : parser.getExportedFunctions()) {
            std::cout << " - " << func << std::endl;
        }

        std::cout << "\n[~] Sections:" << std::endl;
        for (const auto& sec : parser.getSections()) {
            std::cout << " - " << sec.name
                << " | VA: 0x" << std::hex << sec.virtualAddress
                << " | Size: 0x" << sec.size
                << " | Perms: " << sec.permissions << std::dec << std::endl;
        }

        return 0;
    }

    std::string path = argv[1];
    PEParser parser(path);

    if (!parser.isValid()) {
        std::cout << "[!] File is not a valid PE file." << std::endl;
        return 1;
    }

    std::cout << "[~] Imported DLLs:" << std::endl;
    for (const auto& dll : parser.getImportedDLLs()) {
        std::cout << " - " << dll << std::endl;
    }

    std::cout << "\n[~] Exported Functions:" << std::endl;
    for (const auto& func : parser.getExportedFunctions()) {
        std::cout << " - " << func << std::endl;
    }

    std::cout << "\n[~] Sections:" << std::endl;
    for (const auto& sec : parser.getSections()) {
        std::cout << " - " << sec.name
            << " | VA: 0x" << std::hex << sec.virtualAddress
            << " | Size: 0x" << sec.size
            << " | Perms: " << sec.permissions << std::dec << std::endl;
    }

    return 0;
}
