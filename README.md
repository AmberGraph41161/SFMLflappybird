# SFMLflappybird

> a flappybird clone made with SFML, with some "adjustments" to make it more fun and interesting

![splashbargif](md/splashbar.gif)

![gameplaygif](md/gameplay.gif)

![splashbargif](md/splashbar.gif)

### Resources

***Art***

> all art done by me, drawn by hand in aseprite

***Sounds/SFX***

> all sfx done by me in SFXR

### Controls
| keyboard button | action |
| --- | --- |
| q | quit |
| SPACE | jump |
| ESCAPE | pause |

# INSTALLATION

## Linux

Dependencies:

> make >= 4.4.1

> libsfml >= 3.0.0

> clang >= 18.1.8
>  
> (provided Makefile uses clang++. You can probably edit the makefile if wanted)

### My distro's repos *DON'T* have SFML:

1. clone the git repo and cd into it
```shell
git clone https://github.com/AmberGraph41161/SFMLflappybird && cd SFMLflappybird
```

2. make a build directory and cd into it
```shell
mkdir build && cd build
```

3. run cmake (this *should* automatically download SFML for you)
```shell
cmake ..
```

4. run cmake-generated makefile
```shell
make
```

5. move new binary to parent folder to resolve resource finding (binary and "resources/" must be in the same directory)
```shell
mv SFMLflappybird ../ && cd ..
```

6. run and play SFMLflappybird!
```shell
./SFMLflappybird
```

### My distro's repos *DO* have SFML:

1. if your distro's repos have SFML, install SFML dependency with package manger.
on arch linux (and any other arch-based distro), do:
```shell
sudo pacman -Sy sfml
```

2. clone the git repo and cd into it
```shell
git clone https://github.com/AmberGraph41161/SFMLflappybird && cd SFMLflappybird
```

3. run the provided makefile
```shell
make
```

4. run the output binary and enjoy SFMLflappybird!
```shell
./main
```

## Windows

***AS OF Thursday, March 06, 2025, 23:13:14***
***installation instructions for SFML 3.0.x for Windows have not been tested nor updated yet***
***may or may not be totally broken, since SFML 3.0.x deprecated openAL for miniaudio***

> If you are not using MinGW, you're on your own. Good luck

Prerequisites:

- Software needed:
    - MinGW (g++/gcc, mingw32-make)
        - Either [MinGW-sourceforge](https://sourceforge.net/projects/mingw/) or [CodeBlocksIDE+MinGW-setup](https://www.codeblocks.org/downloads/binaries/)
    - [CMake](https://cmake.org/download/)
    - [git bash](https://git-scm.com/downloads)

1. open git bash terminal, clone the repository, and then cd into it
```shell
git clone https://github.com/AmberGraph41161/SFMLflappybird && cd SFMLflappybird
```

2. make build directory and cd into it
```shell
mkdir build && cd build
```

3. run the following CMake command to generate a makefile:
```shell
cmake -G "MinGW Makefiles" ..
```

4. run mingw32-make on the CMake generated makefile
```shell
mingw32-make
```

5. move new binary to parent folder
```shell
mv SFMLflappybird.exe ..
```

6. copy necessary .dll files into same directory as SFMLflappybird binary
```shell
cp _deps/sfml-build/lib/* ../
cp _deps/sfml-src/extlibs/bin/x86/openal32.dll ../
```

7. finally, cd into parent directory, run SFMLflappybird, and enjoy!
```shell
cd .. && ./SFMLflappybird.exe
```

## MacOS (OS X)
not tested as of Friday, November 22, 2024, 18:11:25

# Please view only

This project is available for **viewing only**.
You are welcome to read and explore the source code, but **you may not copy, modify, or distribute** the code under any circumstances.
All rights are reserved to the original author.

For any requests regarding usage, please contact the author directly.
