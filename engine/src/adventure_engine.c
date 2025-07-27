#include "adventure_engine.h"
#include <json-c/json.h>
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

// Helper function to get string from json object
const char* get_json_string(json_object* obj, const char* key) {
    json_object* value;
    if (json_object_object_get_ex(obj, key, &value)) {
        if (json_object_is_type(value, json_type_string)) {
            return json_object_get_string(value);
        }
    }
    return NULL;
}

// Helper function to get boolean from json object
bool get_json_bool(json_object* obj, const char* key) {
    json_object* value;
    if (json_object_object_get_ex(obj, key, &value)) {
        if (json_object_is_type(value, json_type_boolean)) {
            return json_object_get_boolean(value);
        }
    }
    return false;
}

// Parse location from JSON
bool parse_location(json_object* location_json, Location* location, const char* location_id) {
    if (!location_json || !location) return false;
    
    // Set location ID
    safe_strcpy(location->id, location_id, sizeof(location->id));
    
    // Parse basic properties
    const char* title = get_json_string(location_json, "title");
    if (title) {
        safe_strcpy(location->title, title, sizeof(location->title));
    }
    
    const char* description = get_json_string(location_json, "description");
    if (description) {
        safe_strcpy(location->description, description, sizeof(location->description));
    }
    
    const char* image = get_json_string(location_json, "image");
    if (image) {
        safe_strcpy(location->image_path, image, sizeof(location->image_path));
    }
    
    const char* first_visit = get_json_string(location_json, "first_visit_text");
    if (first_visit) {
        safe_strcpy(location->first_visit_text, first_visit, sizeof(location->first_visit_text));
    }
    
    location->visited = get_json_bool(location_json, "visited");
    
    // Parse exits
    json_object* exits;
    if (json_object_object_get_ex(location_json, "exits", &exits)) {
        if (json_object_is_type(exits, json_type_object)) {
            location->exits_count = 0;
            
            // Iterate through exits object
            json_object_object_foreach(exits, direction, target_obj) {
                if (location->exits_count >= MAX_EXITS) break;
                
                if (json_object_is_type(target_obj, json_type_string)) {
                    safe_strcpy(location->exits[location->exits_count].direction, 
                               direction, sizeof(location->exits[location->exits_count].direction));
                    safe_strcpy(location->exits[location->exits_count].target_location, 
                               json_object_get_string(target_obj), 
                               sizeof(location->exits[location->exits_count].target_location));
                    location->exits_count++;
                }
            }
        }
    }
    
    // Parse items
    json_object* items;
    if (json_object_object_get_ex(location_json, "items", &items)) {
        if (json_object_is_type(items, json_type_array)) {
            location->items_count = 0;
            int array_len = json_object_array_length(items);
            
            for (int i = 0; i < array_len && location->items_count < MAX_ITEMS; i++) {
                json_object* item = json_object_array_get_idx(items, i);
                if (json_object_is_type(item, json_type_string)) {
                    safe_strcpy(location->items[location->items_count], 
                               json_object_get_string(item), 
                               sizeof(location->items[location->items_count]));
                    location->items_count++;
                }
            }
        }
    }
    
    return true;
}

// Parse inventory item from JSON
bool parse_inventory_item(json_object* item_json, InventoryItem* item, const char* item_id) {
    if (!item_json || !item) return false;
    
    safe_strcpy(item->id, item_id, sizeof(item->id));
    
    const char* name = get_json_string(item_json, "name");
    if (name) {
        safe_strcpy(item->name, name, sizeof(item->name));
    }
    
    const char* description = get_json_string(item_json, "description");
    if (description) {
        safe_strcpy(item->description, description, sizeof(item->description));
    }
    
    item->takeable = get_json_bool(item_json, "takeable");
    item->useable = get_json_bool(item_json, "useable");
    
    const char* use_text = get_json_string(item_json, "use_text");
    if (use_text) {
        safe_strcpy(item->use_text, use_text, sizeof(item->use_text));
    }
    
    return true;
}

