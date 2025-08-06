# `cmenu`

## About 

`cmenu` is a cross-platform [`dmenu`](https://tools.suckless.org/dmenu/) clone built with [Raylib](https://www.raylib.com/) in ~500 lines of code. 

## Features

```
Usage: cmenu [options]

Options:

    -b --bottom            Appears at the bottom of the screen. (default: false)
    -c --center            Centers the window. (default: false)
    -m --monitor MONITOR   Try to display on monitor number supplied. Monitor
                           numbers are starting from 0. (default: 0)
    -p --prompt PROMPT     Prompt to be displayed to the left of the input field.
                           (default: None)
    -x POSITION            X position of the window. (default: 0)
    -y POSITION            Y position of the window. (default: 0)
    -w --width WIDTH       Width of the window. (default: screen width)
    --font-size FONT_SIZE  Font size of the prompt and input. (default: 16px)

Behaviour:

    The menu will be read from STDIN as newline separated strings. Order is preserved.

    If the user input is a match for an option and the user has not selected an
    option, the first matching option is printed to stdout and the program
    exits with code 0.

    If the user input is not a match for an option, the user input is printed
    to stdout and the program exits with code 2.

    If the user input is empty, nothing is printed and the program exits with
    code 0.

    If the user presses ESC, the program exits with code 0.
```

## Dependencies

None

## Building

### 1. Get Raylib

You will need [Raylib](https://www.raylib.com/) library installed on your system to be able to build `cmenu`. You can try running 

```shell 
make install-deps
```

To install raylib. 

### 2. Initialise git submodules

```shell
$ git submodule init
```

### 3. Build

```shell
$ make
```

The output binaries will be in the `target/` directory.

## TODO

- [ ] Support mouse click
- [ ] Allow custom font
- [ ] Enable custom colours
- [ ] Fix arrow navigation and item selection highlight
- [ ] Create test suite
- [ ] Write launcher script (as per `dmenu_run`)
- [ ] set up companion shell menu script (use `bmenu` format?)
- [ ] Add number shortcuts option
- [ ] Exit on lose focus by default, with option to prevent
- [ ] Build for windows (?)

## Done 

- [x] enable vertical mode
- [x] Enable center of screen mode with vertical
- [x] Fix ugly font
- [x] Fix styling with input box

## Feature inspiration 

- https://github.com/muff1nman/dmenu2
- https://github.com/davatorium/rofi
- https://github.com/emgram769/lighthouse
- https://github.com/MarkHedleyJones/dmenu-extended
- https://github.com/TrilbyWhite/interrobang
- https://github.com/sbstnc/dmenu-ee
- https://ulauncher.io/
- https://github.com/Cloudef/bemenu
- https://github.com/albertlauncher
