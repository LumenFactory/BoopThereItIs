/*
This is the code for the animation of the tree lights. It is
responsible for controlling lights and setting the animations etc.
*/

#include <FastLED.h>

///////////////////////////////////////
// Hardware definitions
///////////////////////////////////////

#define PIN_LED 13
#define NUM_LEDS 178


#define DATA_PIN 5
#define BUTTON_PIN 6

///////////////////////////////////////
// Timing Variables
///////////////////////////////////////

uint32_t last_draw = 0;
uint32_t wait_draw = 10;

uint32_t last_update = 0;
uint32_t wait_update = 200; // This is the number of MS between frames. e.g. 500 = 2 fps

uint32_t last_buttoncheck = 0;
uint32_t wait_buttoncheck = 50;

///////////////////////////////////////
// Light Variables and Animation Variables
///////////////////////////////////////

bool is_button_pressed = false; //Check for button press
bool ledsON = false;

//List of vibe colours available
uint32_t rainbowColors[] = {
  0xFF0000, // Red
  0xFF7F00, // Orange
  0xFFFF00, // Yellow
  0x00FF00, // Green
  0x0000FF, // Blue
  0x4B0082, // Indigo
  0x8B00FF  // Violet
};

uint32_t color_select = 0;

//Define the array of LEDs
CRGB leds[NUM_LEDS];

//Setup steps go here
void setup()
{
  
  Serial.begin(115200);
  delay(1000); // Put a small delay to ensure everything is setup properly
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH); // Turn the built-in led on to show there is power
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  fill_solid(leds, NUM_LEDS, 0x000000); //turn all leds off
  FastLED.show();
  
  FastLED.setBrightness(100); //Set Brightness of LED strip

}

//Listen for button press
void check_button_state()
{
  if (digitalRead(BUTTON_PIN) == HIGH)
  {
    is_button_pressed = true;
    color_select++;
  }
  else {
    is_button_pressed = false;
  }
}

void draw()
{

  // Turn the built-in LED on when the button is pressed
  if (is_button_pressed)
  {
    digitalWrite(PIN_LED, HIGH);
  }
  else
  {
    digitalWrite(PIN_LED, LOW);
  }
}

///////////////////////////////////////
// Vibe Flash animation
//// Animation outputs colour on all LEDS when button is pressed. Sets all LEDs to black when button is depressed
///////////////////////////////////////
void vibe_flash()
{
    if(is_button_pressed) {
        fill_solid(leds, NUM_LEDS, rainbowColors[color_select]); //If button is pressed, display colour from defined colour palette
    }
    else {
      fill_solid(leds, NUM_LEDS, 0x000000); //If no button press, LEDs off
    }
    FastLED.show();
}

void loop()
{
  // put your main code here, to run repeatedly:

  // Update the ButtonState every N Milliseconds
  if (last_buttoncheck + wait_buttoncheck < millis())
  {
    check_button_state();
    last_buttoncheck = millis();
  }

  // Update the GameState every N Milliseconds
  if (last_draw + wait_draw < millis())
  {
    draw();
    vibe_flash();
    last_draw = millis();
        
    if(color_select > 6) {
      color_select = 0;
    }
}
}

