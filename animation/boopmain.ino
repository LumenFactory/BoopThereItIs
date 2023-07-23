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
uint32_t wait_update = 10; // This is the number of MS between frames. e.g. 500 = 2 fps

uint32_t last_buttoncheck = 0;
uint32_t wait_buttoncheck = 10;

///////////////////////////////////////
// Light Variables and Animation Variables
///////////////////////////////////////

bool is_button_pressed = false; //Check for button press
bool last_buttonpress = false; // This variable stores whether the button was pressed in the last loop

uint8_t global_hue = 0; //Sets hue of CHSV to 0
int pos = 0; //initial location of the cursor

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
  }
  else {
    is_button_pressed = false;
  }
}
///////////////////////////////////////
// Animation List
// 1. Vibe Flash
// 2. Vibe Rainbow
// 3. Vibe Cursor Spin
// 4. Truck charge 
///////////////////////////////////////


///////////////////////////////////////
// 1. Vibe Flash animation
//// Animation outputs colour on all LEDS when button is pressed. Sets all LEDs to black when button is depressed
///////////////////////////////////////
void vibe_flash()
{
    if(is_button_pressed) {
      for(int i = 0; i < NUM_LEDS; i++){
		uint8_t bri = 255;
		leds[i] = CHSV(global_hue, 200, bri);
	}
  }
    else {
      for(int i = 0; i < NUM_LEDS; i++){
      leds[i] = CHSV(0, 0, 0);
      } //If no button press, LEDs off
    }
    FastLED.show();
  
  //Color change if button is released
  if(is_button_pressed == false & last_buttonpress == true) {
    global_hue = random8();
    Serial.println(global_hue);
  }
  last_buttonpress = is_button_pressed;
}

///////////////////////////////////////
// 2. Vibe Rainbow animation
//// Animation displays spinning rainbow through LEDS when button is pressed. Sets all LEDs to black when button is depressed
///////////////////////////////////////
void rainbow_vibe()
{
    if(is_button_pressed) {
     fill_rainbow( leds, NUM_LEDS, global_hue, 1); // If button pressed, light up rainbow
  }
    else {
      for(int i = 0; i < NUM_LEDS; i++){
      leds[i] = CHSV(0, 0, 0);
      } //If no button press, LEDs off
    }
    FastLED.show();
  
  //Hold colors if continuous press
  if(is_button_pressed == true & last_buttonpress == true) {
    global_hue = global_hue + 3;
  }
  last_buttonpress = is_button_pressed;
}

///////////////////////////////////////
// 3. Vibe Cursor Spin animation
//// Animation displays a cursor of leds that cycle through the strip when button pressed. Sets all LEDs to black when button is depressed
///////////////////////////////////////
void vibe_cursor_spin()
{
  // a colored dot moving around a cicle, with fading trails
  if(is_button_pressed){
  fadeToBlackBy( leds, NUM_LEDS, 20);
  leds[pos] += CHSV( global_hue, 255, 255);
  }
  else {
      for(int i = 0; i < NUM_LEDS; i++){
      leds[i] = CHSV(0, 0, 0);
      } //If no button press, LEDs off
    }
  FastLED.show();
  pos++;
  
  if (pos > 255){
  pos = 0;
  } //reset position of cursor back to 0 to spin it around the hoop

  //
  if(is_button_pressed == false & last_buttonpress == true) {
    global_hue = random8();
  }
  last_buttonpress = is_button_pressed; 
}

void trunk_charge(){
  // Charge up the trunk with a green light
  if(is_button_pressed){
  leds[pos] = CHSV(96, 237, 255);
  pos++;
  }
  else {
      for(int i = 0; i < NUM_LEDS; i++){
      leds[i] = CHSV(0, 0, 0);
      pos = 0;
      } //If no button press, LEDs off
   }
  FastLED.show();
}

//Draw the animation selected
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
  // Input which animation to use here
  //vibe_flash();
  //rainbow_vibe();
  //vibe_cursor_spin();
  trunk_charge();
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
    last_draw = millis();
  }
}