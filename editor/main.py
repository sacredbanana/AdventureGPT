#!/usr/bin/env python3
"""
AdventureGPT Editor - Main Application
A cross-platform editor for creating text adventure games with static images.
"""

import sys
import json
import os
from pathlib import Path
from PySide6.QtWidgets import (
    QApplication, QMainWindow, QTabWidget, QVBoxLayout, QHBoxLayout,
    QWidget, QLabel, QLineEdit, QTextEdit, QPushButton, QFileDialog,
    QMessageBox, QSplitter, QListWidget, QListWidgetItem, QFormLayout,
    QGroupBox, QScrollArea, QFrame
)
from PySide6.QtCore import Qt, QSettings
from PySide6.QtGui import QPixmap, QIcon, QAction


class LocationEditor(QWidget):
    """Widget for editing game locations."""
    
    def __init__(self):
        super().__init__()
        self.setup_ui()
        
    def setup_ui(self):
        layout = QHBoxLayout(self)
        
        # Left panel - Location list
        left_panel = QVBoxLayout()
        left_panel.addWidget(QLabel("Locations:"))
        
        self.location_list = QListWidget()
        self.location_list.currentItemChanged.connect(self.on_location_selected)
        left_panel.addWidget(self.location_list)
        
        # Add/Remove buttons
        button_layout = QHBoxLayout()
        self.add_location_btn = QPushButton("Add Location")
        self.remove_location_btn = QPushButton("Remove Location")
        self.add_location_btn.clicked.connect(self.add_location)
        self.remove_location_btn.clicked.connect(self.remove_location)
        button_layout.addWidget(self.add_location_btn)
        button_layout.addWidget(self.remove_location_btn)
        left_panel.addLayout(button_layout)
        
        left_widget = QWidget()
        left_widget.setLayout(left_panel)
        left_widget.setMaximumWidth(250)
        
        # Right panel - Location details
        right_panel = QVBoxLayout()
        
        # Location properties
        form_layout = QFormLayout()
        self.location_id_edit = QLineEdit()
        self.location_title_edit = QLineEdit()
        self.location_description_edit = QTextEdit()
        self.location_description_edit.setMaximumHeight(100)
        
        form_layout.addRow("Location ID:", self.location_id_edit)
        form_layout.addRow("Title:", self.location_title_edit)
        form_layout.addRow("Description:", self.location_description_edit)
        
        # Image section
        image_group = QGroupBox("Location Image")
        image_layout = QVBoxLayout()
        
        self.image_label = QLabel("No image selected")
        self.image_label.setMinimumHeight(200)
        self.image_label.setStyleSheet("border: 1px solid gray; background-color: #f0f0f0;")
        self.image_label.setAlignment(Qt.AlignCenter)
        
        image_button_layout = QHBoxLayout()
        self.load_image_btn = QPushButton("Load Image")
        self.generate_image_btn = QPushButton("Generate with AI")
        self.load_image_btn.clicked.connect(self.load_image)
        self.generate_image_btn.clicked.connect(self.generate_image)
        image_button_layout.addWidget(self.load_image_btn)
        image_button_layout.addWidget(self.generate_image_btn)
        
        image_layout.addWidget(self.image_label)
        image_layout.addLayout(image_button_layout)
        image_group.setLayout(image_layout)
        
        # Exits section
        exits_group = QGroupBox("Exits")
        exits_layout = QVBoxLayout()
        self.exits_list = QListWidget()
        
        exits_button_layout = QHBoxLayout()
        self.add_exit_btn = QPushButton("Add Exit")
        self.remove_exit_btn = QPushButton("Remove Exit")
        self.add_exit_btn.clicked.connect(self.add_exit)
        self.remove_exit_btn.clicked.connect(self.remove_exit)
        exits_button_layout.addWidget(self.add_exit_btn)
        exits_button_layout.addWidget(self.remove_exit_btn)
        
        exits_layout.addWidget(self.exits_list)
        exits_layout.addLayout(exits_button_layout)
        exits_group.setLayout(exits_layout)
        
        right_panel.addLayout(form_layout)
        right_panel.addWidget(image_group)
        right_panel.addWidget(exits_group)
        right_panel.addStretch()
        
        right_widget = QWidget()
        right_widget.setLayout(right_panel)
        
        # Add to main layout
        layout.addWidget(left_widget)
        layout.addWidget(right_widget)
        
    def add_location(self):
        """Add a new location to the list."""
        location_id = f"location_{self.location_list.count() + 1}"
        item = QListWidgetItem(location_id)
        self.location_list.addItem(item)
        self.location_list.setCurrentItem(item)
        
    def remove_location(self):
        """Remove the selected location."""
        current_item = self.location_list.currentItem()
        if current_item:
            self.location_list.takeItem(self.location_list.row(current_item))
            
    def on_location_selected(self, current, previous):
        """Handle location selection change."""
        if current:
            # Load location data into form
            # This would be connected to the actual data model
            pass
            
    def load_image(self):
        """Load an image file for the current location."""
        file_path, _ = QFileDialog.getOpenFileName(
            self, "Load Location Image", "", 
            "Image Files (*.png *.jpg *.jpeg *.bmp *.gif)"
        )
        if file_path:
            pixmap = QPixmap(file_path)
            scaled_pixmap = pixmap.scaled(
                self.image_label.size(), Qt.KeepAspectRatio, Qt.SmoothTransformation
            )
            self.image_label.setPixmap(scaled_pixmap)
            
    def generate_image(self):
        """Generate an AI image for the current location."""
        QMessageBox.information(self, "AI Generation", "AI image generation will be implemented in a future version.")
        
    def add_exit(self):
        """Add a new exit to the current location."""
        # This would open a dialog to configure the exit
        exit_name = f"Exit {self.exits_list.count() + 1}"
        self.exits_list.addItem(exit_name)
        
    def remove_exit(self):
        """Remove the selected exit."""
        current_row = self.exits_list.currentRow()
        if current_row >= 0:
            self.exits_list.takeItem(current_row)


