#include "PEParser.h"
#include <fstream>
#include <iostream>
#include <windows.h>

PEParser::PEParser(const std::string& filepath)
    : m_filepath(filepath), m_valid(false) {
    parse();
}

bool PEParser::isValid() const {
    return m_valid;
}

std::vector<std::string> PEParser::getImportedDLLs() const {
    return m_importedDLLs;
}

std::vector<std::string> PEParser::getExportedFunctions() const {
    return m_exportedFunctions;
}

std::vector<SectionInfo> PEParser::getSections() const {
    return m_sections;
}

void PEParser::parse() {
    std::ifstream file(m_filepath, std::ios::binary);
    if (!file) {
        std::cerr << "[!] Failed to open file: " << m_filepath << std::endl;
        return;
    }

    IMAGE_DOS_HEADER dosHeader{};
    file.read(reinterpret_cast<char*>(&dosHeader), sizeof(dosHeader));
    if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) {
        return;
    }

    file.seekg(dosHeader.e_lfanew, std::ios::beg);

    IMAGE_NT_HEADERS ntHeaders{};
    file.read(reinterpret_cast<char*>(&ntHeaders), sizeof(ntHeaders));
    if (ntHeaders.Signature != IMAGE_NT_SIGNATURE) {
        return;
    }

    m_valid = true;

    file.seekg(dosHeader.e_lfanew + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER) + ntHeaders.FileHeader.SizeOfOptionalHeader, std::ios::beg);

    m_sections.clear();
    for (int i = 0; i < ntHeaders.FileHeader.NumberOfSections; ++i) {
        IMAGE_SECTION_HEADER section{};
        file.read(reinterpret_cast<char*>(&section), sizeof(section));

        SectionInfo si;
        si.name = std::string(reinterpret_cast<const char*>(section.Name), strnlen_s(reinterpret_cast<const char*>(section.Name), 8));
        si.virtualAddress = section.VirtualAddress;
        si.size = section.Misc.VirtualSize;

        si.permissions = "";
        if (section.Characteristics & IMAGE_SCN_MEM_READ) si.permissions += "R";
        else si.permissions += "-";
        if (section.Characteristics & IMAGE_SCN_MEM_WRITE) si.permissions += "W";
        else si.permissions += "-";
        if (section.Characteristics & IMAGE_SCN_MEM_EXECUTE) si.permissions += "X";
        else si.permissions += "-";

        m_sections.push_back(si);

    }

    m_importedDLLs.clear();
    DWORD importDirRVA = ntHeaders.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    DWORD importDirSize = ntHeaders.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size;
    if (importDirRVA == 0) return; 

    file.seekg(dosHeader.e_lfanew + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER) + ntHeaders.FileHeader.SizeOfOptionalHeader, std::ios::beg);
    std::vector<IMAGE_SECTION_HEADER> sections(ntHeaders.FileHeader.NumberOfSections);
    for (int i = 0; i < ntHeaders.FileHeader.NumberOfSections; ++i) {
        file.read(reinterpret_cast<char*>(&sections[i]), sizeof(IMAGE_SECTION_HEADER));
    }

    auto rvaToOffset = [&](DWORD rva) -> DWORD {
        for (const auto& sec : sections) {
            if (rva >= sec.VirtualAddress && rva < sec.VirtualAddress + sec.SizeOfRawData) {
                return rva - sec.VirtualAddress + sec.PointerToRawData;
            }
        }
        return 0;
        };

    DWORD importOffset = rvaToOffset(importDirRVA);
    if (importOffset == 0) return;

    file.seekg(importOffset, std::ios::beg);

    while (true) {
        IMAGE_IMPORT_DESCRIPTOR importDesc{};
        file.read(reinterpret_cast<char*>(&importDesc), sizeof(importDesc));
        if (importDesc.Name == 0) break;

        DWORD nameOffset = rvaToOffset(importDesc.Name);
        if (nameOffset == 0) break;

        file.seekg(nameOffset, std::ios::beg);

        char dllName[256]{};
        file.getline(dllName, sizeof(dllName));
        m_importedDLLs.push_back(std::string(dllName));

        file.seekg(importOffset + sizeof(IMAGE_IMPORT_DESCRIPTOR) * (m_importedDLLs.size()), std::ios::beg);
    }

    m_exportedFunctions.clear();
    DWORD exportDirRVA = ntHeaders.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    if (exportDirRVA == 0) return; 

    DWORD exportOffset = rvaToOffset(exportDirRVA);
    if (exportOffset == 0) return;

    file.seekg(exportOffset, std::ios::beg);

    IMAGE_EXPORT_DIRECTORY exportDir{};
    file.read(reinterpret_cast<char*>(&exportDir), sizeof(exportDir));

    if (exportDir.NumberOfNames == 0) return;

    DWORD namesOffset = rvaToOffset(exportDir.AddressOfNames);
    if (namesOffset == 0) return;

    file.seekg(namesOffset, std::ios::beg);

    for (DWORD i = 0; i < exportDir.NumberOfNames; ++i) {
        DWORD nameRVA = 0;
        file.read(reinterpret_cast<char*>(&nameRVA), sizeof(DWORD));
        DWORD nameOffset2 = rvaToOffset(nameRVA);
        if (nameOffset2 == 0) continue;

        file.seekg(nameOffset2, std::ios::beg);
        char funcName[256]{};
        file.getline(funcName, sizeof(funcName));
        m_exportedFunctions.push_back(std::string(funcName));

        file.seekg(namesOffset + sizeof(DWORD) * (i + 1), std::ios::beg);
    }
}
