# MENU2CAR

This utility let's you create a MaxFlash/JCart cart image with your favourite Atari 8-bit games.

Games are to be in standard Atari Dos binary.

The example file menu4car.txt contains an input in form of sort of csv file, separated by "|" char. It is an UTF-8 encoded file with Polish diacritical letters handling as well as some German letters too.

The first field is the name of the game in readable form.
The second field is the path to the DOS binary executable (in the future more file types will be handled)
The third optional field are the options. For now:
    * c0 - no compression
    * c1 - apl 256-byte window compression,
    * c2 - apl every DOS block (long enough) compression
    * ca - auto - gets the shortest compressed version (or uncompressed) from above

For now maximum 26 games may be added. Sometimes some game does not work with compression (what games?), please add c0 as the option then.

When file .txt is ready, invoke:

   ./menu4car menu4car.txt -o outcartimage.car

Then outcartimage.car is a cart image MAXFLASH (type 42) compatible.

Full list of parameteres:

    ./menu4car

or

    ./menu4car -?

If a parameter is specified more than once, the last is active.


## Build the executables:

To get new repo, type in terminal:

    git clone https://github.com/GienekP/menu4car.git

To init submodules, type:

    cd menu4car
    git submodule init
    git submodule update

Then type "make" to make dependences (apultra library) and the menu4car itself.

Optionally go to the "tools" tirectory and type "make" to build homesoft atr converter tool. This tool outputs all the games and corresponding txt file with file list.
