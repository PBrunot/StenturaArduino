/*
   Copyright (c) 2015 Kevin Nygaard

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
#define ON_LED   6
#define DATA_REC 7

#define DEBUG

#define ARRAY_SIZE(array) \
  (sizeof(array) / sizeof(*array))

/*
   Setup IO and serial connections
*/
void setup()
{
  // Initialize serial as per WinSteno requirements
  Serial.begin(BAUD, SERIAL_8E1);
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
   Convert raw Stentura byte data into packed Gemini PR format

   Data that is seemingly skipped is rendundant, and as far as I can tell,
   unnecessary

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
   Every key as value, chords are individual keys OR'd together.

*/

// First 12 keys values =  B0 - B1 value , B3-B4 zero.
//                               S       T       K       P       W       H       R        A      O      *      E      U         (english)
//                               S       P       C       T       H       V       R        I      A      *      E      O         (see italian layout.pg)

// Next 11 keys values = B0 = 1, B1 = 0, B3-B4 value
//                               F       R       P       B       L       G       T       S       D      Z      #
//                               C       S       T       H       P       R       I       E       A      O      #

unsigned long keys_values[] = {0x0300, 0x0500, 0x0900, 0x1100, 0x2100, 0x4100, 0x8100, 0x0102, 0x0104, 0x0108, 0x0110, 0x0120,
                               0x0140, 0x0180, 0x0200, 0x0400, 0x0800, 0x0100, 0x0200, 0x0400, 0x8000, 0x0200, 0x0400 };

char keys[] = { 'S','P','C','T','H','V','R','I','A','*','E','O','C','S','T','H','P','R','I','E','A','O','#'};

void construct_data(char raw_data[], char packed_data[])
{
  char coded_key[] = {0, 0, 0, 0};
#ifdef DEBUG
  Serial.write('Sending:');
#endif
  for (int i = 0; i < 23; i++)
  {
    if (raw_data[i])
    {
      
      if (i < 12)
      {
        coded_key[0] = keys_values[i] >> 8;
        coded_key[1] = keys_values[i] << 8;
        coded_key[2] = 0;
        coded_key[3] = 0;
      }
      else
      {
        coded_key[0] = 1;
        coded_key[1] = 0;
        coded_key[2] = keys_values[i] >> 8;
        coded_key[3] = keys_values[i] << 8;
      }
  
      // OR the results
      packed_data[0] |= coded_key[0];
      packed_data[1] |= coded_key[1];
      packed_data[2] |= coded_key[2];
      packed_data[3] |= coded_key[3];

#ifdef DEBUG
      Serial.write(keys[i]);
#endif
    }
#ifdef DEBUG    
    Serial.write('\n');
#endif    
  }
}

/*
   Read keystrokes and return them to the host
*/
void loop()
{
  // Byte array of depressed keys, following mapping above
  // 0 means not pressed, 1 means pressed
  static char pressed_keys[24];

  // Keys packed in Gemini PR format, ready for transmission over serial
  static char packed_keys[4];

  // Chord has been started, but is not complete (all keys released)
  static char in_progress;

  // Send data to host over serial
  static char send_data;

  int i;
  char pressed;
  char keys_down;

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
    digitalWrite(DATA_REC,  LOW);
  } else {
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
        digitalWrite(DATA_REC,  HIGH); // Indicate there is data in the buffer
        keys_down   = 1;
        in_progress = 1;
      }

      /*
         Toggle clock. Max frequency of shift register MM74HC165 is 30
         MHz, so it can be switched without any delay by Arduino.
      */
      digitalWrite(CLK, HIGH);
      digitalWrite(CLK, LOW);
    }

    // Make latch transparent again to capture next set of keystrokes
    digitalWrite(LATCH_EN, LOW);

    // Return data to host when all keys have been released
    if (in_progress && !keys_down) {
      send_data = 1;
    }
  }
}


