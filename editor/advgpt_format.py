"""
AdventureGPT Game Format Specification

This module defines the .advgpt format used for storing and exchanging
adventure game data between the Python editor and C engine.
"""

import json
from typing import Dict, List, Any, Optional
from pathlib import Path


class AdvGPTFormat:
    """
    Defines the .advgpt game format structure and provides utilities
    for creating, validating, and manipulating game data.
    """
    
    @staticmethod
    def create_empty_game() -> Dict[str, Any]:
        """Create an empty game structure with default values."""
        return {
            "meta": {
                "title": "Untitled Adventure",
                "author": "Unknown",
                "description": "",
                "version": "1.0",
                "created": "",
                "modified": ""
            },
            "start_location": "start",
            "locations": {
                "start": {
                    "title": "Starting Location",
                    "description": "You are at the beginning of your adventure.",
                    "image": "",
                    "exits": {},
                    "items": [],
                    "flags_required": {},
                    "flags_set": {},
                    "first_visit_text": "",
                    "visited": False
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
    
    @staticmethod
    def create_location(
        location_id: str,
        title: str,
        description: str,
        image: str = "",
        exits: Optional[Dict[str, str]] = None,
        items: Optional[List[str]] = None,
        flags_required: Optional[Dict[str, Any]] = None,
        flags_set: Optional[Dict[str, Any]] = None,
        first_visit_text: str = ""
    ) -> Dict[str, Any]:
        """Create a location structure."""
        return {
            "title": title,
            "description": description,
            "image": image,
            "exits": exits or {},
            "items": items or [],
            "flags_required": flags_required or {},
            "flags_set": flags_set or {},
            "first_visit_text": first_visit_text,
            "visited": False
        }
    
    @staticmethod
    def create_item(
        item_id: str,
        name: str,
        description: str,
        takeable: bool = True,
        useable: bool = False,
        use_text: str = "",
        use_flags_required: Optional[Dict[str, Any]] = None,
        use_flags_set: Optional[Dict[str, Any]] = None
    ) -> Dict[str, Any]:
        """Create an inventory item structure."""
        return {
            "name": name,
            "description": description,
            "takeable": takeable,
            "useable": useable,
            "use_text": use_text,
            "use_flags_required": use_flags_required or {},
            "use_flags_set": use_flags_set or {}
        }
    
    @staticmethod
    def validate_game_data(game_data: Dict[str, Any]) -> List[str]:
        """
        Validate game data structure and return list of errors.
        Returns empty list if valid.
        """
        errors = []
        
        # Check required top-level keys
        required_keys = ["meta", "start_location", "locations", "inventory_items", "game_flags", "player"]
        for key in required_keys:
            if key not in game_data:
                errors.append(f"Missing required key: {key}")
        
        if errors:
            return errors
        
        # Check meta section
        meta = game_data["meta"]
        meta_required = ["title", "author", "version"]
        for key in meta_required:
            if key not in meta:
                errors.append(f"Missing meta key: {key}")
        
        # Check start location exists
        start_location = game_data["start_location"]
        locations = game_data["locations"]
        
        if start_location not in locations:
            errors.append(f"Start location '{start_location}' not found in locations")
        
        # Validate each location
        for loc_id, location in locations.items():
            location_errors = AdvGPTFormat._validate_location(loc_id, location, locations)
            errors.extend(location_errors)
        
        # Check player data
        player = game_data["player"]
        player_required = ["inventory", "current_location", "flags"]
        for key in player_required:
            if key not in player:
                errors.append(f"Missing player key: {key}")
        
        if "current_location" in player and player["current_location"] not in locations:
            errors.append(f"Player current_location '{player['current_location']}' not found in locations")
        
        return errors
    
    @staticmethod
    def _validate_location(loc_id: str, location: Dict[str, Any], all_locations: Dict[str, Any]) -> List[str]:
        """Validate a single location."""
        errors = []
        
        # Check required location keys
        required_keys = ["title", "description", "exits"]
        for key in required_keys:
            if key not in location:
                errors.append(f"Location '{loc_id}' missing required key: {key}")
        
        # Check that all exits point to valid locations
        if "exits" in location:
            for direction, target_location in location["exits"].items():
                if target_location not in all_locations:
                    errors.append(f"Location '{loc_id}' exit '{direction}' points to non-existent location '{target_location}'")
        
        return errors
    
    @staticmethod
    def save_to_file(game_data: Dict[str, Any], file_path: str) -> bool:
        """Save game data to .advgpt file. Returns True on success."""
        try:
            # Validate before saving
            errors = AdvGPTFormat.validate_game_data(game_data)
            if errors:
                print(f"Validation errors: {errors}")
                return False
            
            with open(file_path, 'w', encoding='utf-8') as f:
                json.dump(game_data, f, indent=2, ensure_ascii=False)
            return True
        except Exception as e:
            print(f"Error saving game data: {e}")
            return False
    
    @staticmethod
    def load_from_file(file_path: str) -> Optional[Dict[str, Any]]:
        """Load game data from .advgpt file. Returns None on error."""
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                game_data = json.load(f)
            
            # Validate loaded data
            errors = AdvGPTFormat.validate_game_data(game_data)
            if errors:
                print(f"Loaded game data has validation errors: {errors}")
                return None
            
            return game_data
        except Exception as e:
            print(f"Error loading game data: {e}")
            return None
    
    @staticmethod
    def get_format_specification() -> str:
        """Return a human-readable format specification."""
        return """
AdventureGPT Game Format (.advgpt) Specification
===============================================

The .advgpt format is a JSON-based format for storing text adventure games.

Structure:
{
  "meta": {
    "title": "Game Title",
    "author": "Author Name",
    "description": "Game description",
    "version": "1.0",
    "created": "ISO timestamp",
    "modified": "ISO timestamp"
  },
  "start_location": "location_id",
  "locations": {
    "location_id": {
      "title": "Location Title",
      "description": "Location description",
      "image": "path/to/image.png",
      "exits": {
        "north": "other_location_id",
        "south": "another_location_id"
      },
      "items": ["item_id1", "item_id2"],
      "flags_required": {"flag_name": true},
      "flags_set": {"flag_name": true},
      "first_visit_text": "Text shown on first visit",
      "visited": false
    }
  },
  "inventory_items": {
    "item_id": {
      "name": "Item Name",
      "description": "Item description",
      "takeable": true,
      "useable": false,
      "use_text": "Text when item is used",
      "use_flags_required": {"flag_name": true},
      "use_flags_set": {"flag_name": true}
    }
  },
  "game_flags": {
    "flag_name": false
  },
  "player": {
    "inventory": ["item_id1"],
    "current_location": "location_id",
    "flags": {"personal_flag": true}
  }
}

Key Concepts:
- Locations are connected via exits (north, south, east, west, up, down, etc.)
- Items can be placed in locations and picked up by the player
- Flags control game state and can gate access to locations or enable actions
- Images are referenced by path and should be included in the game package
"""


# Example usage and testing
if __name__ == "__main__":
    # Create a sample game
    game = AdvGPTFormat.create_empty_game()
    
    # Add another location
    game["locations"]["tower_stairs"] = AdvGPTFormat.create_location(
        "tower_stairs",
        "Tower Stairs",
        "Ancient stone stairs spiral upward into darkness.",
        "tower_stairs.png",
        exits={"down": "start", "up": "tower_top"}
    )
    
    # Add tower top
    game["locations"]["tower_top"] = AdvGPTFormat.create_location(
        "tower_top",
        "Tower Top",
        "You stand atop the ancient tower, with a magnificent view of the lands below.",
        "tower_top.png",
        exits={"down": "tower_stairs"}
    )
    
    # Update start location exits
    game["locations"]["start"]["exits"]["north"] = "tower_stairs"
    game["locations"]["start"]["title"] = "Tower Base"
    game["locations"]["start"]["description"] = "A massive stone tower looms above you."
    
    # Add a sample item
    game["inventory_items"]["ancient_key"] = AdvGPTFormat.create_item(
        "ancient_key",
        "Ancient Key",
        "A weathered bronze key with mysterious runes.",
        takeable=True,
        useable=True,
        use_text="The key glows briefly with magical energy."
    )
    
    # Place the key in the tower top
    game["locations"]["tower_top"]["items"].append("ancient_key")
    
    # Update metadata
    game["meta"]["title"] = "Tower of Dreams"
    game["meta"]["author"] = "AdventureGPT"
    game["meta"]["description"] = "A simple adventure in an ancient tower."
    
    # Validate and display
    errors = AdvGPTFormat.validate_game_data(game)
    if errors:
        print("Validation errors:")
        for error in errors:
            print(f"  - {error}")
    else:
        print("Game data is valid!")
        print(json.dumps(game, indent=2)) 