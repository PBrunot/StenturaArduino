/*
   Copyright (c) 2015 Pascal Brunot 
   based on the works of Kevin Nygaard

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

/*
   Pinout
   Ribbon
   ||||||
   ||||||
   ||||||
   xxxxxx (contacts facing up)
    \\\\\\_   Power +5V
     \\\\\_ 2 Test
      \\\\_ 3 Shift load
       \\\_ 4 Serial out
        \\_ 5 Clock
         \_   Ground
*/

/* IO Definitions */
#define CLK      5
#define DATA_IN  4
#define LATCH_EN 3
#define TEST_EN  2
#define BAUD     9600

// Stentura has two leds, ON and DATA TRANSMIT.
// To drive them two pins will be used.
#define ON_LED   6
#define DATA_REC 7

// If debug, serial port outputs text with pressed keys
//#define DEBUG

// Some keys are bouncing. By design, Stentura board already eliminates most of bounces with the latches.
// However as we send data everytime no pressed key is found, a bounce may happen since we poll at roughly 1.5 Mhz.
// (this can be seen experimentally pressing single keys on the machine).
// To avoid bounces, we must read empty data for DEBOUNCE_CYCLES before to send acquired keys. 
//
// Each cycle is roughly 0.5ms on Arduino Nano. 5 cycles was enough to eliminate all bounces.
#define DEBOUNCE_CYCLES 5

#define ARRAY_SIZE(array) \
  (sizeof(array) / sizeof(*array))

/*
   Setup IO and serial connections
*/
void setup()
{
  
#ifdef DEBUG    
  // Initialize for console monitor
  Serial.begin(57600);
#else
  // Initialize serial as per WinSteno requirements 9600, 8 E 1
  Serial.begin(BAUD, SERIAL_8E1);
#endif

  while (!Serial) ; 

  // Setup IOs
  pinMode(CLK,      OUTPUT);
  pinMode(LATCH_EN, OUTPUT);
  pinMode(TEST_EN,  OUTPUT);
  pinMode(DATA_IN,  INPUT);

  pinMode(ON_LED,  OUTPUT);
  pinMode(DATA_REC,  OUTPUT);

  // Drive known values to outputs
  digitalWrite(CLK,      LOW);
  digitalWrite(LATCH_EN, LOW);
  digitalWrite(TEST_EN,  LOW);

  digitalWrite(ON_LED,   HIGH);
  digitalWrite(DATA_REC,  LOW);
}

/*
   Convert raw Stentura byte data into WinSteno format
   Raw matrix mapping of Stentura 200 SRT from serial shift registers
    0: S-
    1: T-
    2: K-
    3: P-
    4: W-
    5: H-
    6: R-
    7: A-
    8: O-
    9:
   10: -E
   11: -U
   12: -F
   13: -R
   14: -P
   15: -B
   16: -L
   17: -G
   18: -T
   19: -S
   20: -D
   21: -Z
   22: #
   23: N/A

   WinSteno 2000 Protocol

   In WinSteno2000 protocol each serial frame is 4 bytes.
   B0-B3 are the bytes in the order seen on the serial line.
   
   Every key has a unique value, chords are just the individual values OR'd together.

   First 14 keys values :  B0 - B1 set to value (little indian), B2-B3 set to zero.
                                 S       T       K       P       W       H       R        A      O      *      E      U     F       R     (english)
                                 S       P       C       T       H       V       R        I      A      *      E      O     C       S     (see italian layout.jpg)

   Next 9 keys values : B0 = 1, B1 = 0, B2-B3 set to key value (little endian)
                                       P       B       L       G       T       S       D      Z      #   (english)
                                       T       H       P       R       I       E       A      O      #   (italian)

*/

word keys_values[] = {0x0300, 0x0500, 0x0900, 0x1100, 0x2100, 0x4100, 0x8100, 0x0102, 0x0104, 0x0108, 0x0110, 0x0120, 0x0140, 0x0180,
                      0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000, 0x0002, 0x0004 };

// To output pressed keys for debugging only
char keys[] = { 'S','P','C','T','H','V','R','I','A','*','E','O','C','S','T','H','P','R','I','E','A','O','#'};

// To avoid dynamic allocation
char tempstr[3] = {0,0,0};


