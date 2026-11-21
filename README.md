# RakNet

RakNet 2.52 fork compatible with SA:MP 0.3.7.

## Build

### Linux

Use a native GCC or Clang toolchain:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

For a cross-build, specify the target compiler. For example, on Debian with
`g++-aarch64-linux-gnu` installed:

```sh
cmake -S . -B build/linux-arm64 -DCMAKE_SYSTEM_NAME=Linux \
  -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++ -DCMAKE_BUILD_TYPE=Release
cmake --build build/linux-arm64 --parallel
```

Use `i686-linux-gnu-g++` for x86 or `arm-linux-gnueabihf-g++` for ARMv7 hard-float.

### Windows

Use a separate build directory per architecture. Visual Studio 2022 accepts
`Win32`, `x64`, or `ARM64` when the corresponding C++ tools are installed.

```powershell
cmake -S . -B build/win32 -G "Visual Studio 17 2022" -A Win32
cmake --build build/win32 --config Release --parallel
```

### Android

For a native build in Termux:

```sh
pkg install clang cmake make
cmake -S . -B build/termux -DCMAKE_BUILD_TYPE=Release
cmake --build build/termux --parallel
```

To cross-compile against the Android NDK, set `ANDROID_NDK_HOME` to the NDK
directory and choose `arm64-v8a`, `armeabi-v7a`, `x86`, or `x86_64`.

```sh
cmake -S . -B build/android-arm64 \
  -DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake" \
  -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-21 \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build/android-arm64 --parallel
```

Use a separate build directory for each ABI. The static library is built with
position-independent code on Unix targets, including Android.

## Samples

Desktop native builds include the samples by default. For a library-only build,
configure with `-DRAKNET_BUILD_SAMPLES=OFF`. Android and cross-builds default to
OFF.

| Sample | Description |
|---|---|
| BigPacketTest | Sends a 1 MB packet to test packet splitting |
| BitStream | Reading and writing native types with BitStream |
| DroppedConnectionTest | Behavior when a connection is dropped |
| ReliableOrderedTest | Reliable ordered packet delivery |
| RPC | Remote procedure calls |
| Query | SA-MP UDP query server and client |
| RCON | SA-MP UDP RCON server and client |
