# DevilutionX Modding Framework

This directory contains the modding framework for DevilutionX, which allows modders to customize various aspects of the game without modifying the core game code.

## Item Drop Rate Modification System

The Item Drop Rate Modification System is the first component of the modding framework. It allows modders to customize item drop rates in the game through a JSON configuration file.

### Documentation

For detailed documentation on the Item Drop Rate Modification System, see:
- [Item Drop Rate Modding Guide](../../docs/item_drop_rate_modding.md)

### Testing Tool

The modding framework includes a command-line tool for testing drop rate modifications. This tool allows you to:

- Generate reports of how drop rate modifiers affect item drop rates
- Simulate item drops with the current configuration
- Compare original and modified drop rates
- Reload the drop rate configuration without restarting the game

#### Building the Testing Tool

To build the testing tool, use the following CMake command:

```bash
cmake -B build -S .
cmake --build build --target droprate_tool
```

#### Using the Testing Tool

```bash
# Generate a report of drop rate modifications
./droprate_tool report drop_rate_report.md

# Simulate 1000 drops with monster level 30 and dungeon level 16
./droprate_tool simulate 1000 30 16

# Compare original and modified drop rates
./droprate_tool compare 30 16

# Reload the drop rate configuration
./droprate_tool reload
```

## Configuration Files

The modding framework uses the following configuration files:

- `mods/config/drop_rate_config.json`: The main configuration file for item drop rates
- `mods/config/drop_rate_config_sample.json`: A sample configuration file with examples

## Future Enhancements

The modding framework is designed to be extensible. Future enhancements may include:

- Monster modification system
- Quest modification system
- Item property modification system
- UI customization
- Scripting support

## Contributing

If you'd like to contribute to the modding framework, please follow the [DevilutionX contribution guidelines](../../docs/CONTRIBUTING.md).

## License

The modding framework is licensed under the same license as DevilutionX.
