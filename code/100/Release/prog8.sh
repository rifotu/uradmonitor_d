#!/bin/sh
#avrdude -v
#echo "*** writing fuses for 8MHz external crystal"
avrdude -p atmega128 -c usbasp -U lfuse:w:0xdc:m 	-U hfuse:w:0x9f:m 	-U efuse:w:0xff:m 
echo "*** writting flash"
#extra options -vvvv -B 1..4
/usr/local/AVRMacPack-20071126/bin/avrdude -p atmega128 -c usbasp -U flash:w:uradmonitor-D.hex:i
