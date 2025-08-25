#pragma once

#include <string>
#include <vector>
#include <map>
#include <windows.h>
#include <json.hpp>
#include "../Data/LocalizedString.h"

namespace CustomLocalizations {

    class LocalizationManager {
    private:
        LocalizationManager();
        LocalizationManager(const LocalizationManager&) = delete;
        LocalizationManager& operator=(const LocalizationManager&) = delete;

        std::wstring ToWString(const std::string& str);

        bool m_isInitialized = false;
        int m_languageIndex = 0;
        int m_baseCustomId = 5000;

        // Vettore che contiene le traduzioni
        std::vector<CustomLocalizationEntry> m_customStrings;
        // Mappa da ID logico a ID numerico
        std::map<std::string, int> m_idMap;

    public:
        static LocalizationManager& getInstance();

        bool initialize(const std::string& modPath);

        int getNumericId(const std::string& logicalId);
        const wchar_t* getStringByIndex(int index) const;

        bool isInitialized() const { return m_isInitialized; }
    };
}