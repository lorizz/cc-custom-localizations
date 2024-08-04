# CustomLocalizations Mod

**CustomLocalizations** is a mod for Castle Crashers that allows you to add custom string localizations to the game's `ntext` field in SWF files. The mod injects a new function into the ntext parsing process, enabling support for custom localizations if the `ntext` ID is 828 or higher.

## Installation

To install the CustomLocalizations mod:

1. **Download the mod files**:
   - Under the release section, download the lastest version.

2. **Place the files in the game's root directory**:
   - Unzip the release into the root directory of Castle Crashers, where the game executable is located.

## Creating Custom Localizations

To create your own custom localizations, you need to provide a JSON file following this structure:

```json
[
  {
    "id": 828,
    "englishStr": "Critical Rate",
    "germanStr": "Kritische Trefferquote",
    "frenchStr": "Taux de Critique",
    "spanishStr": "Tasa de Críticos",
    "italianStr": "Tasso Critico",
    "simplifiedChineseStr": "暴击率",
    "koreanStr": "치명타 확률",
    "tradChineseStr": "暴擊率",
    "portugueseStr": "Taxa de Crítico",
    "japaneseStr": "クリティカル率",
    "russianStr": "Рейтинг Крита"
  },
  {
    "id": 829,
    "englishStr": "Critical Damage",
    "germanStr": "Kritischer Schaden",
    "frenchStr": "Dégâts Critiques",
    "spanishStr": "Daño Crítico",
    "italianStr": "Danno Critico",
    "simplifiedChineseStr": "暴击伤害",
    "koreanStr": "치명타 피해",
    "tradChineseStr": "暴擊傷害",
    "portugueseStr": "Dano Crítico",
    "japaneseStr": "クリティカルダメージ",
    "russianStr": "Критический Урон"
  }
]

### Notes:

- **ID Requirement**: Ensure that the `id` field in your JSON file is 828 or higher. IDs below 828 are reserved, and overwriting them may cause malfunctioning or crashes, as this scenario has not been extensively tested.
- **File Location**: Place your JSON file in the `scripts` folder alongside other mod scripts.

## Troubleshooting

- **Game Crashes**: If the game crashes or behaves unexpectedly, verify that your JSON file is correctly formatted and that all `id` values are 828 or higher.
- **Localization Issues**: Ensure that all required language fields are included and properly formatted in your JSON file.

## Contributing

If you have suggestions, bug reports, or want to contribute to the development of CustomLocalizations, feel free to open an issue or submit a pull request.

## License

This mod is provided under the [MIT License](LICENSE).
