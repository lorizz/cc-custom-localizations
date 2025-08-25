# Custom Localizations for Castle Crashers

## Overview

**Custom Localizations** is a powerful modding framework extension for Castle Crashers that allows creators to add new, fully translated strings to the game. Built on top of the **[HookCrashers Modding Framework](https://github.com/lorizz/hookcrashers)**, this mod enables you to dynamically add your own text for character names, item descriptions, UI elements, and more, without overwriting existing game files.

This system is designed to be seamlessly integrated into your own mods, allowing you to create a professional and polished experience for players across all supported languages.

## Features

- **Dynamic String Injection:** Add new strings to the game without replacing the original localization table.
- **Full Language Support:** Provide translations for all 11 languages supported by Castle Crashers.
- **Easy SWF Integration:** A simple ActionScript function call is all you need to use your custom strings in Flash UI files.
- **Simple JSON Format:** Manage all your translations in a single, easy-to-read `strings.json` file.
- **Framework-Based:** Relies on the stability and power of HookCrashers for safe game integration.

## Installation

This mod requires the **HookCrashers Modding Framework** to be installed first.

1.  **Install HookCrashers:** Follow the installation instructions on the [HookCrashers GitHub page](https://github.com/lorizz/hookcrashers). Ensure it is working correctly.
2.  **Download CustomLocalizations:** Download the latest release from the releases page.
3.  **Install the Mod:**
    *   Navigate to your Castle Crashers root directory (e.g., `C:\Program Files (x86)\Steam\steamapps\common\CastleCrashers`).
    *   Go into the `mods` folder (created by HookCrashers).
    *   Place the `CustomLocalizations.asi` file inside the `mods` folder.
    *   Create a new folder named `CustomLocalizations` inside the `mods` folder.
4.  **Create your Strings File:** Inside the newly created `mods\CustomLocalizations` folder, create a file named `strings.json`.

Your final folder structure should look like this:
```
.../CastleCrashers/
└── mods/
    ├── CustomLocalizations.asi
    └── CustomLocalizations/
        └── strings.json
```

## How to Use: The `strings.json` File

This file is the heart of your custom translations. It's where you define all the new text you want to add to the game.

### Structure

The JSON file must have the following structure:

```json
{
  "base_custom_id": 5000,
  "strings": [
    {
      "id": "MY_MOD_TITLE",
      "english": "My Awesome Mod",
      "italian": "La Mia Mod Fantastica",
      "german": "Mein Tolles Mod",
      "french": "Mon Mod Génial",
      "spanish": "Mi Mod Impresionante",
      "schinese": "我的超棒模组",
      "korean": "내 멋진 모드",
      "tchinese": "我的超棒模組",
      "portuguese": "Meu Mod Incrível",
      "japanese": "私の素晴らしいMOD",
      "russian": "Мой Потрясающий Мод"
    }
  ]
}
```

### Field Explanation

-   **`"base_custom_id"`**: (Number) This is the starting numerical ID for your custom strings. The vanilla game uses IDs up to around 1200. Setting this to `5000` or higher ensures there are no conflicts with existing or future game strings.
-   **`"strings"`**: (Array) This is an array containing all your custom string objects.
-   **`"id"`**: (String) This is your unique **logical ID** for the string. You will use this exact name in your SWF/ActionScript code to retrieve the string. Use clear, descriptive names (e.g., `MY_CHARACTER_NAME`, `MENU_OPTION_1`).
-   **`"english"`, `"italian"`, etc.**: (String) These fields contain the actual translation for each language. The supported language keys are:
    -   `english`
    -   `german`
    -   `french`
    -   `spanish`
    -   `italian`
    -   `schinese` (Simplified Chinese)
    -   `korean`
    -   `tchinese` (Traditional Chinese)
    -   `portuguese`
    -   `russian`
    -   `japanese`
    
    **Note:** You do not need to provide a translation for every language. If a language key is missing, an empty string will be used as a fallback.

## Usage in ActionScript (SWF Files)

To display your custom strings in the game's UI, you need to use the global ActionScript function provided by this mod: `GetLocalization()`.

**Workflow:**
1.  Call `GetLocalization("your_logical_id")` to get the numerical ID that the game engine can understand.
2.  Assign this numerical ID to the `.ntext` property of a TextField object.

### Example

Let's say you have a TextField in your Flash file with the instance name `title_txt`. To set its text using your custom string, you would use the following ActionScript 2.0 code:

```actionscript
// 1. Get the numerical ID by calling the global function with your logical ID.
var myTitleID = GetLocalization("MY_MOD_TITLE");

// 2. Assign the returned number to the .ntext property of your TextField.
// The game will now automatically look up this ID and display the correct translation.
title_txt.ntext = myTitleID;

// You can also do it in one line:
// description_txt.ntext = GetLocalization("WEAPON_FROG_NAME");
```

That's it! The system handles the rest, ensuring the correct translation is shown based on the player's language settings in Steam.