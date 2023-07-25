#define USE_OCTOWS2811
#include <OctoWS2811.h>
#include <FastLED.h>
FASTLED_USING_NAMESPACE

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#define SIGNAL_PIN 10
#define NUM_LEDS_PER_STRIP 180
#define NUM_STRIPS 8
const int NUM_LEDS = NUM_STRIPS * NUM_LEDS_PER_STRIP;

CRGB leds[NUM_STRIPS * NUM_LEDS_PER_STRIP];

bool prev_state = false;
bool is_button_pressed = false;
uint32_t last_button_press = 0;

uint32_t last_draw = 0;
uint32_t wait_draw = 10;

uint8_t mode = 0;
uint8_t dir = 0;
uint8_t bri_pulse = 0;

// Annimatios
void draw_randColor();
void draw_rainbowvibe();
void draw_rainbow();
void vibe_sparkle();
void vibe_pulse();

// List of animations to cycle through.  Each is defined as a separate function below.
typedef void (*AnimationList[])();
AnimationList animations = {draw_randColor, draw_rainbowvibe, draw_rainbow, vibe_sparkle, vibe_pulse};
int current_anim = 0;

int motion_offset = 0;
uint8_t global_hue = 0;

/////////////////////////////////////////////////
// Core
/////////////////////////////////////////////////

void setup()
{

    Serial.begin(115200);

    FastLED.addLeds<OCTOWS2811>(leds, NUM_LEDS_PER_STRIP);
    FastLED.setBrightness(32);
    pinMode(13, OUTPUT);
    pinMode(SIGNAL_PIN, INPUT);

    delay(2000);
    Serial.println("Hello");
}

void loop()
{
    EVERY_N_MILLISECONDS(10) { mode = 1 - mode; };
    EVERY_N_MILLISECONDS(40) { motion_offset = (motion_offset + 1) % NUM_LEDS_PER_STRIP; };
    EVERY_N_MILLISECONDS(20) { global_hue++; };
    EVERY_N_SECONDS(10) {set_next_animation();}
    checkSignal();
    draw();
}

/////////////////////////////////////////////////
// Input Handlers
/////////////////////////////////////////////////

void checkSignal()
{
    is_button_pressed = digitalRead(SIGNAL_PIN);
    if (is_button_pressed != prev_state)
    {
        set_next_animation();
        last_button_press = millis();
    }

    prev_state = is_button_pressed;
}

/////////////////////////////////////////////////
// Draw Functions
/////////////////////////////////////////////////

void set_next_animation()
{
    current_anim = (current_anim + 1) % ARRAY_SIZE(animations);
}

void draw()
{
    if (last_draw + wait_draw < millis())
    {
        //draw_randColor();
        //draw_rainbowvibe();
        //draw_rainbow();
        //vibe_sparkle();
        vibe_pulse();
        //animations[current_anim]();
        last_draw = millis();
    }
    FastLED.show();
}

typedef struct RandColorState
{
    uint8_t hue = 0;
    uint8_t offset = 40;
} RandColorState_t;

RandColorState_t randColorState;

void setAnim_randColor()
{
    randColorState.hue = random8();
}

void draw_randColor()
{
    for (int s = 0; s < NUM_STRIPS; s++)
    {
        for (int i = 0; i < NUM_LEDS_PER_STRIP; i++)
        {
            leds[ArrayIndex(s, i)] = CHSV(randColorState.hue + s * randColorState.offset, 255, 255);
        }
    }
}

void draw_rainbowvibe()
{
    if (is_button_pressed)
    {
        fill_rainbow(leds, NUM_LEDS, global_hue, 1); // If button pressed, light up rainbow
    }
    else
    {
        for (int i = 0; i < NUM_LEDS; i++)
        {
            leds[i] = CHSV(0, 0, 0);
        } // If no button press, LEDs off
    }
}

void draw_rainbow()
{
    uint8_t bri = (is_button_pressed) ? 255 : 0;
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = CHSV(i + motion_offset, 200, bri);
    }
}


void vibe_sparkle() 
{
   if(is_button_pressed) 
   {
    for(int i = 0; i < NUM_LEDS; i++)
    {
	uint8_t bri = 255;
		leds[i] = CHSV(global_hue, 200, bri);
    addGlitter(10);
	    }
    }
    else {
      for(int i = 0; i < NUM_LEDS; i++){
      leds[i] = CHSV(0, 0, 0);
      } //If no button press, LEDs off
    }
  
  //Color change if button is released
  if(is_button_pressed && !prev_state) {
    global_hue = random8();
  }
}

///////////////////////////////////////
// Glitter function from FastLED Demo Reel
///////////////////////////////////////
void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

///////////////////////////////////////
// Vibe pulse
//// Upon press, pulse the LEDs on the recieved vibe - increase and decrease the speed of the pulse
///////////////////////////////////////
void vibe_pulse()
{
   if(is_button_pressed) {
      for(int i = 0; i < NUM_LEDS; i++){
		leds[i] = CHSV(global_hue, 200, bri_pulse);
  }
	}
    else {
      for(int i = 0; i < NUM_LEDS; i++){
      leds[i] = CHSV(0, 0, 0);
      } //If no button press, LEDs off
    }

  //if(is_button_pressed == true & last_buttonpress == true) 
   bri_pulse += dir; // Increase brightness by 1
  // Check if brightness maxed
  if (bri_pulse >= 200) {
    dir = -2; // decrease brightness
  } else if (bri_pulse < 10) {
    dir = 2; // increase brightenss
  }
  //Color change if button is released
  if(!is_button_pressed & prev_state) {
    global_hue = random8();
  }
}

/////////////////////////////////////////////////
// Utils
/////////////////////////////////////////////////

uint32_t ArrayIndex(int strip_index, int pixel_index)
{
    return strip_index * NUM_LEDS_PER_STRIP + pixel_index;
}
