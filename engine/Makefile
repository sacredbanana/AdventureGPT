# AdventureGPT Engine Makefile
# Cross-platform build system for the C game engine

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = adventuregpt-engine

# Directories
SRCDIR = src
BUILDDIR = build

# Source files (without path)
SOURCES = main.c adventure_engine.c
# Object files in build directory
OBJECTS = $(SOURCES:%.c=$(BUILDDIR)/%.o)

# Libraries and includes
LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf -ljson-c -lm
INCLUDES = -I$(SRCDIR)

# Platform-specific settings
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    CFLAGS += -D_GNU_SOURCE
    LIBS += -ldl
endif
ifeq ($(UNAME_S),Darwin)
    # macOS
    INCLUDES += -I/usr/local/include -I/opt/homebrew/include
    LDFLAGS += -L/usr/local/lib -L/opt/homebrew/lib
endif
ifeq ($(OS),Windows_NT)
    # Windows (MinGW)
    TARGET = adventuregpt-engine.exe
    LIBS += -lmingw32 -lSDL2main
endif

# Default target
all: $(TARGET)

# Build the main executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS) $(LIBS)

# Compile source files to build directory
$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Create build directory if it doesn't exist
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# Clean build artifacts
clean:
	rm -rf $(BUILDDIR) $(TARGET)

# Install dependencies (Linux/macOS)
install-deps:
ifeq ($(UNAME_S),Linux)
	@echo "Installing dependencies for Linux..."
	sudo apt-get update
	sudo apt-get install -y libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libjson-c-dev
endif
ifeq ($(UNAME_S),Darwin)
	@echo "Installing dependencies for macOS..."
	@if command -v brew >/dev/null 2>&1; then \
		brew install sdl2 sdl2_image sdl2_ttf json-c; \
	else \
		echo "Homebrew not found. Please install Homebrew first: https://brew.sh/"; \
		exit 1; \
	fi
endif

# Debug build
debug: CFLAGS += -DDEBUG -O0
debug: $(TARGET)

# Release build
release: CFLAGS += -O3 -DNDEBUG
release: $(TARGET)

# Create a sample game for testing
sample-game:
	@echo "Creating sample game..."
	@mkdir -p ../games/sample
	@python3 -c "from sys import path; path.append('../editor'); from advgpt_format import *; import json; game = AdvGPTFormat.create_empty_game(); game['locations']['tower_stairs'] = AdvGPTFormat.create_location('tower_stairs', 'Tower Stairs', 'Ancient stone stairs spiral upward into darkness.', 'tower_stairs.png', exits={'down': 'start', 'up': 'tower_top'}); game['locations']['tower_top'] = AdvGPTFormat.create_location('tower_top', 'Tower Top', 'You stand atop the ancient tower, with a magnificent view of the lands below.', 'tower_top.png', exits={'down': 'tower_stairs'}); game['locations']['start']['exits']['north'] = 'tower_stairs'; game['locations']['start']['title'] = 'Tower Base'; game['locations']['start']['description'] = 'A massive stone tower looms above you.'; game['meta']['title'] = 'Tower of Dreams'; game['meta']['author'] = 'AdventureGPT'; game['meta']['description'] = 'A simple adventure in an ancient tower.'; game['inventory_items']['ancient_key'] = AdvGPTFormat.create_item('ancient_key', 'Ancient Key', 'A weathered bronze key with mysterious runes.', takeable=True, useable=True, use_text='The key glows briefly with magical energy.'); game['locations']['tower_top']['items'].append('ancient_key'); print(json.dumps(game, indent=2))" > ../games/sample/sample_game.advgpt
	@echo "Sample game created at ../games/sample/sample_game.advgpt"

# Test the engine with sample game
test: $(TARGET) sample-game
	./$(TARGET) ../games/sample/sample_game.advgpt

# Rebuild everything from scratch
rebuild: clean all

# Check for source file changes
check-sources:
	@echo "Source files in $(SRCDIR):"
	@ls -la $(SRCDIR)/
	@echo ""
	@echo "Object files in $(BUILDDIR):"
	@ls -la $(BUILDDIR)/ 2>/dev/null || echo "No object files found (clean build)"

# Show build information
info:
	@echo "AdventureGPT Engine Build Information"
	@echo "====================================="
	@echo "Platform: $(UNAME_S)"
	@echo "Compiler: $(CC)"
	@echo "Source directory: $(SRCDIR)"
	@echo "Build directory: $(BUILDDIR)"
	@echo "Target: $(TARGET)"
	@echo "Sources: $(SOURCES)"
	@echo "Objects: $(OBJECTS)"
	@echo "CFLAGS: $(CFLAGS)"
	@echo "INCLUDES: $(INCLUDES)"
	@echo "LIBS: $(LIBS)"

# Help
help:
	@echo "AdventureGPT Engine Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  all          - Build the engine (default)"
	@echo "  clean        - Remove build artifacts"
	@echo "  debug        - Build with debug symbols"
	@echo "  release      - Build optimized release version"
	@echo "  install-deps - Install system dependencies"
	@echo "  sample-game  - Create a sample game for testing"
	@echo "  test         - Build and test with sample game"
	@echo "  rebuild      - Clean and rebuild everything"
	@echo "  check-sources - Show source and object file status"
	@echo "  info         - Show build configuration"
	@echo "  help         - Show this help message"
	@echo ""
	@echo "Directory structure:"
	@echo "  $(SRCDIR)/     - Source files (.c, .h)"
	@echo "  $(BUILDDIR)/   - Object files (.o)"
	@echo "  ./       - Final executable"

.PHONY: all clean install-deps debug release sample-game test rebuild check-sources info help 