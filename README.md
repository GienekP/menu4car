# MENU2CAR

This utility let's you create a MaxFlash/JCart cart image with your favourite Atari 8-bit games.

File types handled:
- standard Atari Dos binary (extensions: .exe, .xex, .obj, .com eithercase)
- 1kB, 2kB, 4kB 8kB .car standard cartridge image files (extension: .car eithercase)

The example file menu4car.txt contains an input in form of sort of csv file, separated by "|" char. It is an UTF-8 encoded file with Polish diacritical letters handling as well as some German letters too.

The first field is the name of the game in readable form. Only first 24 chars will be copied.
The second field is the path to the DOS binary executable , or 1k, 2k, 4k, 8k .car cartridge image format.

Cartridge .car images are treated as ordinary 8kB cartridges and must have headers and '.car' extension. When mounted, system sees them as original cartridge.

In the future more file types will be handled (planned ATR files)

The third optional field are the options. For now options for dos executables:
* c0 - no compression
* c1 - apl 256-byte window compression, full stream in/out.
* c2 - apl every DOS block (long enough) compression
* ca - auto - gets the shortest compressed version (or uncompressed) from above

Maximum 4 * 26 = 104 files may be added. Sometimes some game may not work with compression (what games? - please report in Issues), please add c0 as the option then for tests (and raise an issue, if the file works on Atari for example from SIO device)

## Usage

When file .txt is ready, invoke:

    ./menu4car menu4car.txt -o outcartimage.car

Then outcartimage.car is a cart image MAXFLASH (type 42) compatible.

Full list of parameteres:

    ./menu4car

or

    ./menu4car -?

If a parameter is specified more than once, the last is active.


## Build the executables:

To get the code, type in terminal:

    git clone https://github.com/GienekP/menu4car.git

To init submodules, type:

    cd menu4car
    git submodule init
    git submodule update

Then type "make" to make dependences (apultra library) and the menu4car itself.

Optionally go to the "tools" tirectory and type "make" to build homesoft atr converter tool. This tool outputs all the games and corresponding txt file with file list.

Authors:

The main author of main functionality: menu, xex reading - GienekP
The add-ons: cmdline arguments, optimization, compression, cartridge handling and 4-page navigation - Jakub Husak
