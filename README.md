# MENU2CAR

This utility let's you create a MaxFlash/JCart cart image with your favourite Atari 8-bit games.

Games are to in standard Atari Dos binary.

The example file menu4car.txt contains an input in form of sort of csv file, separated by "|" sign. It is an UTF-8 encoded file with Polish diacritical letters handling as well as some german letters too.

The first field is the name of the game in readable form, you can use, the second field is the path to the binary executable.
 For now maximum 26 games may be added.

When file .txt is ready, invoke:

./menu4car menu4car.txt outcartimage.car

Then outcartimage.car is a cart image MAXFLASH (type 42) compatible.
