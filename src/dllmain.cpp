#include "stdafx.h"
#include "Core/LocalizationManager.h"
#include "SWF/GetLocalization.h"
#include <HookCrashersAPI.h>
#include <windows.h>
#include "Hooks/StringLookupHook.h"

using HookCrashers::API::Client;

extern "C" {
    __declspec(dllexport) const char* GetModName() { return "Custom Localizations"; }
    __declspec(dllexport) const char* GetModAuthor() { return "ilVonBurza"; }
    __declspec(dllexport) const char* GetModVersion() { return "2.0"; }

    __declspec(dllexport) bool InitializeMod() {
        Client::LogInfo("[CustomLocalizations] Initializing...");

        char dllPath[MAX_PATH] = { 0 };
        HMODULE hMod = GetModuleHandleA("CustomLocalizations.asi");
        if (!hMod) {
            Client::LogError("[CustomLocalizations] Failed to get module handle for the mod DLL!");
            return false;
        }
        GetModuleFileNameA(hMod, dllPath, MAX_PATH);

        std::string modFullPath = dllPath;
        size_t lastSlash = modFullPath.find_last_of("\\/");
        std::string modDirectory = (lastSlash != std::string::npos) ? modFullPath.substr(0, lastSlash + 1) : "";

        if (modDirectory.empty()) {
            Client::LogError("[CustomLocalizations] Could not determine mod directory path!");
            return false;
        }

        std::string customLocRootPath = modDirectory + "CustomLocalizations\\";

        if (!CustomLocalizations::LocalizationManager::getInstance().initialize(customLocRootPath)) {
            Client::LogError("[CustomLocalizations] LocalizationManager initialization failed!");
            return false;
        }

        // Imposta l'hook che intercetta .ntext
        CustomLocalizations::SetupStringLookupHook(HookCrashers::API::Client::GetModuleBase());

        // Registra la funzione che l'SWF userà per ottenere gli ID
        CustomLocalizations::RegisterGetLocalizationFunction();

        Client::LogInfo("[CustomLocalizations] Successfully initialized.");
        return true;
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
    }
    return TRUE;
}