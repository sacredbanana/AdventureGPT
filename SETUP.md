# AdventureGPT Setup Guide

This guide will help you get AdventureGPT up and running on your system.

## System Requirements

### For the Python Editor
- Python 3.8 or higher
- pip (Python package manager)

### For the C Engine
- C compiler (GCC, Clang, or MSVC)
- SDL2 development libraries
- cJSON library

## Platform-Specific Setup

### macOS

1. **Install Homebrew** (if not already installed):
   ```bash
   /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
   ```

2. **Install Python dependencies**:
   ```bash
   pip3 install -r requirements.txt
   ```

3. **Install C engine dependencies**:
   ```bash
   cd engine
   make install-deps
   ```

4. **Build the engine**:
   ```bash
   make
   ```

### Linux (Ubuntu/Debian)

1. **Install system dependencies**:
   ```bash
   sudo apt-get update
   sudo apt-get install python3 python3-pip build-essential
   sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libcjson-dev
   ```

2. **Install Python dependencies**:
   ```bash
   pip3 install -r requirements.txt
   ```

3. **Build the engine**:
   ```bash
   cd engine
   make
   ```

### Windows

1. **Install Python** from python.org

2. **Install Python dependencies**:
   ```cmd
   pip install -r requirements.txt
   ```

3. **For the C engine**, you have several options:
   - **Option A: Use vcpkg** (recommended)
     ```cmd
     vcpkg install sdl2 sdl2-image sdl2-ttf cjson
     ```
   - **Option B: Use MSYS2/MinGW**
     ```bash
     pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-cjson
     ```

4. **Build the engine** (adjust paths as needed):
   ```cmd
   cd engine
   make  # or use Visual Studio
   ```

## Quick Start

1. **Test the Python editor**:
   ```bash
   cd editor
   python3 main.py
   ```

2. **Test the C engine**:
   ```bash
   cd engine
   ./adventuregpt-engine ../games/sample/sample_game.advgpt
   ```

## Creating Your First Game

1. Launch the editor: `python3 editor/main.py`
2. Use the **Story Editor** tab to set game metadata
3. Use the **Map Editor** tab to create locations and set up connections
4. Use the **Export** tab to save your game as a `.advgpt` file
5. Test your game: `./engine/adventuregpt-engine your_game.advgpt`

## Troubleshooting

### Common Issues

**"No module named 'PySide6'"**
- Solution: Run `pip3 install -r requirements.txt`

**"SDL could not initialize"**
- Solution: Make sure SDL2 libraries are installed and in your system PATH

**"Failed to load font"**
- The engine tries to load system fonts. If it fails, place a TTF font file in `assets/fonts/default.ttf`

**Build errors on macOS**
- Make sure Xcode command line tools are installed: `xcode-select --install`

**Build errors on Linux**
- Make sure you have the development packages: `sudo apt-get install build-essential`

### Getting Help

- Check the main README.md for detailed documentation
- Look at the sample game in `games/sample/` for format examples
- The `advgpt_format.py` file contains the complete format specification

## Development Setup

If you want to contribute to AdventureGPT:

1. Fork the repository
2. Set up the development environment as above
3. Make changes and test thoroughly
4. Run the test suite: `cd engine && make test`
5. Submit a pull request

## Next Steps

- Try creating a simple adventure game
- Experiment with the inventory and flag systems
- Look into adding images to your locations
- Share your games with others! 