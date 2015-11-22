# StenturaArduino

The goal of this project is to make a US Stentura 200 SRT stenograph machine compatibile with italian stenotype system.
Since I was not able to reverse engineer RS232 pin layout, I used an Arduino Micro to interface switches and convert data.
I did not implement the Stentura SRT Protocol, but a simpler one used by WinSteno 2000 softare.

# Why this project?

I bought a used stentura SRT 200 on ebay and I need to remap steno keys for italian language and make it work with PC steno software, like plover or WinSteno 2000.

Since remapping the keys is just a substitution scheme, so it could be done at PC level. However the dictionaries used by PC steno software needs to be updated for the new map. 

I was not able to use the serial comunication, so I went for hardware interface with Arduino.

# Documents

Reverse engineer serial pin layout
 > does not work maybe custom voltage are used?
 > there's a 3k5 resistor on I/O after MAX232 TTL to RS level converter
 > need to buy stentura srt cable and try again

Interface an arduino controller to the key latches board
 > he did the hardwork before me: https://github.com/balthamos/steno-arduino

# Next steps

Reverse engineer RS232 with stentura srt cable (so that other people can save 30$ )
Document arduino board layout 
Develop a PCB for more robust handling
Develop a Plover driver using WinSteno protocol 
