#include "stringlocs.h"
std::vector<LocalizationEntry>* InjectAndGetCustomLocalizations() {
    Logger& l = Logger::Instance();

    char exePath[MAX_PATH];
    if (GetModuleFileNameA(NULL, exePath, MAX_PATH) == 0) {
        l.Get()->info("Failed to get executable path");
        l.Get()->flush();
        return nullptr;
    }

    // Estrai la directory dell'eseguibile
    std::string basePath = std::string(exePath);
    basePath = basePath.substr(0, basePath.find_last_of("\\/"));

    char currentDir[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, currentDir);
    l.Get()->info("Current working directory: {}", currentDir);
    l.Get()->info("Executable directory: {}", basePath);

    std::string jsonPath = basePath + "\\scripts\\CustomLocalizations\\strings.json";
    l.Get()->info("Attempting to open file: {}", jsonPath);
    l.Get()->flush();

    std::ifstream inputFile(jsonPath);
    if (!inputFile.is_open()) {
        l.Get()->info("Failed to open file");
        l.Get()->flush();
        return nullptr;
    }

    l.Get()->info("File opened successfully");
    l.Get()->flush();

    auto* vec = new std::vector<LocalizationEntry>;
    try {
        nlohmann::json jsonData;
        inputFile >> jsonData;
        l.Get()->info("JSON parsed, entries: {}", jsonData.size());
        l.Get()->flush();

        for (const auto& item : jsonData) {
            LocalizationEntry locString;
            locString.id = item["id"].get<int>();
            locString.languages[0] = ToWChar(item["englishStr"].get<std::string>());
            locString.languages[1] = ToWChar(item["germanStr"].get<std::string>());
            locString.languages[2] = ToWChar(item["frenchStr"].get<std::string>());
            locString.languages[3] = ToWChar(item["spanishStr"].get<std::string>());
            locString.languages[4] = ToWChar(item["italianStr"].get<std::string>());
            locString.languages[5] = ToWChar(item["simplifiedChineseStr"].get<std::string>());
            locString.languages[6] = ToWChar(item["koreanStr"].get<std::string>());
            locString.languages[7] = ToWChar(item["tradChineseStr"].get<std::string>());
            locString.languages[8] = ToWChar(item["portugueseStr"].get<std::string>());
            locString.languages[9] = ToWChar(item["japaneseStr"].get<std::string>());
            locString.languages[10] = ToWChar(item["russianStr"].get<std::string>());
            vec->push_back(locString);
        }

        l.Get()->info("Loaded {} localization entries", vec->size());
        l.Get()->flush();
    }
    catch (const std::exception& e) {
        l.Get()->info("Exception during JSON parsing: {}", e.what());
        l.Get()->flush();
        delete vec;
        return nullptr;
    }

    return vec;
}

// Ensure to free the allocated memory when you are done using the LocalizedString objects
void FreeLocalizedStrings(std::vector<LocalizationEntry>* vec) {
    if (vec) {
        for (auto& locString : *vec) {
            for (auto& lang : locString.languages) {
                free(const_cast<wchar_t*>(lang));  // Free the allocated strings
            }
        }
        delete vec;
    }
}

