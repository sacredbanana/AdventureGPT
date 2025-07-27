#include "adventure_engine.h"
#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper function to safely copy strings
void safe_strcpy(char* dest, const char* src, size_t dest_size) {
    if (!src) {
        dest[0] = '\0';
        return;
    }
    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';
}

// Helper function to read file contents
char* read_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        return NULL;
    }
    
    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Allocate buffer
    char* buffer = malloc(file_size + 1);
    if (!buffer) {
        printf("Error: Could not allocate memory for file\n");
        fclose(file);
        return NULL;
    }
    
    // Read file
    size_t bytes_read = fread(buffer, 1, file_size, file);
    buffer[bytes_read] = '\0';
    
    fclose(file);
    return buffer;
}

// Parse location from JSON
bool parse_location(cJSON* location_json, Location* location, const char* location_id) {
    if (!location_json || !location) return false;
    
    // Set location ID
    safe_strcpy(location->id, location_id, sizeof(location->id));
    
    // Parse basic properties
    cJSON* title = cJSON_GetObjectItem(location_json, "title");
    if (title && cJSON_IsString(title)) {
        safe_strcpy(location->title, title->valuestring, sizeof(location->title));
    }
    
    cJSON* description = cJSON_GetObjectItem(location_json, "description");
    if (description && cJSON_IsString(description)) {
        safe_strcpy(location->description, description->valuestring, sizeof(location->description));
    }
    
    cJSON* image = cJSON_GetObjectItem(location_json, "image");
    if (image && cJSON_IsString(image)) {
        safe_strcpy(location->image_path, image->valuestring, sizeof(location->image_path));
    }
    
    cJSON* first_visit = cJSON_GetObjectItem(location_json, "first_visit_text");
    if (first_visit && cJSON_IsString(first_visit)) {
        safe_strcpy(location->first_visit_text, first_visit->valuestring, sizeof(location->first_visit_text));
    }
    
    cJSON* visited = cJSON_GetObjectItem(location_json, "visited");
    if (visited && cJSON_IsBool(visited)) {
        location->visited = cJSON_IsTrue(visited);
    }
    
    // Parse exits
    cJSON* exits = cJSON_GetObjectItem(location_json, "exits");
    if (exits && cJSON_IsObject(exits)) {
        location->exits_count = 0;
        cJSON* exit = NULL;
        cJSON_ArrayForEach(exit, exits) {
            if (location->exits_count >= MAX_EXITS) break;
            
            safe_strcpy(location->exits[location->exits_count].direction, 
                       exit->string, sizeof(location->exits[location->exits_count].direction));
            safe_strcpy(location->exits[location->exits_count].target_location, 
                       exit->valuestring, sizeof(location->exits[location->exits_count].target_location));
            location->exits_count++;
        }
    }
    
    // Parse items
    cJSON* items = cJSON_GetObjectItem(location_json, "items");
    if (items && cJSON_IsArray(items)) {
        location->items_count = 0;
        cJSON* item = NULL;
        cJSON_ArrayForEach(item, items) {
            if (location->items_count >= MAX_ITEMS) break;
            if (cJSON_IsString(item)) {
                safe_strcpy(location->items[location->items_count], 
                           item->valuestring, sizeof(location->items[location->items_count]));
                location->items_count++;
            }
        }
    }
    
    return true;
}

// Parse inventory item from JSON
bool parse_inventory_item(cJSON* item_json, InventoryItem* item, const char* item_id) {
    if (!item_json || !item) return false;
    
    safe_strcpy(item->id, item_id, sizeof(item->id));
    
    cJSON* name = cJSON_GetObjectItem(item_json, "name");
    if (name && cJSON_IsString(name)) {
        safe_strcpy(item->name, name->valuestring, sizeof(item->name));
    }
    
    cJSON* description = cJSON_GetObjectItem(item_json, "description");
    if (description && cJSON_IsString(description)) {
        safe_strcpy(item->description, description->valuestring, sizeof(item->description));
    }
    
    cJSON* takeable = cJSON_GetObjectItem(item_json, "takeable");
    if (takeable && cJSON_IsBool(takeable)) {
        item->takeable = cJSON_IsTrue(takeable);
    }
    
    cJSON* useable = cJSON_GetObjectItem(item_json, "useable");
    if (useable && cJSON_IsBool(useable)) {
        item->useable = cJSON_IsTrue(useable);
    }
    
    cJSON* use_text = cJSON_GetObjectItem(item_json, "use_text");
    if (use_text && cJSON_IsString(use_text)) {
        safe_strcpy(item->use_text, use_text->valuestring, sizeof(item->use_text));
    }
    
    return true;
}

