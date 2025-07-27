#ifndef ADVENTURE_ENGINE_H
#define ADVENTURE_ENGINE_H

#include <stdbool.h>

#define MAX_STRING_LENGTH 256
#define MAX_DESCRIPTION_LENGTH 1024
#define MAX_EXITS 8
#define MAX_ITEMS 32
#define MAX_LOCATIONS 256
#define MAX_INVENTORY_ITEMS 64
#define MAX_FLAGS 128

typedef struct {
    char direction[32];
    char target_location[64];
} Exit;

typedef struct {
    char id[64];
    char title[MAX_STRING_LENGTH];
    char description[MAX_DESCRIPTION_LENGTH];
    char image_path[MAX_STRING_LENGTH];
    char first_visit_text[MAX_DESCRIPTION_LENGTH];
    bool visited;
    
    Exit exits[MAX_EXITS];
    int exits_count;
    
    char items[MAX_ITEMS][64];
    int items_count;
    
    // Game flags (simplified for now)
    char flags_required[MAX_FLAGS][64];
    bool flags_required_values[MAX_FLAGS];
    int flags_required_count;
    
    char flags_set[MAX_FLAGS][64];
    bool flags_set_values[MAX_FLAGS];
    int flags_set_count;
} Location;

typedef struct {
    char id[64];
    char name[MAX_STRING_LENGTH];
    char description[MAX_DESCRIPTION_LENGTH];
    bool takeable;
    bool useable;
    char use_text[MAX_DESCRIPTION_LENGTH];
} InventoryItem;

typedef struct {
    char title[MAX_STRING_LENGTH];
    char author[MAX_STRING_LENGTH];
    char description[MAX_DESCRIPTION_LENGTH];
    char version[32];
} GameMeta;

typedef struct {
    char inventory[MAX_INVENTORY_ITEMS][64];
    int inventory_count;
    char current_location[64];
    
    char flags[MAX_FLAGS][64];
    bool flag_values[MAX_FLAGS];
    int flags_count;
} Player;

typedef struct {
    GameMeta meta;
    char start_location[64];
    
    Location locations[MAX_LOCATIONS];
    int locations_count;
    
    InventoryItem inventory_items[MAX_INVENTORY_ITEMS];
    int inventory_items_count;
    
    char game_flags[MAX_FLAGS][64];
    bool game_flag_values[MAX_FLAGS];
    int game_flags_count;
    
    Player player;
} GameState;

// Function declarations
GameState* load_game(const char* filename);
void cleanup_game(GameState* game);
Location* get_location_by_id(GameState* game, const char* location_id);
Location* get_current_location(GameState* game);
bool move_player(GameState* game, const char* direction);
bool has_item(GameState* game, const char* item_id);
bool add_item_to_inventory(GameState* game, const char* item_id);
bool remove_item_from_inventory(GameState* game, const char* item_id);
bool get_flag(GameState* game, const char* flag_name);
void set_flag(GameState* game, const char* flag_name, bool value);
bool check_location_requirements(GameState* game, Location* location);

#endif // ADVENTURE_ENGINE_H 