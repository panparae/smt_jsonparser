# smt_jsonparser
C++ Json parsing and writing library for large data process. 

## Build Dependency
* [CMake](https://cmake.org/)
	* Add this to your `PATH`

## Register Visual Studio as a Generator
* Open `x64 Native Tools Command Prompt`
* Run `cmake`
* Close the cmd window

## Building & Installing
```
git clone https://github.com/panparae/smt_jsonparser
cd smt_jsonparser
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build/. --config Release
cmake --install build/.
```

## Usage
Copy `smt_jsonparser/include/` header files to your project.