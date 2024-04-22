
# BeeEngine.Localization Class

## Overview
The `BeeEngine.Localization` class provides functionality for retrieving translated text strings for a specified locale. It supports dynamic string localization with variable substitution and plural forms.

## Properties

### `Locale`
- **Type**: `string`
- **Description**: Gets or sets the current locale for translation. The locale should be in the format of a language code (`en`), optionally followed by a region code (`US`) in the format `en_US`.
- **Exceptions**:
  - `ArgumentNullException`: Thrown if a null value is set.
  - `ArgumentException`: Thrown if the locale format is incorrect.

## Methods

### `Translate(string key)`
- **Returns**: `string`
- **Description**: Returns the translated string for the current locale using the specified key.
- **Parameters**:
  - `key`: The key for the translated string.
- **Exceptions**:
  - `ArgumentNullException`: Thrown if `key` is null.
  - Returns the key itself if the translation is missing or an error occurs.

### `Translate(string key, params object[] args)`
- **Returns**: `string`
- **Description**: Returns a formatted translated string using the specified key and substitution arguments.
- **Parameters**:
  - `key`: The key for the translated string.
  - `args`: An array of objects representing substitution pairs. Each pair consists of a placeholder name and its corresponding value. Name must be a `string` and value must be one of: `int`, `uint`, `short`, `ushort`, `long`, `ulong`, `byte`, `sbyte`, `bool`.
- **Exceptions**:
  - `ArgumentNullException`: Thrown if any argument or `key` is null.
  - `ArgumentException`: Thrown if args are incorrectly formatted or types do not match expected types.
- **Usage**:
  - Correct: `Localization.Translate("hello", "count", 2)` returns "Hello 2 Worlds!"
  - Error: `Localization.Translate("hello", "count")` throws `ArgumentException`.

## Usage Examples

```csharp
// Example 1: Correct usage
string greeting = Localization.Translate("hello", "count", 1);
Log.Info(greeting);  // Output: "Hello 1 World!"

// Example 2: Error handling
try {
    string faultyGreeting = Localization.Translate("hello", "count");
} catch (ArgumentException ex) {
    Log.Error("Argument error: {0}", ex.Message);
}
```

## Recommendations
- Do not cache the results of translation methods in your game. Always call them when you need a localized string to ensure up-to-date translations.
