#include "LocalizationManager.h"
#include <HookCrashersAPI.h>
#include <fstream>
#include <thread>
#include <chrono>
#include <codecvt> // Necessario per la conversione in SWFAttributeSetterHook
#include <locale>  // Necessario per la conversione

namespace CustomLocalizations {

    LocalizationManager& LocalizationManager::getInstance() {
        static LocalizationManager instance;
        return instance;
    }

    LocalizationManager::LocalizationManager() {}

    std::wstring LocalizationManager::ToWString(const std::string& str) {
        if (str.empty()) return std::wstring();
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
        std::wstring wstrTo(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
        return wstrTo;
    }

    int LocalizationManager::getNumericId(const std::string& logicalId) {
        auto it = m_idMap.find(logicalId);
        return (it != m_idMap.end()) ? it->second : -1;
    }

    const wchar_t* LocalizationManager::getStringByIndex(int index) const {
        if (index >= 0 && index < m_customStrings.size()) {
            const auto& entry = m_customStrings[index];
            if (m_languageIndex >= 0 && m_languageIndex < entry.languages.size()) {
                return entry.languages[m_languageIndex].c_str();
            }
        }
        return L""; // Restituisce una stringa vuota se non trova nulla
    }

    bool LocalizationManager::initialize(const std::string& modPath) {
        if (m_isInitialized) return true;

        uintptr_t steamClientBase = (uintptr_t)GetModuleHandleA("steamclient.dll");
        if (steamClientBase == 0) {
            HookCrashers::API::Client::LogError("[CustomLocalizations] steamclient.dll not found!");
            return false;
        }

        const uintptr_t LANGUAGE_ADDRESS = steamClientBase + 0x12AFE80;
        std::string language;

        for (int attempts = 0; attempts < 600; ++attempts) {
            if (*(char*)LANGUAGE_ADDRESS != '\0') {
                language = (char*)LANGUAGE_ADDRESS;
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        if (language.empty()) language = "english";

        HookCrashers::API::Client::LogInfo("[CustomLocalizations] Detected Steam language: " + language);

        if (language == "english") m_languageIndex = 0;
        else if (language == "german") m_languageIndex = 1;
        else if (language == "french") m_languageIndex = 2;
        else if (language == "spanish") m_languageIndex = 3;
        else if (language == "italian") m_languageIndex = 4;
        else if (language == "schinese") m_languageIndex = 5;
        else if (language == "koreana") m_languageIndex = 6;
        else if (language == "tchinese") m_languageIndex = 7;
        else if (language == "portuguese") m_languageIndex = 8;
        else if (language == "japanese") m_languageIndex = 9;
        else if (language == "russian") m_languageIndex = 10;
        else m_languageIndex = 0;

        std::string jsonPath = modPath + "strings.json";
        std::ifstream file(jsonPath);
        if (!file.is_open()) {
            HookCrashers::API::Client::LogError("[CustomLocalizations] Failed to open strings.json at: " + jsonPath);
            return false;
        }

        try {
            nlohmann::json root;
            file >> root;

            if (root.contains("base_custom_id")) {
                m_baseCustomId = root["base_custom_id"].get<int>();
            }

            if (root.contains("strings")) {
                for (const auto& item : root["strings"]) {
                    CustomLocalizationEntry entry;

                    entry.logicalId = item.at("id").get<std::string>();
                    entry.numericId = m_baseCustomId + m_customStrings.size();

                    entry.languages.push_back(ToWString(item.value("english", "")));
                    entry.languages.push_back(ToWString(item.value("german", "")));
                    entry.languages.push_back(ToWString(item.value("french", "")));
                    entry.languages.push_back(ToWString(item.value("spanish", "")));
                    entry.languages.push_back(ToWString(item.value("italian", "")));
                    entry.languages.push_back(ToWString(item.value("schinese", "")));
                    entry.languages.push_back(ToWString(item.value("korean", "")));
                    entry.languages.push_back(ToWString(item.value("tchinese", "")));
                    entry.languages.push_back(ToWString(item.value("portuguese", "")));
                    entry.languages.push_back(ToWString(item.value("japanese", "")));
                    entry.languages.push_back(ToWString(item.value("russian", "")));

                    m_customStrings.push_back(entry);
                    m_idMap[entry.logicalId] = entry.numericId;
                }
            }
            HookCrashers::API::Client::LogInfo("[CustomLocalizations] Loaded " + std::to_string(m_customStrings.size()) + " custom localization entries.");
        }
        catch (const std::exception& e) {
            HookCrashers::API::Client::LogError("[CustomLocalizations] Failed to parse strings.json: " + std::string(e.what()));
            return false;
        }

        m_isInitialized = true;
        return true;
    }
}