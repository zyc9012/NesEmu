### NesEmu

This is a C++11 port of an NES Emulator

Visit the original Go version at: https://github.com/fogleman/nes

### Dependencies

    glew: http://glew.sourceforge.net/
    glfw: http://www.glfw.org/
    portaudio: http://www.portaudio.com/

The project can be nicely compiled and run on Visual Studio 2015, solutions
on other platforms will be added soon.

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
| Reset                 | R           |

| State                 | Emulator    |
| --------------------- | ----------- |
| Quick Save            | F5          |
| Quick Load            | F9          |

### TODO

    screenshot
    more mappers
