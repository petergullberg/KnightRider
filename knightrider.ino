/*-------------------------------------------------------------------------
  knightrider - 
  A project implementing the Knight Rider KITT Scanner.

  The project uses Neopixel (WS278x RGB) leds to create a
  KITT scanner.
  -------------------------------------------------------------------------
  License goes here....
  -------------------------------------------------------------------------*/


#include <Adafruit_NeoPixel.h>
#define PIN 6
int running = 1;

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip;


//--------

//#include <stdio.h>
//#include <unistd.h>


/* Knight Rider - KIT Scanner*/
#define BYTE unsigned char

// The original KIT scanner used Halogen lamps, with afterglow
#define ON_INTENSITY  30    // 300ms
#define DECAY_RATE  1   // 10 units per 10ms


BYTE lamp_brightness[8];
BYTE lamp_on_off[8];

// Function that emulates halogen light that is slowly decauing, which slowly dims the lamp
void decay_light(void)
{
  int val;
  int cnt;
  for(cnt=0;cnt<8;cnt++) {
    // only decay brightness once lamp is off
    if ( 0 ==  lamp_on_off[cnt] ) {
      val = lamp_brightness[cnt];
      val -= DECAY_RATE;
      if ( val < 0) {
        val=0;
      }
      lamp_brightness[cnt]=val;
    }
  }
}
void set_lamps( BYTE val, BYTE force_off )
{
  int cnt;
  int t=0x80;
  for (cnt=0;cnt<8;cnt++) {
    if ( val & t ) {
      lamp_on_off[cnt] = 1;
      lamp_brightness[cnt] = ON_INTENSITY;
    }
    else {
      lamp_on_off[cnt] = 0;
      //  don't clear brightness let it decay
      if ( force_off ) {
        lamp_brightness[cnt] = 0;
      }
    }
    t = t>>1;
  }
}

// Simply experimental values on red.
uint32_t red_strength[] =
{
    0x00000000,  // red
    0x00000100,  // 
    0x00000300,  // 
    0x00000A00,  // 
    0x00000F00,  // 
    0x00001000,  // 
    0x00001600,  // 
    0x00002000,  // 
    0x00002F00,  // 
    0x00005000,  // 
    0x00005000,  // 
};
void show_lamps( void )
{
int cnt;
int ret;

  for(cnt=0;cnt<8;cnt++) {
    strip.setPixelColor(cnt, red_strength[ lamp_brightness[cnt] / 3 ]);
  }
  strip.show(); // Show all pixel
  
}

int P2[] =  {
 0x80, 0xC0, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01,0x01, 
 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0xC0, 0x80,0x80, 0xFFFF,
 };
int  P1[] = { 
 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x01,0x01,
 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x80, 0xFFFF, 
 };

typedef struct {
  char *Name;   // Name of the scheme
  int timing;   // timing (in 10ms)
  int *pattern; // 0xFFFF in the end to restart
} _KITT;

_KITT KITT[] = {
// left=MSB to right=LSB, 0xFFFF = restart
// Name == NULL, stops transaction
/*
 { 
 "Normal scan",
 12,
 P1,
 }
,*/
 {
 "Fast Scan",
 9,
 P2,
 } 
,
/*
 {
 "Test 3",
 25,
 P1,
 }
,*/
 {
 NULL,
 0,
 P1,
 }
};



void _main(void)
{
  int pos=0;
  int p_cnt=0;
  int timing=0;

//  printf("KITT %s\n", KITT[p_cnt].Name);
//  printf("KITT timing %d\n", KITT[p_cnt].timing);

  set_lamps( 0x00,1 );
  show_lamps();
//  printf("\n12345678\n");

  p_cnt=0;
  pos = 0;
  timing = KITT[p_cnt].timing;
  while (running) {
    // run at 10ms speed
    delay(10);
    // Periodically  updates scanner
    if ( ++timing > KITT[p_cnt].timing ) {
      timing = 0;
      if ( 0xFFFF == KITT[0].pattern[pos] ) {
        p_cnt++;
        pos = 0;
        if (NULL == KITT[p_cnt].Name ) {
          p_cnt=0;
        }
        timing = KITT[p_cnt].timing;
        printf("KITT %s\n", KITT[p_cnt].Name);
        printf("KITT timing %d\n", KITT[p_cnt].timing);
      }
      set_lamps(KITT[p_cnt].pattern[pos],0);
      pos++;
    }
    show_lamps();
    decay_light();
  }
}


//--------
void setup() {
  // put your setup code here, to run once:
  strip = Adafruit_NeoPixel(60, PIN, NEO_RGB + NEO_KHZ800);

  strip.begin();
  
  strip.show(); // Initialize all pixels to 'off'

}

void loop() {
  // put your main code here, to run repeatedly://
  _main();

}