void InitFlow(uintptr_t base) {
    Logger& l = Logger::Instance();
    DWORD oldProtect;
    DWORD steamClientBase = (DWORD)GetModuleHandleA("steamclient.dll");

    l.Get()->info("Steam client base: 0x{:02X}", steamClientBase);
    l.Get()->flush();

    if (steamClientBase == NULL) {
        return;
    }

    const auto LANGUAGE_ADDRESS = steamClientBase + 0x12009D0;
    l.Get()->info("Final offset: 0x{:02X}", LANGUAGE_ADDRESS);
    l.Get()->flush();
    const int MAX_ATTEMPTS = 600;  // 60 seconds max (checking every 100ms)

    std::string language;
    int languageIndex = 0;
    int attempts;
    for (attempts = 0; attempts < MAX_ATTEMPTS; ++attempts) {
        MEMORY_BASIC_INFORMATION mbi;
        if (VirtualQuery(reinterpret_cast<LPCVOID>(LANGUAGE_ADDRESS), &mbi, sizeof(mbi))) {
            if (mbi.State == MEM_COMMIT) {
                // Check if the first byte is non-zero
                char firstByte = *reinterpret_cast<char*>(LANGUAGE_ADDRESS);
                if (firstByte == '\0') {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    continue;  // Skip to the next iteration
                }

                // Now try to read the data
                try {
                    const size_t MAX_LENGTH = 50;  // Maximum length to prevent infinite loops
                    for (size_t i = 0; i < MAX_LENGTH; ++i) {
                        char currentChar = *reinterpret_cast<char*>(LANGUAGE_ADDRESS + i);
                        if (currentChar == '\0') {
                            break;  // Stop at null terminator
                        }
                        language += currentChar;
                    }
                    break;  // Successfully read the language, exit the loop
                }
                catch (...) {
                    break;  // Exit the loop on exception
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    l.Get()->info("Current language {}", language);
    l.Get()->flush();
    if (language == "english") languageIndex = 0;
    else if (language == "german") languageIndex = 1;
    else if (language == "french") languageIndex = 2;
    else if (language == "spanish") languageIndex = 3;
    else if (language == "italian") languageIndex = 4;
    else if (language == "schinese") languageIndex = 5;
    else if (language == "koreana") languageIndex = 6;
    else if (language == "tchinese") languageIndex = 7;
    else if (language == "portuguese") languageIndex = 8;
    else if (language == "japanese") languageIndex = 9;
    else if (language == "russian") languageIndex = 10;
    else languageIndex = 0;  // Default

    l.Get()->info("languageIndex: {}", languageIndex);
    l.Get()->flush();

    auto* locStartAddress = InjectAndGetCustomLocalizations();

    l.Get()->info("LocStartAddress created");
    l.Get()->flush();

    BYTE* newMemory = (BYTE*)VirtualAlloc(NULL, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (newMemory == NULL) {
        return;
    }

    l.Get()->info("newMemory created");
    l.Get()->flush();

    if (!locStartAddress || locStartAddress->empty()) {
        l.Get()->info("locStartAddress is null or empty");
        l.Get()->flush();
        return;
    }

    // Set up our code cave
    BYTE* codeCave = newMemory;

    // Write our new comparison and branching logic
    int offset = 0;
    codeCave[offset++] = 0x81; // cmp
    codeCave[offset++] = 0xFA; // edx,
    *(DWORD*)(codeCave + offset) = 0x033C; // 828
    offset += 4;

    codeCave[offset++] = 0x7C; // jl (jump if less than)
    codeCave[offset++] = 0x2A; // to original code (adjust this offset!!!!!)

    // If ID >= 828, subtract 827 from EDX
    codeCave[offset++] = 0x81; // sub
    codeCave[offset++] = 0xEA; // edx,
    *(DWORD*)(codeCave + offset) = 0x033C; // 828
    offset += 4;

    l.Get()->info("BEFORE assigning locStartAddress data");
    l.Get()->flush();

    // Load the value at locStartAddress + (subtracted EDX * 12)
    codeCave[offset++] = 0xB8; // mov eax,
    *(DWORD*)(codeCave + offset) = (DWORD)locStartAddress->data();
    offset += 4;

    l.Get()->info("AFTER assigning locStartAddress data");
    l.Get()->flush();

    codeCave[offset++] = 0x69; // imul
    codeCave[offset++] = 0xD2; // imul edx, edx
    *(DWORD*)(codeCave + offset) = sizeof(LocalizationEntry); // Multiply by 48 bytes
    offset += 4;

    // Load languageIndex into ECX
    codeCave[offset++] = 0xB9; // mov ecx,
    *(DWORD*)(codeCave + offset) = languageIndex; // Load languageIndex into ecx
    offset += 4;

    // Multiply ECX by 4
    codeCave[offset++] = 0x69; // imul
    codeCave[offset++] = 0xC9; // imul ecx, ecx
    *(DWORD*)(codeCave + offset) = 4; // Multiply by 4
    offset += 4;

    // Add the result to EAX
    codeCave[offset++] = 0x01; // add
    codeCave[offset++] = 0xC8; // add eax, ecx

    codeCave[offset++] = 0x01; // add
    codeCave[offset++] = 0xD0; // add eax, ecx

    codeCave[offset++] = 0x83; // add
    codeCave[offset++] = 0xC0; // add eax, 4
    codeCave[offset++] = 0x04; // add eax, 4

    codeCave[offset++] = 0x8B; // mov
    codeCave[offset++] = 0x30; // mov esi, eax

    // Jump to 0x304B8E (skipping the rest of the original string loading code)
    codeCave[offset++] = 0xE9;
    *(DWORD*)(codeCave + offset) = (base + 0x84B8E) - (DWORD)(codeCave + offset + 4);
    offset += 4;

    // Original code path (for IDs < 828)
    codeCave[offset++] = 0x8B; // mov eax,dword ptr ds:[ecx+C]
    codeCave[offset++] = 0x41;
    codeCave[offset++] = 0x0C;

    codeCave[offset++] = 0x8B; // mov ecx,dword ptr ds:[ecx+8]
    codeCave[offset++] = 0x49;
    codeCave[offset++] = 0x08;

    codeCave[offset++] = 0x8B; // mov eax,dword ptr ds:[eax+edx*4]
    codeCave[offset++] = 0x04;
    codeCave[offset++] = 0x90;

    codeCave[offset++] = 0x8B; // mov esi,dword ptr ds:[eax+ecx*4+4]
    codeCave[offset++] = 0x74;
    codeCave[offset++] = 0x88;
    codeCave[offset++] = 0x04;

    codeCave[offset++] = 0xE9;
    *(DWORD*)(codeCave + offset) = (base + 0x84B8E) - (DWORD)(codeCave + offset + 4);
    offset += 4;
    l.Get()->info("Injected code");
    l.Get()->flush();
    // Patch the original code to jump to our code cave
    uintptr_t originalInstructionAddress = base + 0x84B79;
    if (VirtualProtect(reinterpret_cast<void*>(originalInstructionAddress), 5, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        *(BYTE*)originalInstructionAddress = 0xE9;  // jmp
        *(DWORD*)(originalInstructionAddress + 1) = (DWORD)codeCave - (originalInstructionAddress + 5);

        DWORD temp;
        VirtualProtect(reinterpret_cast<void*>(originalInstructionAddress), 5, oldProtect, &temp);
    }
    else {
        VirtualFree(newMemory, 0, MEM_RELEASE);
        return;
    }
}