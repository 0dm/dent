# dent - a CHIP-8 interpreter
Includes some free ROMs to try.

# Requires SDL2
## Keep SDL2.dll in the same directory as dent.exe
usage:
```
dent <path_to_rom>
```

# Screenshot
![Breakout](docs/img/breakout.png)

`BREAKOUT`

# Controls
- F1: Pause
- F4: Exit
- F11: Toggle Fullscreen

## Gamepad:
```
1 2 3 4
q w e r
a s d f
z x c v
```

# Build
use cmake or build with `g++ main.cpp CPU/CPU.cpp GFX/GFX.cpp -lmingw32 -lSDL2main -lSDL2 -O3 -o dent.exe`

# TODO:
```diff
-Add Sound
-Cleaner Instruction Handling
``` 

# Resources

[Wikipedia](https://en.wikipedia.org/wiki/CHIP-8)

[Short Tutorial](http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/)
