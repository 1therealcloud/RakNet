# RakNet 2.52

A RakNet fork compatible with SA:MP 0.3.7.
* Client-Only: Contains only client-side code, with no server implementation.
* No Networking Logic: Excludes protocol-level networking code (e.g., handshakes).
* Not taken from leaked SA:MP code.

## Build:

### Linux GCC x86
```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-m32"
cmake --build build -j$(nproc)
```

### MinGW x86
```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_CXX_COMPILER=i686-w64-mingw32-g++`
cmake --build build -j$(nproc)
```

### MSVC x86
```
cmake -S . -B build -A Win32
cmake --build build --config Release
```