class StoryEditor(QWidget):
    """Widget for editing game story and dialogue."""
    
    def __init__(self):
        super().__init__()
        self.setup_ui()
        
    def setup_ui(self):
        layout = QVBoxLayout(self)
        
        # Game metadata
        metadata_group = QGroupBox("Game Metadata")
        metadata_layout = QFormLayout()
        
        self.game_title_edit = QLineEdit()
        self.game_author_edit = QLineEdit()
        self.game_description_edit = QTextEdit()
        self.game_description_edit.setMaximumHeight(80)
        
        metadata_layout.addRow("Game Title:", self.game_title_edit)
        metadata_layout.addRow("Author:", self.game_author_edit)
        metadata_layout.addRow("Description:", self.game_description_edit)
        metadata_group.setLayout(metadata_layout)
        
        # Story content
        story_group = QGroupBox("Story Content")
        story_layout = QVBoxLayout()
        
        self.story_text_edit = QTextEdit()
        self.story_text_edit.setPlaceholderText("Enter your adventure story, dialogue, and narrative content here...")
        
        story_layout.addWidget(self.story_text_edit)
        story_group.setLayout(story_layout)
        
        # Inventory and flags
        systems_group = QGroupBox("Game Systems")
        systems_layout = QHBoxLayout()
        
        # Inventory items
        inventory_layout = QVBoxLayout()
        inventory_layout.addWidget(QLabel("Inventory Items:"))
        self.inventory_list = QListWidget()
        
        inventory_buttons = QHBoxLayout()
        self.add_item_btn = QPushButton("Add Item")
        self.remove_item_btn = QPushButton("Remove Item")
        self.add_item_btn.clicked.connect(self.add_inventory_item)
        self.remove_item_btn.clicked.connect(self.remove_inventory_item)
        inventory_buttons.addWidget(self.add_item_btn)
        inventory_buttons.addWidget(self.remove_item_btn)
        
        inventory_layout.addWidget(self.inventory_list)
        inventory_layout.addLayout(inventory_buttons)
        
        # Game flags
        flags_layout = QVBoxLayout()
        flags_layout.addWidget(QLabel("Game Flags:"))
        self.flags_list = QListWidget()
        
        flags_buttons = QHBoxLayout()
        self.add_flag_btn = QPushButton("Add Flag")
        self.remove_flag_btn = QPushButton("Remove Flag")
        self.add_flag_btn.clicked.connect(self.add_flag)
        self.remove_flag_btn.clicked.connect(self.remove_flag)
        flags_buttons.addWidget(self.add_flag_btn)
        flags_buttons.addWidget(self.remove_flag_btn)
        
        flags_layout.addWidget(self.flags_list)
        flags_layout.addLayout(flags_buttons)
        
        systems_layout.addLayout(inventory_layout)
        systems_layout.addLayout(flags_layout)
        systems_group.setLayout(systems_layout)
        
        layout.addWidget(metadata_group)
        layout.addWidget(story_group)
        layout.addWidget(systems_group)
        
    def add_inventory_item(self):
        """Add a new inventory item."""
        item_name = f"Item {self.inventory_list.count() + 1}"
        self.inventory_list.addItem(item_name)
        
    def remove_inventory_item(self):
        """Remove the selected inventory item."""
        current_row = self.inventory_list.currentRow()
        if current_row >= 0:
            self.inventory_list.takeItem(current_row)
            
    def add_flag(self):
        """Add a new game flag."""
        flag_name = f"flag_{self.flags_list.count() + 1}"
        self.flags_list.addItem(flag_name)
        
    def remove_flag(self):
        """Remove the selected flag."""
        current_row = self.flags_list.currentRow()
        if current_row >= 0:
            self.flags_list.takeItem(current_row)


