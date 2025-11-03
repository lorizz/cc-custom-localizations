#include "stdafx.h"
#include "Core/LocalizationManager.h"
#include "SWF/GetLocalization.h"
#include <windows.h>
#include "Hooks/StringLookupHook.h"
#include <HookCrashers.h>

extern "C" {
    __declspec(dllexport) const char* GetModName() { return "Custom Localizations"; }
    __declspec(dllexport) const char* GetModAuthor() { return "ilVonBurza"; }
    __declspec(dllexport) const char* GetModVersion() { return "2.0"; }

    __declspec(dllexport) bool InitializeMod() {
        HookCrashers::LogInfo("[CustomLocalizations] Initializing...");

        char dllPath[MAX_PATH] = { 0 };
        HMODULE hMod = GetModuleHandleA("CustomLocalizations.asi");
        if (!hMod) {
            HookCrashers::LogError("[CustomLocalizations] Failed to get module handle for the mod DLL!");
            return false;
        }
        GetModuleFileNameA(hMod, dllPath, MAX_PATH);

        std::string modFullPath = dllPath;
        size_t lastSlash = modFullPath.find_last_of("\\/");
        std::string modDirectory = (lastSlash != std::string::npos) ? modFullPath.substr(0, lastSlash + 1) : "";

        if (modDirectory.empty()) {
            HookCrashers::LogError("[CustomLocalizations] Could not determine mod directory path!");
            return false;
        }

        std::string customLocRootPath = modDirectory + "CustomLocalizations\\";

        if (!CustomLocalizations::LocalizationManager::getInstance().initialize(customLocRootPath)) {
            HookCrashers::LogError("[CustomLocalizations] LocalizationManager initialization failed!");
            return false;
        }

        // Imposta l'hook che intercetta .ntext
        CustomLocalizations::SetupStringLookupHook(HookCrashers::GetModuleBase());

        // Registra la funzione che l'SWF userà per ottenere gli ID
        CustomLocalizations::RegisterGetLocalizationFunction();

        HookCrashers::LogInfo("[CustomLocalizations] Successfully initialized.");
        return true;
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
    }
    return TRUE;
}