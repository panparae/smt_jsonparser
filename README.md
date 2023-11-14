# smt_jsonparser
This plugin made with Example plugin template for use with CommonLibSSE by [Ryan-rsm-McKenzie.](https://github.com/Ryan-rsm-McKenzie/ExamplePlugin-CommonLibSSE)

SKSE64 Plugin made for using hotkeys with EquipSet.

Currently CommonLibSSE all included in `extern`. I want to submodule it rather then included it all but, don't know how to do it with modified one.

## Dependency
* [tcmalloc](https://github.com/google/tcmalloc)

## Build Dependency
* [CMake](https://cmake.org/)
	* Add this to your `PATH`
* [vcpkg](https://github.com/microsoft/vcpkg)
* [Catch2](https://github.com/catchorg/Catch2) 

## Register Visual Studio as a Generator
* Open `x64 Native Tools Command Prompt`
* Run `cmake`
* Close the cmd window

## Set Environment Variable
*  Add `smt_jsonparser_extern`
	* locate external module directory `smt_jsonparser/extern`

## Building
```
git clone https://github.com/panparae/smt_jsonparser
cd smt_jsonparser
cmake -B build/Debug -S . -DCMAKE_BUILD_TYPE=Debug -DUNIT_TEST:BOOL=false -DCMAKE_TOOLCHAIN_FILE={YOUR_VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake
```

## Usage
Copy `smt_jsonparser/include/` header files and tcmalloc dll, lib `smt_jsonparser/extern/` to your project.