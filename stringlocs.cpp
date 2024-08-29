#include "stringlocs.h"

std::vector<LocalizationEntry>* InjectAndGetCustomLocalizations() {
    auto* vec = new std::vector<LocalizationEntry>;

    // Open and parse the JSON file
    std::ifstream inputFile("./scripts/CustomLocalizations/strings.json");
    if (!inputFile.is_open()) {
        return nullptr;
    }

    nlohmann::json jsonData;
    inputFile >> jsonData;

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

    const auto LANGUAGE_ADDRESS = steamClientBase + 0x125EF78;
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

    auto* locStartAddress = InjectAndGetCustomLocalizations();

    BYTE* newMemory = (BYTE*)VirtualAlloc(NULL, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (newMemory == NULL) {
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

    // Load the value at locStartAddress + (subtracted EDX * 12)
    codeCave[offset++] = 0xB8; // mov eax,
    *(DWORD*)(codeCave + offset) = (DWORD)locStartAddress->data();
    offset += 4;

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