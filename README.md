# Reverb Plugin

A professional audio reverb plugin built with JUCE framework, featuring a custom-designed user interface with rotary controls.

## Features

- **High-quality reverb processing** for adding space and depth to audio
- **Cross-platform support**: VST3, AU, and Standalone formats
- **Universal binary**: Supports both Intel (x86_64) and Apple Silicon (arm64) architectures

## Requirements

- **CMake** 3.25 or higher
- **C++23** compatible compiler
- **macOS** 10.13 or higher

## Building the Plugin

### 1. Configure the Project

```bash
cmake --preset debug
```

For release builds:
```bash
cmake --preset release
```

### 2. Build

Debug build:
```bash
cmake --build build-debug --config Debug
```

Release build:
```bash
cmake --build build-release --config Release
```

## Using the Plugin

### Using a DAW

Load the plugin in your preferred DAW (Logic Pro, Ableton Live, Reaper, etc.) from the standard plugin locations.

## Development

### Project Structure

```
Reverb/
├── Source/
│   ├── PluginProcessor.cpp/h    # Audio processing logic
│   ├── PluginEditor.cpp/h       # User interface
│   └── ui/
│       └── CustomLookAndFeel.h  # Custom UI styling
├── Assets/                      # Binary assets (images, fonts, etc.)
├── CMakeLists.txt               # Build configuration
└── CMakePresets.json            # Build presets
```


