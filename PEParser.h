#pragma once

#include <string>
#include <vector>

struct SectionInfo {
    std::string name;
    uint32_t virtualAddress;
    uint32_t size;
    std::string permissions; // "R-X", "RW-", etc.
    /*
    R (Read) — la sezione può essere letta.
    W (Write) — la sezione può essere scritta (modificata).
    X (Execute) — la sezione può essere eseguita come codice.
    */
};

class PEParser {
public:
    explicit PEParser(const std::string& filepath);
    bool isValid() const;

    std::vector<std::string> getImportedDLLs() const;
    std::vector<std::string> getExportedFunctions() const;
    std::vector<SectionInfo> getSections() const;

private:
    std::string m_filepath;
    bool m_valid;

    std::vector<std::string> m_importedDLLs;
    std::vector<std::string> m_exportedFunctions;
    std::vector<SectionInfo> m_sections;

    void parse();
};