// Encode the pressed key array into a 4 bytes packet to be sent on serial port.
void construct_data(char raw_data[], byte packed_data[])
{
  static char coded_key[] = {0, 0, 0, 0};
  
  for(int i = 0; i < 4; i++)
  {
    packed_data[i]=0;
  }
#ifdef DEBUG
  Serial.write("Sending:");
#endif
  for (int i = 0; i < 23; i++)
  {
    if (raw_data[i])
    {
      
      if (i < 14)
      {
        coded_key[0] = (keys_values[i] >> 8) & 0xFF;
        coded_key[1] = (keys_values[i] & 0xFF);
        coded_key[2] = 0;
        coded_key[3] = 0;
      }
      else
      {
        coded_key[0] = 1;
        coded_key[1] = 0;
        coded_key[2] = (keys_values[i] >> 8) & 0xFF;
        coded_key[3] = (keys_values[i] & 0xFF);
      }
#ifdef DEBUG      
      Serial.write(keys[i]);
      Serial.print("( key value ");
      Serial.print(keys_values[i]);
      Serial.println(")");
#endif
      // OR the results
      packed_data[0] |= coded_key[0];
      packed_data[1] |= coded_key[1];
      packed_data[2] |= coded_key[2];
      packed_data[3] |= coded_key[3];

    }
  }

#ifdef DEBUG    
  Serial.print("Serial packet: ");
  for(int i = 0; i < 4; i++)
  {
    if (packed_data[i] < 10) Serial.print("0"); // No support for %02X format string in Arduino
    Serial.print(packed_data[i],HEX);
    Serial.print(" ");
  }
  Serial.println();
  Serial.write('\n');
#endif  
}

/*
   Read keystrokes and return them to the host
*/
void loop()
{
  // Byte array of depressed keys, following mapping above
  // 0 means not pressed, 1 means pressed
  static char pressed_keys[24];

  // Keys packed in WinSteno2000 format, ready for transmission over serial
  static byte packed_keys[4];

  // Chord has been started, but is not complete (all keys released)
  static char in_progress;

  // Send data to host over serial
  static char send_data;
  static int debounce_cycles = 0;
 
  int i;
  char pressed;
  char keys_down;

  long duration = micros();
  
  if (send_data) {
    construct_data(pressed_keys, packed_keys);

#ifndef DEBUG
    Serial.write(packed_keys, ARRAY_SIZE(packed_keys));
#endif

    /*
       Data returned to host, reset stateful variables
       Note that we don't need to do this in the setup, because they are all
       static and guaranteed to be 0 by spec
    */
    memset(pressed_keys, 0, sizeof(pressed_keys));
    send_data   = 0;
    in_progress = 0;
    debounce_cycles = 0;
    digitalWrite(DATA_REC,  LOW); // Switch off LED, data has been transmitted
  } 
  else 
  {
    // Latch current state of all keys
    digitalWrite(LATCH_EN, HIGH);

    // Read all latched data
    keys_down = 0;
    for (i = 0; i < ARRAY_SIZE(pressed_keys); i++) {
      /*
         All inputs are pulled up. Pressing a key shorts the circuit to
         ground.

         We invert the logic here to convert to more conventional positive
         logic.
      */
      pressed          = !digitalRead(DATA_IN);

      // Once a key is pressed, it stays pressed until the chord is over
      pressed_keys[i] |= pressed;
      if (pressed) {
        digitalWrite(DATA_REC,  HIGH); // Switch on LED to indicate there is data in the buffer
        keys_down   = 1;
        in_progress = 1;
        debounce_cycles = 0;
      }

      /*
         Toggle clock. Max frequency of shift register MM74HC165 is 30
         MHz, so it can be switched without any delay by Arduino.
      */
      digitalWrite(CLK, HIGH);
      digitalWrite(CLK, LOW);
    }

    // When no key are pressed, we wait for DEBOUNCE_CYCLES before to send data.
    if (keys_down == 0)
    {
        debounce_cycles++;
    }
    
    // Make latch transparent again to capture next set of keystrokes
    digitalWrite(LATCH_EN, LOW);

    // Return data to host when all keys have been released
    if (in_progress && !keys_down && debounce_cycles > DEBOUNCE_CYCLES) {
      send_data = 1;
    }

#ifdef DEBUG
    if (duration % 20000 == 0) // Output cycle duration from time to time.
    {
      duration = micros() - duration;
      Serial.print("** Cycle duration :");
      Serial.print(duration);
      Serial.print(" us\n");
    }
#endif

  }
}


