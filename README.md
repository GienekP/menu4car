# MENU2CAR

This utility let's you create a AtariMaxFlash/JatariCart cart image with your favourite Atari 8-bit games.
Also, XEX flasher ready to write do some media may be created. No more additional tools!
In addition XEX flasher may be created for any .car or .bin file.

Input file types handled:
- standard Atari Dos binary (extensions: .exe, .xex, .obj, .com eithercase)
- 1kB, 2kB, 4kB or 8kB .car standard cartridge image files (extension: .car eithercase)
- any cartridge file for xex flasher generation.

The example file menu4car.txt contains an input in form of sort of csv file, separated by "|" char. It is an UTF-8 encoded file with Polish diacritical letters handling as well as some German letters too.

The first field is the name of the game in readable form. Only first 24 chars will be copied.
The second field is the path to the DOS binary executable , or 1k, 2k, 4k, 8k .car cartridge image format.

Cartridge .car images are treated as ordinary 8kB cartridges and must have headers and '.car' extension. When mounted, system sees them as original cartridge (they sit in one of the banks in JCart so they act as real carts)


In the future more file types will be handled (planned ATR files)

The third optional field are the options. For now options for dos executables:
* c0 - no compression
* c1 - apl 256-byte window compression, full stream in/out.
* c2 - apl every DOS block (long enough) compression
* c3 - zx0 every DOS block (long enough) compression
* ca - auto - gets the shortest compressed version (or uncompressed) from above

Maximum 7 * 26 = 182 files may be added. Sometimes some game may not work with compression (what games? - please report in Issues), please add c0 as the option then for tests (and raise an issue, if the file works on Atari for example from SIO device)

## Usage

When file .txt is ready, invoke:

    ./menu4car menu4car.txt -o outcartimage.car

then outcartimage.car is a cart image MAXFLASH (type 42) compatible.

    ./menu4car menu4car.txt -o outcartimage.xex

and outcartimage.xex is a flasher application ready to be placed on the media readable by real Atari. The file size nay exceed 1 MB, so floppy disk may too small for cartridge images filled up more than disk size.

    ./menu4car menu4car.txt -o outcartimage

and outcartimage.xex, outcartimage.car and outcartimage.bin files are written.

    ./menu4car -b cartimage.car -o cartimageflasher.xex

and flasher for cartimage.car is written.


Full list of parameteres:

    ./menu4car

or

    ./menu4car -?

If a parameter is specified more than once, the last is active. Parameter '-S' if given must be after '-s'.

## Binaries

Binaries are in "Releases" tab.

## Build the executables:

### For windows 10/11

 - Download and install mingw32 from site https://sourceforge.net/projects/mingw
 - Run downloaded installer
 - select:
 -- mingw-developer-tools
 -- mingw32-basev
 -- msys-base
   then apply changes.

Then add to the path (Edit system environemnt variables):
 - c:\MinGW\msys\1.0\bin
 - c:\MinGW\bin

Then Windows-R and enter cmd to run shell
Tools like make, bash, awk, sed should work now.

### for all systems then:

You will need mads_*/mads.exe which can be downloaded from https::github.com/tebe6502/Mad-Assembler from bin/<SO>/ catalog.
You will also need git for convenience (I personally have it installed under WSL and switch from env to env, but it can be installed separately)

If you do not want to use git, you can download the code from repositories below by hand from github (as zip) and set them by hand (https://github.com/GienekP/menu4car and https://github.com/emmanuel-marty/apultra)

To get the code, type in terminal:

    git clone https://github.com/GienekP/menu4car.git

To init submodules, type:

    cd menu4car
    git submodule init
    git submodule update
    
In case "by hand" the apultra content should be placed in menu4car/apultra catalog. Apultra content is optional. ZX0 is obligatory, but in slightly modified version. https://github.com/jhusak/ZX0 is the right one.

Then type "make" to make dependences (apultra library and ZX0) and the menu4car itself.

Optionally go to the "tools" tirectory and type "make" to build homesoft atr converter tool. This tool outputs all the games and corresponding txt file with file list.

## Authors:

The main author of main functionality: menu, xex reading - GienekP

The add-ons: cmdline arguments, optimization, compression, 8kB cartridge handling, 7-page navigation, xex flasher generation, xex compressed loading - Jakub Husak

The apultra 6502 decompressor by xxl (incompatibility of apultra compressed output with aplib spec fixed by him, also); also zx0 6502 decompressor by him.

The apultra compressor by Emmanuel Marty.

The ZX0 compressor by Einar Saukas, modified by ClaireCheshireCat.
