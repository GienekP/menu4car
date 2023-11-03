# MENU2CAR

This utility let's you create a MaxFlash/JCart cart image with your favourite Atari 8-bit games.

Games are to be in standard Atari Dos binary.

The example file menu4car.txt contains an input in form of sort of csv file, separated by "|" char. It is an UTF-8 encoded file with Polish diacritical letters handling as well as some German letters too.

The first field is the name of the game in readable form.
The second field is the path to the binary executable.
The third optional field are the options. For now:
    * c0 - no compression
    * c1 - apl 256-byte window compression,
    * c2 - apl block by block compression
    * ca - auto - gets the shortest compressed version from above

For now maximum 26 games may be added. Sometimes some game does not work with compression, please add c0 as the option then.

When file .txt is ready, invoke:

./menu4car menu4car.txt outcartimage.car

Then outcartimage.car is a cart image MAXFLASH (type 42) compatible.

##Build the executables:
    type "make" to make dependences (apultra library) and the menu4car itself
    Optionally go to the tools tirectory and type "make" to build homesoft atr converter tool. This tool outputs all the games and corresponding txt file with file list.
