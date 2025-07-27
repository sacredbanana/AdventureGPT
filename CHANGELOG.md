# AdventureGPT Changelog

## [Unreleased]

### Changed
- **BREAKING**: Switched from cJSON to json-c library for JSON parsing
  - Better Amiga compatibility (json-c has an official Amiga port)
  - Updated all JSON parsing code to use json-c API
  - Updated build system and documentation
  - No changes to .advgpt file format - remains fully compatible

- **Improved**: Reorganized C engine directory structure
  - Moved all source files (.c, .h) to `engine/src/` directory
  - Object files (.o) now built in `engine/build/` directory
  - Cleaner project organization and better build isolation
  - Updated Makefile with proper dependency tracking

### Technical Details
- Replaced `cJSON.h` includes with `json-c/json.h`
- Updated Makefile to link against `-ljson-c` instead of `-lcjson`
- Rewrote JSON parsing functions:
  - `cJSON_Parse()` → `json_tokener_parse()`
  - `cJSON_GetObjectItem()` → `json_object_object_get_ex()`
  - `cJSON_IsString()` → `json_object_is_type(obj, json_type_string)`
  - `cJSON_Delete()` → `json_object_put()`
  - Added helper functions for cleaner code
- Updated dependency installation instructions for all platforms

### Benefits
- **Amiga Support**: json-c has a maintained static library port for AmigaOS
- **Stability**: json-c is more widely used and actively maintained
- **Performance**: Similar performance characteristics with cleaner API
- **Future-proofing**: Better long-term support for retro platforms

### Migration Notes
For developers building from source:
- **macOS**: `brew uninstall cjson && brew install json-c`
- **Ubuntu/Debian**: `sudo apt-get install libjson-c-dev` (instead of libcjson-dev)
- **Windows**: Use `json-c` package in vcpkg or MSYS2

## [1.0.0] - Initial Release

### Added
- Python-based visual game editor with PySide6
- C-based game engine with SDL2 rendering
- .advgpt JSON game format specification
- Cross-platform build system
- Sample game for testing
- Comprehensive documentation 