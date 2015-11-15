# StenturaArduino
Implement stentura SRT 200 stenograph machine serial protocol on arduino

# Why this project?

I bought a used stentura SRT 200 on ebay and I need to remap steno keys for italian language and make it work with PC steno software, like plover.

Remapping the keys is just a substitution scheme, so it could be done at PC level. However the dictionaries used by PC steno software needs to be updated for the new map. 

I was not able to use the serial comunication, so I went for hardware interface with Arduino.

# Documetns

Reverse engineer serial pin layout
 > does not work maybe custom voltage are used?
 > there's a 3k5 resistor on I/O after MAX232 TTL to RS level converter
 > need to buy stentura srt cable and try again

Interface an arduino controller to the key latches board
 > he did the hardwork before me: https://github.com/balthamos/steno-arduino

Issue is existing implementation is using Gemini PR protocol, which my italian software steno does not handle. Therefore I am implementing stenograph protocol on arduino.

# Next steps

Reverse engineer stentura srt cable
Document arduino board layout