GameState* load_game(const char* filename) {
    char* file_content = read_file(filename);
    if (!file_content) {
        return NULL;
    }
    
    cJSON* json = cJSON_Parse(file_content);
    free(file_content);
    
    if (!json) {
        printf("Error: Invalid JSON in game file\n");
        return NULL;
    }
    
    GameState* game = calloc(1, sizeof(GameState));
    if (!game) {
        printf("Error: Could not allocate memory for game state\n");
        cJSON_Delete(json);
        return NULL;
    }
    
    // Parse metadata
    cJSON* meta = cJSON_GetObjectItem(json, "meta");
    if (meta) {
        cJSON* title = cJSON_GetObjectItem(meta, "title");
        if (title && cJSON_IsString(title)) {
            safe_strcpy(game->meta.title, title->valuestring, sizeof(game->meta.title));
        }
        
        cJSON* author = cJSON_GetObjectItem(meta, "author");
        if (author && cJSON_IsString(author)) {
            safe_strcpy(game->meta.author, author->valuestring, sizeof(game->meta.author));
        }
        
        cJSON* description = cJSON_GetObjectItem(meta, "description");
        if (description && cJSON_IsString(description)) {
            safe_strcpy(game->meta.description, description->valuestring, sizeof(game->meta.description));
        }
        
        cJSON* version = cJSON_GetObjectItem(meta, "version");
        if (version && cJSON_IsString(version)) {
            safe_strcpy(game->meta.version, version->valuestring, sizeof(game->meta.version));
        }
    }
    
    // Parse start location
    cJSON* start_location = cJSON_GetObjectItem(json, "start_location");
    if (start_location && cJSON_IsString(start_location)) {
        safe_strcpy(game->start_location, start_location->valuestring, sizeof(game->start_location));
    }
    
    // Parse locations
    cJSON* locations = cJSON_GetObjectItem(json, "locations");
    if (locations && cJSON_IsObject(locations)) {
        game->locations_count = 0;
        cJSON* location = NULL;
        cJSON_ArrayForEach(location, locations) {
            if (game->locations_count >= MAX_LOCATIONS) break;
            
            if (parse_location(location, &game->locations[game->locations_count], location->string)) {
                game->locations_count++;
            }
        }
    }
    
    // Parse inventory items
    cJSON* inventory_items = cJSON_GetObjectItem(json, "inventory_items");
    if (inventory_items && cJSON_IsObject(inventory_items)) {
        game->inventory_items_count = 0;
        cJSON* item = NULL;
        cJSON_ArrayForEach(item, inventory_items) {
            if (game->inventory_items_count >= MAX_INVENTORY_ITEMS) break;
            
            if (parse_inventory_item(item, &game->inventory_items[game->inventory_items_count], item->string)) {
                game->inventory_items_count++;
            }
        }
    }
    
    // Parse player data
    cJSON* player = cJSON_GetObjectItem(json, "player");
    if (player) {
        cJSON* current_location = cJSON_GetObjectItem(player, "current_location");
        if (current_location && cJSON_IsString(current_location)) {
            safe_strcpy(game->player.current_location, current_location->valuestring, 
                       sizeof(game->player.current_location));
        } else {
            // Default to start location
            safe_strcpy(game->player.current_location, game->start_location, 
                       sizeof(game->player.current_location));
        }
        
        // Parse player inventory
        cJSON* inventory = cJSON_GetObjectItem(player, "inventory");
        if (inventory && cJSON_IsArray(inventory)) {
            game->player.inventory_count = 0;
            cJSON* item = NULL;
            cJSON_ArrayForEach(item, inventory) {
                if (game->player.inventory_count >= MAX_INVENTORY_ITEMS) break;
                if (cJSON_IsString(item)) {
                    safe_strcpy(game->player.inventory[game->player.inventory_count], 
                               item->valuestring, sizeof(game->player.inventory[game->player.inventory_count]));
                    game->player.inventory_count++;
                }
            }
        }
    } else {
        // Default player to start location
        safe_strcpy(game->player.current_location, game->start_location, 
                   sizeof(game->player.current_location));
    }
    
    cJSON_Delete(json);
    return game;
}

void cleanup_game(GameState* game) {
    if (game) {
        free(game);
    }
}

