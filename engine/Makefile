# AdventureGPT Engine Makefile
# Cross-platform build system for the C game engine

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = adventuregpt-engine

# Source files
SOURCES = main.c adventure_engine.c
OBJECTS = $(SOURCES:.c=.o)

# Libraries and includes
LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf -lcjson -lm
INCLUDES = -I.

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

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)

# Install dependencies (Linux/macOS)
install-deps:
ifeq ($(UNAME_S),Linux)
	@echo "Installing dependencies for Linux..."
	sudo apt-get update
	sudo apt-get install -y libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libcjson-dev
endif
ifeq ($(UNAME_S),Darwin)
	@echo "Installing dependencies for macOS..."
	@if command -v brew >/dev/null 2>&1; then \
		brew install sdl2 sdl2_image sdl2_ttf cjson; \
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
	@python3 ../editor/advgpt_format.py > ../games/sample/sample_game.advgpt
	@echo "Sample game created at ../games/sample/sample_game.advgpt"

# Test the engine with sample game
test: $(TARGET) sample-game
	./$(TARGET) ../games/sample/sample_game.advgpt

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
	@echo "  help         - Show this help message"
	@echo ""
	@echo "Platform: $(UNAME_S)"

.PHONY: all clean install-deps debug release sample-game test help 