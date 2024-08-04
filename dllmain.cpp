#include "utils.h"
#include "logger.h"
#include "stdafx.h"
#include "stringlocs.h"

DWORD WINAPI InitThread(LPVOID lpParam) {
    uintptr_t base = reinterpret_cast<uintptr_t>(lpParam);
    InitFlow(base);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID /*lpReserved*/)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        Logger l;
        l.Get()->info("DLL_PROCESS_ATTACH");
        l.Get()->flush();

        uintptr_t base = reinterpret_cast<uintptr_t>(GetModuleHandleA(NULL));
        IMAGE_DOS_HEADER* dos = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
        IMAGE_NT_HEADERS* nt = reinterpret_cast<IMAGE_NT_HEADERS*>(base + dos->e_lfanew);

        if ((base + nt->OptionalHeader.AddressOfEntryPoint + (0x400000 - base)) == 0x730310)
        {
            l.Get()->info("Correct EXE version detected. Creating initialization thread.");

            // Create a new thread for delayed initialization using CreateThread
            HANDLE hThread = CreateThread(NULL, 0, InitThread, reinterpret_cast<LPVOID>(base), 0, NULL);
            if (hThread)
            {
                CloseHandle(hThread);  // We don't need to wait for the thread, so we can close the handle
            }
            else
            {
                l.Get()->error("Failed to create initialization thread.");
            }
            l.Get()->flush();
        }
        else
        {
            MessageBoxA(NULL, "This .exe is not supported.\nPlease use the Steam version", "Castle Crashers Custom Localizations", MB_ICONERROR);
            return FALSE;
        }
    }
    return TRUE;
}