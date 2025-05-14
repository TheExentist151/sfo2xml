# sfo2xml
sfo2xml is a converter for Sony's SFO format. It can convert it to XML and vice versa.

WARNING: This program supports only *game* SFO's, not *save data* SFO's. You can convert them, 
but the output won't match with the original SFO.

## Usage
Converting a SFO file to XML: 
```
sfo2xml.exe <input SFO file path> [output XML file path]
```

Converting an XML file to SFO: 
```
sfo2xml.exe <input XML file path> [output SFO file path]
```

## Building

### Requirements
 * CMake
 * GCC (if you're building on MSYS or Linux)
 * Git
 * Make (if you're building on MSYS or Linux)

### Windows (Visual Studio)
1. Clone the repo: 
```
git clone https://github.com/TheExentist151/sfo2xml --recursive
```

2. CD into the directory
```
cd sfo2xml
```

3. Run CMake: 
```
cmake -B build
```

This command will generate solution for Visual Studio in the `build` folder.

If you want to change the architecture, you can specify it with the `-A` flag.

4. Open the generated solution `build/sfo2xml.sln` in Visual Studio and build the program

### Windows (MinGW)
1. Clone the repo: 
```
git clone https://github.com/TheExentist151/sfo2xml --recursive
```

2. CD into the directory
```
cd sfo2xml
```

3. Run CMake: 
```
cmake -B build -G "MinGW Makefiles" && cd build
```

This command will generate makefiles for MinGW in the `build` folder and automatically go to this folder.

4. Run mingw32-make: 
```
mingw32-make
```

### MSYS/Linux
1. Clone the repo: 
```
git clone https://github.com/TheExentist151/sfo2xml --recursive
```

2. CD into the directory
```
cd sfo2xml
```

3. Run CMake: 
```
cmake -B build && cd build
```

This command will generate Makefile in the `build` folder and automatically go to this folder

4. Run make: 
```
make
```

## External Resources
 * [PARAM.SFO documentation on psdevwiki.com](https://www.psdevwiki.com/ps3/PARAM.SFO)