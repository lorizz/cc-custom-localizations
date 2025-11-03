#include "StringLookupHook.h"
#include "../Core/LocalizationManager.h"
#include <detours.h>
#include <windows.h>
#include <HookCrashers.h>

namespace CustomLocalizations {

    constexpr uintptr_t STRING_LOOKUP_OFFSET = 0x123430; // FUN_00ad3430

    using OriginalStringLookup_t = uint16_t(__thiscall*)(void* thisPtr, int16_t stringId);
    static OriginalStringLookup_t g_originalLookup = nullptr;

    uint16_t __fastcall DetouredStringLookup(void* thisPtr, void* edx, int16_t stringId)
    {
        LocalizationManager& locManager = LocalizationManager::getInstance();

        if (locManager.isInitialized() && stringId >= 5000)
        {
            // --- ID CUSTOM -> USA LA NOSTRA TABELLA, BYPASSA L'ORIGINALE ---
            int custom_index = stringId - 5000;
            const wchar_t* custom_string = locManager.getStringByIndex(custom_index);

            // Pulisci i buffer interni dell'oggetto, come fa l'originale
            // (ignora per ora le chiamate a FUN_00aa44c0, potrebbero non essere necessarie)
            *(uintptr_t*)((char*)thisPtr + 0xF4) = 0;
            *(uintptr_t*)((char*)thisPtr + 0xF8) = 0;

            if (custom_string) {
                size_t len = wcslen(custom_string);

                // Metti il puntatore alla NOSTRA stringa nel campo che il gioco si aspetta
                *(const wchar_t**)((char*)thisPtr + 0xF8) = custom_string;
                // Metti la lunghezza nel campo corretto
                *(uint16_t*)((char*)thisPtr + 0xC8) = len;

                return len; // Restituisci la lunghezza, come fa l'originale
            }

            *(uint16_t*)((char*)thisPtr + 0xC8) = 0;
            return 0;
        }
        else
        {
            // --- ID VANILLA -> CHIAMA L'ORIGINALE ---
            if (g_originalLookup) {
                return g_originalLookup(thisPtr, stringId);
            }
            return 0;
        }
    }


    bool SetupStringLookupHook(uintptr_t moduleBase) {
        HookCrashers::LogInfo("Setting up StringLookup hook...");

        uintptr_t targetAddress = moduleBase + STRING_LOOKUP_OFFSET;
        g_originalLookup = reinterpret_cast<OriginalStringLookup_t>(targetAddress);

        if (!g_originalLookup) {
            HookCrashers::LogError("Target address for StringLookup is invalid.");
            return false;
        }

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        if (DetourAttach(&(PVOID&)g_originalLookup, DetouredStringLookup) != NO_ERROR) {
            HookCrashers::LogError("DetourAttach for StringLookup failed.");
            DetourTransactionAbort();
            return false;
        }
        if (DetourTransactionCommit() != NO_ERROR) {
            HookCrashers::LogError("DetourTransactionCommit for StringLookup failed.");
            return false;
        }

        HookCrashers::LogInfo("StringLookup hook attached successfully!");
        return true;
    }
}