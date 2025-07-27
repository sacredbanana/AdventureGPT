#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <cjson/cJSON.h>
#include "adventure_engine.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define TEXT_AREA_HEIGHT 200
#define MAX_INPUT_LENGTH 256

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;
    SDL_Texture *location_image;
    char input_buffer[MAX_INPUT_LENGTH];
    int input_length;
    bool running;
} GameRenderer;

GameState *game_state = NULL;
GameRenderer renderer = {0};

bool init_renderer() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        return false;
    }

    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        return false;
    }

    renderer.window = SDL_CreateWindow(
        "AdventureGPT Game Engine",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (renderer.window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    renderer.renderer = SDL_CreateRenderer(renderer.window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer.renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // Load default font (you may need to adjust the path)
    renderer.font = TTF_OpenFont("assets/fonts/default.ttf", 16);
    if (renderer.font == NULL) {
        // Try to use system font as fallback
        #ifdef __APPLE__
        renderer.font = TTF_OpenFont("/System/Library/Fonts/Geneva.ttf", 16);
        #elif __linux__
        renderer.font = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", 16);
        #elif _WIN32
        renderer.font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 16);
        #endif
        
        if (renderer.font == NULL) {
            printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
            return false;
        }
    }

    renderer.running = true;
    return true;
}

void cleanup_renderer() {
    if (renderer.location_image) {
        SDL_DestroyTexture(renderer.location_image);
    }
    if (renderer.font) {
        TTF_CloseFont(renderer.font);
    }
    if (renderer.renderer) {
        SDL_DestroyRenderer(renderer.renderer);
    }
    if (renderer.window) {
        SDL_DestroyWindow(renderer.window);
    }
    
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

SDL_Texture* load_location_image(const char* image_path) {
    if (!image_path || strlen(image_path) == 0) {
        return NULL;
    }
    
    SDL_Surface* surface = IMG_Load(image_path);
    if (!surface) {
        printf("Unable to load image %s! SDL_image Error: %s\n", image_path, IMG_GetError());
        return NULL;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer.renderer, surface);
    SDL_FreeSurface(surface);
    
    if (!texture) {
        printf("Unable to create texture from %s! SDL Error: %s\n", image_path, SDL_GetError());
    }
    
    return texture;
}

void render_text(const char* text, int x, int y, int max_width, SDL_Color color) {
    if (!text || strlen(text) == 0) return;
    
    // Simple word wrapping - split by words and render line by line
    char* text_copy = strdup(text);
    char* word = strtok(text_copy, " ");
    char line_buffer[512] = "";
    int line_y = y;
    int line_height = TTF_FontHeight(renderer.font);
    
    while (word != NULL) {
        char test_line[512];
        if (strlen(line_buffer) > 0) {
            snprintf(test_line, sizeof(test_line), "%s %s", line_buffer, word);
        } else {
            strncpy(test_line, word, sizeof(test_line) - 1);
            test_line[sizeof(test_line) - 1] = '\0';
        }
        
        int text_width;
        TTF_SizeText(renderer.font, test_line, &text_width, NULL);
        
        if (text_width <= max_width) {
            strncpy(line_buffer, test_line, sizeof(line_buffer) - 1);
            line_buffer[sizeof(line_buffer) - 1] = '\0';
        } else {
            // Render current line and start new one
            if (strlen(line_buffer) > 0) {
                SDL_Surface* text_surface = TTF_RenderText_Solid(renderer.font, line_buffer, color);
                if (text_surface) {
                    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer.renderer, text_surface);
                    if (text_texture) {
                        SDL_Rect dest_rect = {x, line_y, text_surface->w, text_surface->h};
                        SDL_RenderCopy(renderer.renderer, text_texture, NULL, &dest_rect);
                        SDL_DestroyTexture(text_texture);
                    }
                    SDL_FreeSurface(text_surface);
                }
                line_y += line_height + 2;
            }
            strncpy(line_buffer, word, sizeof(line_buffer) - 1);
            line_buffer[sizeof(line_buffer) - 1] = '\0';
        }
        
        word = strtok(NULL, " ");
    }
    
    // Render final line
    if (strlen(line_buffer) > 0) {
        SDL_Surface* text_surface = TTF_RenderText_Solid(renderer.font, line_buffer, color);
        if (text_surface) {
            SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer.renderer, text_surface);
            if (text_texture) {
                SDL_Rect dest_rect = {x, line_y, text_surface->w, text_surface->h};
                SDL_RenderCopy(renderer.renderer, text_texture, NULL, &dest_rect);
                SDL_DestroyTexture(text_texture);
            }
            SDL_FreeSurface(text_surface);
        }
    }
    
    free(text_copy);
}

