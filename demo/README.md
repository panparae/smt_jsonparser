# smt_jsonparser_demo
Demo project for compare json libraries.

## Build Dependency
* [CMake](https://cmake.org/)
	* Add this to your `PATH`
* [JsonCpp](https://github.com/open-source-parsers/jsoncpp/)
* [nlohmann-json](https://github.com/nlohmann/json/)

## Register Visual Studio as a Generator
* Open `x64 Native Tools Command Prompt`
* Run `cmake`
* Close the cmd window

## Building
```
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE={YOUR_VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake
cmake --build build/. --config Release
```

## Usage
smt_jsonparser_demo -F[] -T[]