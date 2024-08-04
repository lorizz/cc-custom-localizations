#include "utils.h"

bool SetMemoryProtection(void* address, SIZE_T size, DWORD newProtect, DWORD* oldProtect) {
    return VirtualProtect(address, size, newProtect, oldProtect);
}

std::string IntToHex(uintptr_t value)
{
    std::stringstream ss;
    ss << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << value;
    return ss.str();
}

uint32_t ReadLittleEndian(uintptr_t address) {
    uint8_t bytes[4];
    if (ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<LPCVOID>(address), bytes, sizeof(bytes), nullptr)) {
        return static_cast<uint32_t>(bytes[0]) |
            (static_cast<uint32_t>(bytes[1]) << 8) |
            (static_cast<uint32_t>(bytes[2]) << 16) |
            (static_cast<uint32_t>(bytes[3]) << 24);
    }
    return 0;
}

// Function to convert std::string to std::wstring
std::wstring ToWString(const std::string& str) {
    // Convert std::string (UTF-8) to std::wstring (UTF-16) using std::wstring_convert
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}

// Function to convert std::string to const wchar_t*
const wchar_t* ToWChar(const std::string& str) {
    std::wstring wstr = ToWString(str);
    return _wcsdup(wstr.c_str());  // Allocate and copy the wide string
}