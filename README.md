# StenturaArduino
Emulate stentura SRT 200 stenograph machine serial protocol on arduino

# Goals
I bought a used stentura SRT 200 on ebay and I need to :
- remap steno keys for italian language
- make it work with PC steno software, like plover
- 
# Steps

Reverse engineer serial pin layout
 -> done 
   ( GND ) ( TX ) ( RTS? ) ( RX  )  (    )
      (  )   ( CTS?) (    ) (    )

-> does not work maybe custom voltage are used?

there's a 3k5 resistor on I/O after MAX232 TTL to RS level converter
need to buy stentura srt cable and try again


Interface an arduino controller to the key latches board
 -> done see Github 

Issue is existing implementation is using Gemini PR protocol, which my italian software steno does not handle. Therefore I am implementing stenograph protocol on arduino.

# Next steps

Reverse engineer stentura srt cable
Document arduino board layout
