# SFMLflappybird

> a flappybird clone made with SFML, with some "adjustments" to make it more fun and interesting

![splashbargif](md/splashbar.gif)

![gameplaygif](md/gameplay.gif)

![splashbargif](md/splashbar.gif)

### Resources

all of the art I drew by hand in aseprite

all sound effects were done by me in SFXR

### Controls
| keyboard button | action |
| --- | --- |
| q | quit |
| SPACE | jump |
| ESCAPE | pause |

# INSTALLATION

## Linux

dependencies:
> sfml >= 2.6.1

> clang++ (provided Makefile uses clang++. You can probably edit the makefile if wanted)

### *My distro's repos DON'T have SFML:*

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

### *My distro's repos DO have SFML:*

1. if your distro's repos have SFML, install SFML dependency with package manger.
on arch linux (and any other arch-based distro), do:
```shell
sudo pacman -S sfml
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
broken as of Friday, November 22, 2024, 18:10:56

## MacOS (OSx)
not tested as of Friday, November 22, 2024, 18:11:25

# Please view only

This project is available for **viewing only**.
You are welcome to read and explore the source code, but **you may not copy, modify, or distribute** the code under any circumstances.
All rights are reserved to the original author.

For any requests regarding usage, please contact the author directly.