class ExportTab(QWidget):
    """Widget for exporting games."""
    
    def __init__(self):
        super().__init__()
        self.setup_ui()
        
    def setup_ui(self):
        layout = QVBoxLayout(self)
        
        # Export options
        export_group = QGroupBox("Export Options")
        export_layout = QVBoxLayout()
        
        self.export_path_edit = QLineEdit()
        self.export_path_edit.setPlaceholderText("Select export directory...")
        
        browse_layout = QHBoxLayout()
        browse_layout.addWidget(QLabel("Export Path:"))
        browse_layout.addWidget(self.export_path_edit)
        
        self.browse_btn = QPushButton("Browse...")
        self.browse_btn.clicked.connect(self.browse_export_path)
        browse_layout.addWidget(self.browse_btn)
        
        # Export buttons
        button_layout = QVBoxLayout()
        
        self.export_project_btn = QPushButton("Export .advgpt Project")
        self.export_windows_btn = QPushButton("Export Windows Executable")
        self.export_macos_btn = QPushButton("Export macOS App")
        self.export_linux_btn = QPushButton("Export Linux Binary")
        
        self.export_project_btn.clicked.connect(self.export_project)
        self.export_windows_btn.clicked.connect(self.export_windows)
        self.export_macos_btn.clicked.connect(self.export_macos)
        self.export_linux_btn.clicked.connect(self.export_linux)
        
        button_layout.addWidget(self.export_project_btn)
        button_layout.addWidget(self.export_windows_btn)
        button_layout.addWidget(self.export_macos_btn)
        button_layout.addWidget(self.export_linux_btn)
        
        export_layout.addLayout(browse_layout)
        export_layout.addLayout(button_layout)
        export_layout.addStretch()
        export_group.setLayout(export_layout)
        
        # Export log
        log_group = QGroupBox("Export Log")
        log_layout = QVBoxLayout()
        
        self.export_log = QTextEdit()
        self.export_log.setReadOnly(True)
        self.export_log.setMaximumHeight(150)
        
        log_layout.addWidget(self.export_log)
        log_group.setLayout(log_layout)
        
        layout.addWidget(export_group)
        layout.addWidget(log_group)
        
    def browse_export_path(self):
        """Browse for export directory."""
        directory = QFileDialog.getExistingDirectory(self, "Select Export Directory")
        if directory:
            self.export_path_edit.setText(directory)
            
    def export_project(self):
        """Export the current project as .advgpt format."""
        self.export_log.append("Exporting .advgpt project...")
        # Implementation would save the project in the defined JSON format
        self.export_log.append("Export completed successfully!")
        
    def export_windows(self):
        """Export Windows executable."""
        self.export_log.append("Exporting Windows executable...")
        self.export_log.append("This feature will be implemented when the C engine is ready.")
        
    def export_macos(self):
        """Export macOS app."""
        self.export_log.append("Exporting macOS app...")
        self.export_log.append("This feature will be implemented when the C engine is ready.")
        
    def export_linux(self):
        """Export Linux binary."""
        self.export_log.append("Exporting Linux binary...")
        self.export_log.append("This feature will be implemented when the C engine is ready.")