Location* get_location_by_id(GameState* game, const char* location_id) {
    if (!game || !location_id) return NULL;
    
    for (int i = 0; i < game->locations_count; i++) {
        if (strcmp(game->locations[i].id, location_id) == 0) {
            return &game->locations[i];
        }
    }
    
    return NULL;
}

Location* get_current_location(GameState* game) {
    if (!game) return NULL;
    return get_location_by_id(game, game->player.current_location);
}

bool move_player(GameState* game, const char* direction) {
    if (!game || !direction) return false;
    
    Location* current_location = get_current_location(game);
    if (!current_location) return false;
    
    // Find the exit in the specified direction
    for (int i = 0; i < current_location->exits_count; i++) {
        if (strcasecmp(current_location->exits[i].direction, direction) == 0) {
            // Check if target location exists
            Location* target_location = get_location_by_id(game, current_location->exits[i].target_location);
            if (target_location) {
                // Move player
                safe_strcpy(game->player.current_location, current_location->exits[i].target_location,
                           sizeof(game->player.current_location));
                
                // Mark new location as visited
                target_location->visited = true;
                
                printf("You go %s.\n", direction);
                return true;
            } else {
                printf("Error: Exit leads to non-existent location!\n");
                return false;
            }
        }
    }
    
    printf("You can't go %s from here.\n", direction);
    return false;
}

bool has_item(GameState* game, const char* item_id) {
    if (!game || !item_id) return false;
    
    for (int i = 0; i < game->player.inventory_count; i++) {
        if (strcmp(game->player.inventory[i], item_id) == 0) {
            return true;
        }
    }
    
    return false;
}

bool add_item_to_inventory(GameState* game, const char* item_id) {
    if (!game || !item_id) return false;
    
    if (game->player.inventory_count >= MAX_INVENTORY_ITEMS) {
        printf("Your inventory is full!\n");
        return false;
    }
    
    if (has_item(game, item_id)) {
        printf("You already have that item.\n");
        return false;
    }
    
    safe_strcpy(game->player.inventory[game->player.inventory_count], item_id,
               sizeof(game->player.inventory[game->player.inventory_count]));
    game->player.inventory_count++;
    
    return true;
}

bool remove_item_from_inventory(GameState* game, const char* item_id) {
    if (!game || !item_id) return false;
    
    for (int i = 0; i < game->player.inventory_count; i++) {
        if (strcmp(game->player.inventory[i], item_id) == 0) {
            // Shift remaining items down
            for (int j = i; j < game->player.inventory_count - 1; j++) {
                strcpy(game->player.inventory[j], game->player.inventory[j + 1]);
            }
            game->player.inventory_count--;
            return true;
        }
    }
    
    return false;
}

bool get_flag(GameState* game, const char* flag_name) {
    if (!game || !flag_name) return false;
    
    // Check player flags first
    for (int i = 0; i < game->player.flags_count; i++) {
        if (strcmp(game->player.flags[i], flag_name) == 0) {
            return game->player.flag_values[i];
        }
    }
    
    // Check game flags
    for (int i = 0; i < game->game_flags_count; i++) {
        if (strcmp(game->game_flags[i], flag_name) == 0) {
            return game->game_flag_values[i];
        }
    }
    
    return false; // Default to false if flag not found
}

void set_flag(GameState* game, const char* flag_name, bool value) {
    if (!game || !flag_name) return;
    
    // Try to update existing player flag first
    for (int i = 0; i < game->player.flags_count; i++) {
        if (strcmp(game->player.flags[i], flag_name) == 0) {
            game->player.flag_values[i] = value;
            return;
        }
    }
    
    // Try to update existing game flag
    for (int i = 0; i < game->game_flags_count; i++) {
        if (strcmp(game->game_flags[i], flag_name) == 0) {
            game->game_flag_values[i] = value;
            return;
        }
    }
    
    // Add new player flag if there's space
    if (game->player.flags_count < MAX_FLAGS) {
        safe_strcpy(game->player.flags[game->player.flags_count], flag_name,
                   sizeof(game->player.flags[game->player.flags_count]));
        game->player.flag_values[game->player.flags_count] = value;
        game->player.flags_count++;
    }
}

bool check_location_requirements(GameState* game, Location* location) {
    if (!game || !location) return true; // No requirements means accessible
    
    // Check all required flags
    for (int i = 0; i < location->flags_required_count; i++) {
        bool flag_value = get_flag(game, location->flags_required[i]);
        if (flag_value != location->flags_required_values[i]) {
            return false; // Requirement not met
        }
    }
    
    return true; // All requirements met
} 