# RakNet

RakNet 2.52 fork compatible with SA:MP 0.3.7.

## Build

### Linux GCC x86

```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-m32"
cmake --build build -j$(nproc)
```

### MSVC x86

```
cmake -S . -B build -A Win32
cmake --build build --config Release
```

### .NET 2003 (MSVC 7.1)

```
build-msvc71.cmd
```

## Samples

| Sample | Description |
|---|---|
| BigPacketTest | Sends a 1 MB packet to test packet splitting |
| BitStream | Reading and writing native types with BitStream |
| DroppedConnectionTest | Behavior when a connection is dropped |
| ReliableOrderedTest | Reliable ordered packet delivery |
| RPC | Remote procedure calls |
| Query | SA-MP UDP query server and client |
| RCON | SA-MP UDP RCON server and client |