void render_game() {
    // Clear screen
    SDL_SetRenderDrawColor(renderer.renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer.renderer);
    
    if (!game_state) return;
    
    Location* current_location = get_current_location(game_state);
    if (!current_location) return;
    
    // Render location image (top half of screen)
    if (renderer.location_image) {
        SDL_Rect image_rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT - TEXT_AREA_HEIGHT};
        SDL_RenderCopy(renderer.renderer, renderer.location_image, NULL, &image_rect);
    } else {
        // Draw placeholder rectangle for image area
        SDL_SetRenderDrawColor(renderer.renderer, 64, 64, 64, 255);
        SDL_Rect image_area = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT - TEXT_AREA_HEIGHT};
        SDL_RenderFillRect(renderer.renderer, &image_area);
        
        // Draw "No Image" text
        SDL_Color white = {255, 255, 255, 255};
        render_text("No Image Available", WINDOW_WIDTH/2 - 100, (WINDOW_HEIGHT - TEXT_AREA_HEIGHT)/2, 200, white);
    }
    
    // Draw separator line
    SDL_SetRenderDrawColor(renderer.renderer, 128, 128, 128, 255);
    SDL_RenderDrawLine(renderer.renderer, 0, WINDOW_HEIGHT - TEXT_AREA_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT - TEXT_AREA_HEIGHT);
    
    // Render text area (bottom portion)
    SDL_SetRenderDrawColor(renderer.renderer, 32, 32, 32, 255);
    SDL_Rect text_area = {0, WINDOW_HEIGHT - TEXT_AREA_HEIGHT, WINDOW_WIDTH, TEXT_AREA_HEIGHT};
    SDL_RenderFillRect(renderer.renderer, &text_area);
    
    // Render location title and description
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 0, 255};
    
    int text_y = WINDOW_HEIGHT - TEXT_AREA_HEIGHT + 10;
    
    // Title
    render_text(current_location->title, 10, text_y, WINDOW_WIDTH - 20, yellow);
    text_y += 30;
    
    // Description
    render_text(current_location->description, 10, text_y, WINDOW_WIDTH - 20, white);
    text_y += 60;
    
    // Show available exits
    if (current_location->exits_count > 0) {
        char exits_text[256] = "Exits: ";
        for (int i = 0; i < current_location->exits_count; i++) {
            if (i > 0) strcat(exits_text, ", ");
            strcat(exits_text, current_location->exits[i].direction);
        }
        render_text(exits_text, 10, text_y, WINDOW_WIDTH - 20, white);
        text_y += 25;
    }
    
    // Input prompt
    char prompt[512];
    snprintf(prompt, sizeof(prompt), "> %s", renderer.input_buffer);
    render_text(prompt, 10, WINDOW_HEIGHT - 30, WINDOW_WIDTH - 20, white);
    
    SDL_RenderPresent(renderer.renderer);
}

void handle_input(const char* input) {
    if (!game_state || !input) return;
    
    // Basic command parsing
    if (strncmp(input, "go ", 3) == 0 || strncmp(input, "move ", 5) == 0) {
        const char* direction = strchr(input, ' ') + 1;
        if (move_player(game_state, direction)) {
            // Load new location image
            if (renderer.location_image) {
                SDL_DestroyTexture(renderer.location_image);
                renderer.location_image = NULL;
            }
            
            Location* new_location = get_current_location(game_state);
            if (new_location && new_location->image_path[0] != '\0') {
                renderer.location_image = load_location_image(new_location->image_path);
            }
        }
    } else if (strcmp(input, "look") == 0 || strcmp(input, "l") == 0) {
        // Just re-render current location (already handled by render loop)
    } else if (strcmp(input, "inventory") == 0 || strcmp(input, "i") == 0) {
        // Show inventory (would need to implement inventory display)
        printf("Inventory system not yet implemented.\n");
    } else if (strcmp(input, "help") == 0) {
        printf("Available commands: go <direction>, look, inventory, help, quit\n");
    } else if (strcmp(input, "quit") == 0 || strcmp(input, "exit") == 0) {
        renderer.running = false;
    } else {
        printf("Unknown command: %s\n", input);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <game_file.advgpt>\n", argv[0]);
        return 1;
    }
    
    // Initialize renderer
    if (!init_renderer()) {
        printf("Failed to initialize renderer!\n");
        return 1;
    }
    
    // Load game
    game_state = load_game(argv[1]);
    if (!game_state) {
        printf("Failed to load game: %s\n", argv[1]);
        cleanup_renderer();
        return 1;
    }
    
    printf("Game loaded successfully!\n");
    printf("Title: %s\n", game_state->meta.title);
    printf("Author: %s\n", game_state->meta.author);
    printf("Starting location: %s\n", game_state->start_location);
    
    // Load initial location image
    Location* start_location = get_current_location(game_state);
    if (start_location && start_location->image_path[0] != '\0') {
        renderer.location_image = load_location_image(start_location->image_path);
    }
    
    // Main game loop
    SDL_Event e;
    while (renderer.running) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                renderer.running = false;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_RETURN) {
                    // Process input
                    if (renderer.input_length > 0) {
                        renderer.input_buffer[renderer.input_length] = '\0';
                        handle_input(renderer.input_buffer);
                        renderer.input_length = 0;
                        renderer.input_buffer[0] = '\0';
                    }
                } else if (e.key.keysym.sym == SDLK_BACKSPACE) {
                    if (renderer.input_length > 0) {
                        renderer.input_length--;
                        renderer.input_buffer[renderer.input_length] = '\0';
                    }
                }
            } else if (e.type == SDL_TEXTINPUT) {
                if (renderer.input_length < MAX_INPUT_LENGTH - 1) {
                    strcat(renderer.input_buffer, e.text.text);
                    renderer.input_length += strlen(e.text.text);
                }
            }
        }
        
        render_game();
        SDL_Delay(16); // ~60 FPS
    }
    
    // Cleanup
    cleanup_game(game_state);
    cleanup_renderer();
    
    printf("Game ended. Thanks for playing!\n");
    return 0;
} 