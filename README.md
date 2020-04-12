# CHIP8 Interpreter 

This is a CHIP8 interpreter written in C. It's currently a work in progress, but it supports running CHIP8 ROMs and has a debug mode for debugging a CHIP8 ROM.

### Building and Running

In order to build the project, you will need to have ncurses installed.

```bash
sudo apt-get install libncurses5-dev libncursesw5-dev
```

Once ncurses is installed, you can run use the following command to build the project.

```bash
make build
```

There are some test ROMs in the `/roms` folder. Once the project has been compiled, you can use the following command to run it with a ROM.

```bash
chip8 ./path/to/rom.ch8
```

Additionally, you can pass the `-d` flag to run the the interpreter in debug mode to step through the ROM in a debug mode.

```bash
chip8 -d ./path/to/rom.ch8
```