GameState* load_game(const char* filename) {
    char* file_content = read_file(filename);
    if (!file_content) {
        return NULL;
    }
    
    json_object* json = json_tokener_parse(file_content);
    free(file_content);
    
    if (!json) {
        printf("Error: Invalid JSON in game file\n");
        return NULL;
    }
    
    GameState* game = calloc(1, sizeof(GameState));
    if (!game) {
        printf("Error: Could not allocate memory for game state\n");
        json_object_put(json);
        return NULL;
    }
    
    // Parse metadata
    json_object* meta;
    if (json_object_object_get_ex(json, "meta", &meta)) {
        const char* title = get_json_string(meta, "title");
        if (title) {
            safe_strcpy(game->meta.title, title, sizeof(game->meta.title));
        }
        
        const char* author = get_json_string(meta, "author");
        if (author) {
            safe_strcpy(game->meta.author, author, sizeof(game->meta.author));
        }
        
        const char* description = get_json_string(meta, "description");
        if (description) {
            safe_strcpy(game->meta.description, description, sizeof(game->meta.description));
        }
        
        const char* version = get_json_string(meta, "version");
        if (version) {
            safe_strcpy(game->meta.version, version, sizeof(game->meta.version));
        }
    }
    
    // Parse start location
    const char* start_location = get_json_string(json, "start_location");
    if (start_location) {
        safe_strcpy(game->start_location, start_location, sizeof(game->start_location));
    }
    
    // Parse locations
    json_object* locations;
    if (json_object_object_get_ex(json, "locations", &locations)) {
        if (json_object_is_type(locations, json_type_object)) {
            game->locations_count = 0;
            
            // Iterate through locations object
            json_object_object_foreach(locations, location_id, location_obj) {
                if (game->locations_count >= MAX_LOCATIONS) break;
                
                if (parse_location(location_obj, &game->locations[game->locations_count], location_id)) {
                    game->locations_count++;
                }
            }
        }
    }
    
    // Parse inventory items
    json_object* inventory_items;
    if (json_object_object_get_ex(json, "inventory_items", &inventory_items)) {
        if (json_object_is_type(inventory_items, json_type_object)) {
            game->inventory_items_count = 0;
            
            // Iterate through inventory items object
            json_object_object_foreach(inventory_items, item_id, item_obj) {
                if (game->inventory_items_count >= MAX_INVENTORY_ITEMS) break;
                
                if (parse_inventory_item(item_obj, &game->inventory_items[game->inventory_items_count], item_id)) {
                    game->inventory_items_count++;
                }
            }
        }
    }
    
    // Parse player data
    json_object* player;
    if (json_object_object_get_ex(json, "player", &player)) {
        const char* current_location = get_json_string(player, "current_location");
        if (current_location) {
            safe_strcpy(game->player.current_location, current_location, 
                       sizeof(game->player.current_location));
        } else {
            // Default to start location
            safe_strcpy(game->player.current_location, game->start_location, 
                       sizeof(game->player.current_location));
        }
        
        // Parse player inventory
        json_object* inventory;
        if (json_object_object_get_ex(player, "inventory", &inventory)) {
            if (json_object_is_type(inventory, json_type_array)) {
                game->player.inventory_count = 0;
                int array_len = json_object_array_length(inventory);
                
                for (int i = 0; i < array_len && game->player.inventory_count < MAX_INVENTORY_ITEMS; i++) {
                    json_object* item = json_object_array_get_idx(inventory, i);
                    if (json_object_is_type(item, json_type_string)) {
                        safe_strcpy(game->player.inventory[game->player.inventory_count], 
                                   json_object_get_string(item), 
                                   sizeof(game->player.inventory[game->player.inventory_count]));
                        game->player.inventory_count++;
                    }
                }
            }
        }
    } else {
        // Default player to start location
        safe_strcpy(game->player.current_location, game->start_location, 
                   sizeof(game->player.current_location));
    }
    
    json_object_put(json);
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