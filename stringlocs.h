// stringlocs.h
#pragma once
#include <cstdint>
#include <json.hpp>
#include "logger.h"
#include <fstream>
#include "utils.h"

struct LocalizationEntry {
    int id;
    const wchar_t* languages[11];
};

void InitFlow(uintptr_t base);