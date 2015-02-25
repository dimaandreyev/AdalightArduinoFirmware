
//////////
//
// Arduino interface for the use of ws2812 operated LEDs
// Uses Adalight protocol and is compatible with Boblight, Prismatik etc
// "Magic Word" for synchronisation is 'Ada' followed by LED High, Low and Checksum
//

// Include required libray.
#include <FastSPI_LED2.h>

///// User definitions /////

// Define the number of LEDs
#define NUM_LEDS 5

// Define SPI Pin
#define PIN 3

// Baudrate, higher rate allows faster refresh rate and more LEDs (must be the same as it selected at PC)
#define serialRate 57600

//// End of user definitions /////

//// System  definitions////

// Adalight sends a "Magic Word" before sending the pixel data
uint8_t prefix[] = {'A', 'd', 'a'};

// initialise LED-array
CRGB leds[NUM_LEDS];

//// end of system  definitions////

void setup()
{  
  FastLED.addLeds<WS2812, PIN, GRB>(leds, NUM_LEDS);
  
  // initial RGB flash
  LEDS.showColor(CRGB(255, 0, 0));
  delay(500);
  LEDS.showColor(CRGB(0, 255, 0));
  delay(500);
  LEDS.showColor(CRGB(0, 0, 255));
  delay(500);
  LEDS.showColor(CRGB(128,128, 128));
  delay(500);
  
  //LEDS.showColor(CRGB(0, 0, 0));
  
  Serial.begin(serialRate);
  // Send "Magic Word" string to host
  Serial.print("Ada\n");  
}

void loop() { 
  if(checkSum()){
    // read the transmission data and set LED values
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
      while(!Serial.available());
      leds[i].r = Serial.read();
      while(!Serial.available());
      leds[i].g = Serial.read();
      while(!Serial.available());
      leds[i].b = Serial.read();
    }
    
   // shows new values
   FastLED.show();
  }
}

bool checkSum(){
  uint8_t checksum;
  uint8_t hi, lo;
  
  // Wait for begin.
  WaitPrefix();
  
   // Hi, Lo, Checksum
  while (!Serial.available());
  hi=Serial.read();
  while (!Serial.available());
  lo=Serial.read();
  while (!Serial.available());
  checksum=Serial.read();
  
  // if checksum does not match go back to wait
  if (checksum != (hi ^ lo ^ 0x55))
  {
    return false;
  }
  
  return true;
}

void WaitPrefix(){
  // wait for first byte of Magic Word
  for(uint8_t i = 0; i < sizeof(prefix); ++i) {
   while (!Serial.available());
    // Check next byte in Magic Word
    if(prefix[i] == Serial.read()){
      continue;
    }
    // otherwise, start over
    i = 0;
  }
}
