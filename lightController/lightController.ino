#define USE_OCTOWS2811
#include <OctoWS2811.h>
#include <FastLED.h>

#define SIGNAL_PIN 10
#define NUM_LEDS_PER_STRIP 180
#define NUM_STRIPS 8
const int NUM_LEDS = NUM_STRIPS * NUM_LEDS_PER_STRIP;

CRGB leds[NUM_STRIPS * NUM_LEDS_PER_STRIP];

uint8_t anim = 0;
uint8_t anim_count = 0;
const uint8_t ANIM_RANDCOLOR = anim_count++;

bool prev_state = false;
bool is_button_pressed = false;
uint32_t last_button_press = 0;

uint32_t last_draw = 0;
uint32_t wait_draw = 10;

uint8_t mode = 0;

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
        setAnim_randColor();
        last_button_press = millis();
    }

    prev_state = is_button_pressed;
}

/////////////////////////////////////////////////
// Draw Functions
/////////////////////////////////////////////////

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

/////////////////////////////////////////////////
// Utils
/////////////////////////////////////////////////

uint32_t ArrayIndex(int strip_index, int pixel_index)
{
    return strip_index * NUM_LEDS_PER_STRIP + pixel_index;
}
