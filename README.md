# StenturaArduino

The goal of this project is to make a US Stentura 200 SRT stenograph machine compatibile with italian stenotype system.
Since I was not able to reverse engineer RS232 pin layout, I used an Arduino Micro to interface switches and convert data.
I did not implement the Stentura SRT Protocol, but a simpler one used by WinSteno 2000 softare.

# Story of the project

Reverse engineer serial pin layout - FAIL
 > does not work maybe custom voltage are used?
 > there's a 3k5 resistor on I/O after MAX232 TTL to RS level converter
 > need to buy stentura srt cable and try again

Interface an arduino controller to the key latches board - WORKS
 > he did the hardwork before me: https://github.com/balthamos/steno-arduino

Italian SRT 200 machine - WORKS
 > On italian market, the distributor company changes all the boards with a custom PIC and custom protocol. I reversed engineered the WinSteno protocol to be compatible with PC Software used by italian stenotypists.
 
# Next steps

1. Reverse engineer RS232 with stentura srt cable (so that other people can save 30$ )
2. Document arduino board layout / develop a PCB for more robust handling
3. Develop a Plover driver using WinSteno protocol, just to get rid of proprietary software...
