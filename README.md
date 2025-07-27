# AdventureGPT

A multiplatform tool for creating and playing text adventure games with static images. AdventureGPT consists of a Python-based visual editor and a lightweight C game engine for cross-platform game playback.

## Features

- **Visual Game Editor** (Python + PySide6)
  - Location editor with image support
  - Story and dialogue management
  - Inventory and flag system
  - Export to multiple platforms

- **Lightweight Game Engine** (C + SDL2)
  - Cross-platform support (Windows, macOS, Linux)
  - Future AmigaOS compatibility
  - Efficient .advgpt file format
  - Image and text rendering

- **Game Format**
  - JSON-based .advgpt format
  - Portable game files
  - Image assets bundling
  - Save/load functionality

## Project Structure

```
AdventureGPT/
├── editor/                    # Python-based game editor
│   ├── main.py               # Main editor application
│   ├── advgpt_format.py      # Game format specification
│   └── ...
├── engine/                   # C-based game engine
│   ├── main.c                # Engine entry point
│   ├── adventure_engine.h    # Engine header
│   ├── adventure_engine.c    # Engine implementation
│   └── Makefile              # Build system
├── games/                    # Sample and exported games
├── assets/                   # Shared assets
├── requirements.txt          # Python dependencies
└── README.md
```

## Quick Start

### Prerequisites

**For the Python Editor:**
- Python 3.8+
- PySide6
- Pillow

**For the C Engine:**
- GCC or compatible C compiler
- SDL2, SDL2_image, SDL2_ttf
- cJSON library

### Installation

1. **Clone the repository:**
   ```bash
   git clone <repository-url>
   cd AdventureGPT
   ```

2. **Set up Python environment:**
   ```bash
   pip install -r requirements.txt
   ```

3. **Install C engine dependencies:**
   ```bash
   cd engine
   make install-deps  # Linux/macOS only
   ```

   **Manual installation:**
   - **Ubuntu/Debian:** `sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libcjson-dev`
   - **macOS:** `brew install sdl2 sdl2_image sdl2_ttf cjson`
   - **Windows:** Install libraries via vcpkg or manual setup

4. **Build the engine:**
   ```bash
   make
   ```

### Usage

#### Running the Editor

```bash
cd editor
python main.py
```

The editor provides three main tabs:
- **Map Editor**: Create and edit game locations
- **Story Editor**: Write game narrative and manage metadata
- **Export**: Generate game files and executables

#### Playing Games

```bash
cd engine
./adventuregpt-engine path/to/game.advgpt
```

**Game Controls:**
- `go <direction>` or `move <direction>` - Move between locations
- `look` or `l` - Examine current location
- `inventory` or `i` - Check inventory
- `help` - Show available commands
- `quit` or `exit` - Exit game

#### Creating Your First Game

1. Launch the editor
2. Go to **Story Editor** and set game metadata
3. Switch to **Map Editor** and create locations:
   - Add locations using "Add Location"
   - Set titles and descriptions
   - Configure exits between locations
   - Load or generate images
4. Export your game using the **Export** tab
5. Test with the engine: `./adventuregpt-engine your_game.advgpt`

## Game Format (.advgpt)

AdventureGPT uses a JSON-based format for storing games:

```json
{
  "meta": {
    "title": "Your Adventure",
    "author": "Your Name",
    "description": "Game description"
  },
  "start_location": "start",
  "locations": {
    "start": {
      "title": "Starting Room",
      "description": "You are in a room.",
      "image": "start.png",
      "exits": {
        "north": "next_room"
      }
    }
  },
  "inventory_items": {},
  "game_flags": {},
  "player": {
    "inventory": [],
    "current_location": "start",
    "flags": {}
  }
}
```

## Development

### Building

```bash
# Debug build
make debug

# Release build
make release

# Clean build artifacts
make clean
```

### Testing

```bash
# Create and test with sample game
make test
```

### Platform Support

**Current:**
- ✅ Windows (x64)
- ✅ macOS (Intel/Apple Silicon)
- ✅ Linux (x64)

**Planned:**
- 🔄 AmigaOS (future)
- 🔄 Web (WASM port)

## Architecture

The project uses a split architecture:

- **Editor (Python)**: Rapid development, rich UI, AI integration capabilities
- **Engine (C)**: Lightweight, portable, suitable for retro systems
- **Format (.advgpt)**: JSON-based interchange format

This design allows games created in the modern editor to run on both contemporary and retro systems.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## License

[License information to be added]

## Roadmap

- [ ] AI-powered image generation
- [ ] Sound and music support
- [ ] Scripting system for complex interactions
- [ ] Visual map editor with drag-and-drop
- [ ] AmigaOS port
- [ ] Game packaging and distribution tools
- [ ] Multiplayer adventure support
