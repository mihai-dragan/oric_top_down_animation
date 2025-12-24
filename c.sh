rm *.map
rm testskw.s
rm *.o

cc65 -Oirs --codesize 500 -t atmos testskw.c
#cc65 -Cl -Ois --codesize 500 -t atmos skweeks.c
ca65 testskw.s
ca65 keyboard.s
#ld65 -o skweeks.tap -C myatmos.cfg -m skweeks.map skweeks.o keyboard.o atmos.lib
ld65 -o testskw.tap -t atmos -m testskw.map testskw.o keyboard.o atmos.lib
