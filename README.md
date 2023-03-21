# NesEmu

An NES Emulator in C++11.

## Building

### Installing dependencies

* Windows (MinGW-w64)
    ```
    pacman -S mingw-w64-i686-cmake mingw-w64-i686-SDL2
    ```
* Ubuntu
    ```
    apt-get install cmake libsdl2-dev
    ```
* Mac OS X
    ```
    brew install cmake sdl2
    ```

### Compiling
```
mkdir build && cd build
cmake ..
cmake --build .
```

## Usage

```
NesEmu rom_file [-s screen_scale] [-f image_filter]
options:
    -s    Scale to the orignal screen size (default 2.0)
    -f    Image filter, one of [nearest|linear|best] (default linear)
```

## Controls

| Nintendo              | Emulator    |
| --------------------- | ----------- |
| Up, Down, Left, Right | ↑ ↓ → ←     |
| Start                 | Enter       |
| Select                | Right Shift |
| A                     | Z           |
| B                     | X           |
| A (Turbo)             | A           |
| B (Turbo)             | S           |

| State                 | Emulator    |
| --------------------- | ----------- |
| Quick Save            | F5          |
| Quick Load            | F9          |
