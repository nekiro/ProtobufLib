## ProtobufLib

DL Library created to help/speed up Open Tibia development, it allows to parse and export appearances from protobuf files.

Requires c++11.

Check [wiki](https://github.com/nekiro/ProtobufLib/wiki)

## Example usage

For more detailed description go to [wiki](https://github.com/nekiro/ProtobufLib/wiki)
Additionally all methods are documented in code.

### Load sprite appearances

```cpp
nekiro_proto::SpriteAppearances library{};
try {
	library.loadSpriteSheets("<path_to_file>", false);
} catch (std::exception& e) {
	std::cout << e.what() << std::endl;
}
```

### Get sprite by id

```cpp
library.getSprite(1234);
```

### Load object appearances (effects, missiles, outfits, items)

```cpp
nekiro_proto::Appearances library{};
try {
	library.parseAppearancesFromFile("<path_to_file>");
} catch (std::exception& e) {
	std::cout << e.what() << std::endl;
}
```

### Retrieve all items appearances

```cpp
library.getAppearances(nekiro_proto::OBJECT_TYPE_ITEM)
```
