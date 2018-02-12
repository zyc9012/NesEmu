### NesEmu

This is a C++11 port of an NES Emulator

Visit the original Go version at: https://github.com/fogleman/nes


### Building

#### Windows

Just open the project with Visual Studio 2015 and build.

#### MinGW-w64

    pacman -S mingw-w64-i686-glew
    pacman -S mingw-w64-i686-glfw
    pacman -S mingw-w64-i686-portaudio
    make mingw

#### Ubuntu 14.04

Install glfw3 manually.

    apt-get install portaudio19-dev libglew-dev
    make linux

#### Max OS X

    brew install portaudio glew glfw3
    make osx

### Usage

    NesEmu -rom rom_file [-scale screen_scale] [-filter image_filter]
    options:
        -scale    Scale to the orignal screen size, default is 2.0
        -filter   Image filter, one of [nearest|linear], default is nearest

### Controls

| Nintendo              | Emulator    |
| --------------------- | ----------- |
| Up, Down, Left, Right | I, K, J, L  |
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

### TODO

    screenshot
    more mappers