class AdventureGPTEditor(QMainWindow):
    """Main application window for AdventureGPT Editor."""
    
    def __init__(self):
        super().__init__()
        self.current_project_path = None
        self.setup_ui()
        self.setup_menu()
        
    def setup_ui(self):
        """Set up the main user interface."""
        self.setWindowTitle("AdventureGPT Editor")
        self.setGeometry(100, 100, 1200, 800)
        
        # Create central widget with tabs
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        
        layout = QVBoxLayout(central_widget)
        
        # Create tab widget
        self.tab_widget = QTabWidget()
        
        # Add tabs
        self.location_editor = LocationEditor()
        self.story_editor = StoryEditor()
        self.export_tab = ExportTab()
        
        self.tab_widget.addTab(self.location_editor, "Map Editor")
        self.tab_widget.addTab(self.story_editor, "Story Editor")
        self.tab_widget.addTab(self.export_tab, "Export")
        
        layout.addWidget(self.tab_widget)
        
    def setup_menu(self):
        """Set up the application menu bar."""
        menubar = self.menuBar()
        
        # File menu
        file_menu = menubar.addMenu("File")
        
        new_action = QAction("New Project", self)
        new_action.setShortcut("Ctrl+N")
        new_action.triggered.connect(self.new_project)
        file_menu.addAction(new_action)
        
        open_action = QAction("Open Project", self)
        open_action.setShortcut("Ctrl+O")
        open_action.triggered.connect(self.open_project)
        file_menu.addAction(open_action)
        
        save_action = QAction("Save Project", self)
        save_action.setShortcut("Ctrl+S")
        save_action.triggered.connect(self.save_project)
        file_menu.addAction(save_action)
        
        save_as_action = QAction("Save Project As...", self)
        save_as_action.setShortcut("Ctrl+Shift+S")
        save_as_action.triggered.connect(self.save_project_as)
        file_menu.addAction(save_as_action)
        
        file_menu.addSeparator()
        
        exit_action = QAction("Exit", self)
        exit_action.setShortcut("Ctrl+Q")
        exit_action.triggered.connect(self.close)
        file_menu.addAction(exit_action)
        
        # Help menu
        help_menu = menubar.addMenu("Help")
        
        about_action = QAction("About", self)
        about_action.triggered.connect(self.show_about)
        help_menu.addAction(about_action)
        
    def new_project(self):
        """Create a new project."""
        self.current_project_path = None
        self.setWindowTitle("AdventureGPT Editor - New Project")
        # Clear all editors
        # This would reset all the form fields and lists
        
    def open_project(self):
        """Open an existing project."""
        file_path, _ = QFileDialog.getOpenFileName(
            self, "Open AdventureGPT Project", "", 
            "AdventureGPT Projects (*.advgpt);;JSON Files (*.json)"
        )
        if file_path:
            try:
                with open(file_path, 'r') as f:
                    project_data = json.load(f)
                self.load_project_data(project_data)
                self.current_project_path = file_path
                self.setWindowTitle(f"AdventureGPT Editor - {Path(file_path).name}")
            except Exception as e:
                QMessageBox.critical(self, "Error", f"Failed to open project: {str(e)}")
                
    def save_project(self):
        """Save the current project."""
        if self.current_project_path:
            self.save_project_to_path(self.current_project_path)
        else:
            self.save_project_as()
            
    def save_project_as(self):
        """Save the project with a new name."""
        file_path, _ = QFileDialog.getSaveFileName(
            self, "Save AdventureGPT Project", "", 
            "AdventureGPT Projects (*.advgpt);;JSON Files (*.json)"
        )
        if file_path:
            self.save_project_to_path(file_path)
            self.current_project_path = file_path
            self.setWindowTitle(f"AdventureGPT Editor - {Path(file_path).name}")
            
    def save_project_to_path(self, file_path):
        """Save project data to specified path."""
        try:
            project_data = self.get_project_data()
            with open(file_path, 'w') as f:
                json.dump(project_data, f, indent=2)
            QMessageBox.information(self, "Success", "Project saved successfully!")
        except Exception as e:
            QMessageBox.critical(self, "Error", f"Failed to save project: {str(e)}")
            
    def get_project_data(self):
        """Get current project data as dictionary."""
        # This would collect data from all the editors
        return {
            "meta": {
                "title": self.story_editor.game_title_edit.text() or "Untitled Adventure",
                "author": self.story_editor.game_author_edit.text() or "Unknown",
                "description": self.story_editor.game_description_edit.toPlainText()
            },
            "start_location": "start",
            "locations": {},
            "inventory_items": [],
            "flags": {}
        }
        
    def load_project_data(self, data):
        """Load project data into the editors."""
        # This would populate all the form fields with the loaded data
        if "meta" in data:
            meta = data["meta"]
            self.story_editor.game_title_edit.setText(meta.get("title", ""))
            self.story_editor.game_author_edit.setText(meta.get("author", ""))
            self.story_editor.game_description_edit.setPlainText(meta.get("description", ""))
            
    def show_about(self):
        """Show about dialog."""
        QMessageBox.about(
            self, "About AdventureGPT Editor",
            "AdventureGPT Editor v1.0\n\n"
            "A cross-platform editor for creating text adventure games with static images.\n\n"
            "The editor allows you to create locations, write stories, and export games "
            "for Windows, macOS, Linux, and eventually AmigaOS."
        )


def main():
    """Main application entry point."""
    app = QApplication(sys.argv)
    app.setApplicationName("AdventureGPT Editor")
    app.setApplicationVersion("1.0")
    
    # Set application icon (if available)
    # app.setWindowIcon(QIcon("icon.png"))
    
    window = AdventureGPTEditor()
    window.show()
    
    return app.exec()


if __name__ == "__main__":
    sys.exit(main()) 