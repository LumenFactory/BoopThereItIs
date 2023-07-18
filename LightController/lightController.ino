// MultiArrays - see https://github.com/FastLED/FastLED/wiki/Multiple-Controller-Examples for more info on
// using multiple controllers.  In this example, we're going to set up three NEOPIXEL strips on three
// different pins, each strip getting its own CRGB array to be played with

#include <FastLED.h>

#define SIGNAL_PIN 10

uint8_t anim = 0;
uint8_t anim_count = 0;
const uint8_t ANIM_RANDCOLOR = anim_count++;

#define NUM_LEDS_PER_STRIP 180
CRGB leds1[NUM_LEDS_PER_STRIP];
CRGB leds2[NUM_LEDS_PER_STRIP];
CRGB leds3[NUM_LEDS_PER_STRIP];
CRGB leds4[NUM_LEDS_PER_STRIP];
CRGB leds5[NUM_LEDS_PER_STRIP];
CRGB leds6[NUM_LEDS_PER_STRIP];
CRGB leds7[NUM_LEDS_PER_STRIP];
CRGB leds8[NUM_LEDS_PER_STRIP];

bool prev_state = false;
bool is_button_pressed = false;
uint32_t last_button_press = 0;

uint32_t last_draw = 0;
uint32_t wait_draw = 10;

void setup()
{
    FastLED.addLeds<NEOPIXEL, 2>(leds1, NUM_LEDS_PER_STRIP);
    FastLED.addLeds<NEOPIXEL, 14>(leds2, NUM_LEDS_PER_STRIP);
    FastLED.addLeds<NEOPIXEL, 7>(leds3, NUM_LEDS_PER_STRIP);
    FastLED.addLeds<NEOPIXEL, 8>(leds4, NUM_LEDS_PER_STRIP);
    FastLED.addLeds<NEOPIXEL, 6>(leds5, NUM_LEDS_PER_STRIP);
    FastLED.addLeds<NEOPIXEL, 20>(leds6, NUM_LEDS_PER_STRIP);
    FastLED.addLeds<NEOPIXEL, 21>(leds7, NUM_LEDS_PER_STRIP);
    FastLED.addLeds<NEOPIXEL, 5>(leds8, NUM_LEDS_PER_STRIP);

    pinMode(13, OUTPUT);
    pinMode(SIGNAL_PIN, INPUT);
}

void loop()
{
    checkSignal();
    draw();
    FastLED.show();
}

void checkSignal()
{
    is_button_pressed = digitalRead(SIGNAL_PIN);
    if (is_button_pressed != prev_state)
    {
        setAnim_randColor();
        last_button_press = millis();
    }

    prev_state = is_button_pressed;
}

void draw()
{
    if (last_draw + wait_draw < millis())
    {
        if (anim == ANIM_RANDCOLOR)
        {
            draw_randColor();
        }
        else
        {
            Serial.print("BAD ANIM STATE: ");
            Serial.println(anim);
        }

        last_draw = millis();
    }
}

typedef struct RandColorState
{
    uint8_t hue1 = 0;
    uint8_t hue2 = 0;
    uint8_t hue3 = 0;
} RandColorState_t;

RandColorState_t randColorState;

void setAnim_randColor()
{
    randColorState.hue1 = random8();
    randColorState.hue2 = random8();
    randColorState.hue3 = random8();
}

void draw_randColor()
{
    for (int i = 0; i < NUM_LEDS_PER_STRIP; i++)
    {
        leds1[i] = CHSV(randColorState.hue1, 255, 255);
        leds2[i] = CHSV(randColorState.hue2, 255, 255);
        leds3[i] = CHSV(randColorState.hue3, 255, 255);
    }
    // Do not call FastLED.show() -- its called via draw
}