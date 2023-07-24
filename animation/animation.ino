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

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

///////////////////////////////////////
// Timing Variables
///////////////////////////////////////

uint32_t last_draw = 0;
uint32_t wait_draw = 10;

uint32_t last_update = 0;
uint32_t wait_update = 10; // This is the number of MS between frames. e.g. 500 = 2 fps

uint32_t last_buttoncheck = 0;
uint32_t wait_buttoncheck = 10;

uint32_t flashing_start_time = 0;

///////////////////////////////////////
// Light Variables and Animation Variables
///////////////////////////////////////

bool is_button_pressed = false; //Check for button press
bool last_buttonpress = false; // This variable stores whether the button was pressed in the last loop

uint8_t global_hue = random(255); //Sets hue of CHSV to a random value
uint8_t pos = 0; //initial location of the cursor
uint8_t bri = 255; //brightness of LEDs
int dir = -1; //direction of brightness change for pulse animation

int chance_of_firework = 3; //set the likelihood of a firework going off in the firework animation
bool go_firework = false; //variable to store whether firework should be shot off or not

//Define the array of LEDs
CRGB leds[NUM_LEDS];

//List of animations available to call:
  void vibe_flash();
  void rainbow_vibe();
  void vibe_cursor_spin();
  void trunk_charge();
  void vibe_pulse();
  void vibe_fireworks();
  void vibe_sparkle();

// List of animations to cycle through.  Each is defined as a separate function below.
typedef void (*animation_list[])();
animation_list animations = { vibe_cursor_spin, vibe_pulse, vibe_sparkle, vibe_fireworks, vibe_pulse, vibe_cursor_spin, vibe_pulse, vibe_sparkle, vibe_fireworks, vibe_pulse, rainbow_vibe };


uint8_t current_animation_number = 0; // Index number of which animation is current


//Setup steps go here
void setup()
{
  
  Serial.begin(115200);
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


void next_animation()
{
  // add one to the current animation number, and wrap around at the end
  current_animation_number = (current_animation_number + 1) % ARRAY_SIZE(animations);
}

///////////////////////////////////////
// Animation List
// 1. Vibe Flash
// 2. Vibe Rainbow
// 3. Vibe Cursor Spin
// 4. Truck Charge 
// 5. Vibe Pulse
// 6. Vibe Fireworks
// 7. Vibe Sparkle
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
  fadeToBlackBy( leds, NUM_LEDS, 3);
  leds[pos] += CHSV( global_hue, 255, 255);
  }
  else {
      for(int i = 0; i < NUM_LEDS; i++){
      leds[i] = CHSV(0, 0, 0);
      } //If no button press, LEDs off
    }
  FastLED.show();
  
if (pos >= NUM_LEDS){
  pos = 0;
  } //reset position of cursor back to 0 to spin it around the hoop

  if(is_button_pressed == true & last_buttonpress == true){
  pos += 1;
  
  }
  //
  if(is_button_pressed == false & last_buttonpress == true) {
    global_hue = random8();
    pos = random(180);
  }
  last_buttonpress = is_button_pressed; 
  Serial.println(pos);
}

///////////////////////////////////////
// 4. Trunk charge animation
//// Upon press, "charge up" the LEDs on the tree by turning on LEDs in series until reaching full bar
///////////////////////////////////////
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

///////////////////////////////////////
// 5. Vibe pulse
//// Upon press, pulse the LEDs on the recieved vibe - increase and decrease the speed of the pulse
///////////////////////////////////////
void vibe_pulse(){
   if(is_button_pressed) {
      for(int i = 0; i < NUM_LEDS; i++){
		leds[i] = CHSV(global_hue, 200, bri);
  }
	}
    else {
      for(int i = 0; i < NUM_LEDS; i++){
      leds[i] = CHSV(0, 0, 0);
      } //If no button press, LEDs off
    }
    FastLED.show(); 

  if(is_button_pressed == true & last_buttonpress == true) {
   bri += dir; // Increase brightness by 1
  }
  // Check if brightness maxed
  if (bri >= 200) {
    dir = -2; // decrease brightness
  } else if (bri < 10) {
    dir = 2; // increase brightenss
  }
  //Color change if button is released
  if(is_button_pressed == false & last_buttonpress == true) {
    global_hue = random8();
    Serial.println(global_hue);
  }
  last_buttonpress = is_button_pressed;
}

///////////////////////////////////////
// 6. Vibe Fireworks
//// Upon press, flash hoops and bars at random times
///////////////////////////////////////
void vibe_fireworks(){

static bool flashing = false;

///green light to launch firework if probability condition is satisfied
    if(random8() < chance_of_firework){
    go_firework = true;
  }

//If button is pressed for first time when animation is not flashing, then turn flashing on and log start of flashing animation
if(is_button_pressed){
  if(!flashing){
    flashing_start_time = millis();
  }
}
// Flash firework if button is down and have green light to launch. Hold the flash for 1 second 
    if(is_button_pressed & go_firework) {
      if(millis() - flashing_start_time <= 1000){
        for(int i = 0; i < NUM_LEDS; i++){
		    uint8_t bri = 255;
		    leds[i] = CHSV(global_hue, 200, bri);
        addGlitter(5);
        flashing = true;
      }
	}
  //Fade out the firework after being shown for one second
    else if(millis() - flashing_start_time >= 1000 && millis() - flashing_start_time <= 2000) {
      for(int i = 0; i < NUM_LEDS; i++){
      leds[i] = CHSV(global_hue, 200, bri);
      }
      bri = bri - 5;
      flashing = true;
    }
    //turn lights off after 2 seconds have passed since firework launch
    else if(millis() - flashing_start_time >= 2000) {
     for(int i = 0; i < NUM_LEDS; i++){
    leds[i] = CHSV(0, 0, 0);
    }
    flashing = false;
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
  if(!flashing)
    {go_firework = false;}
}


///////////////////////////////////////
// 7. Vibe Sparkle
//// Upon press, vibe is lit and sparkles while held
///////////////////////////////////////
void vibe_sparkle() 
{
   if(is_button_pressed) {
      for(int i = 0; i < NUM_LEDS; i++){
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
    
    FastLED.show();
  
  //Color change if button is released
  if(is_button_pressed == false & last_buttonpress == true) {
    global_hue = random8();
    Serial.println(global_hue);
  }
  last_buttonpress = is_button_pressed;
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

///////////////////////////////////////
//Draw the animation selected
void draw()
{
  // Turn the built-in LED on when the button is pressed
  if (is_button_pressed)
  {
    digitalWrite(PIN_LED, HIGH);
    //animations[current_animation_number]();
    
  }
  else if (!is_button_pressed)
  {
    digitalWrite(PIN_LED, LOW);
  }
  //For testing, input which animation to use here
  //rainbow_vibe();
  //vibe_cursor_spin();
  //vibe_pulse();
  //vibe_fireworks();
  //vibe_sparkle();
  // vibe_flash();

  //Call animation from animation array
  animations[current_animation_number]();
  if(!is_button_pressed)
    {EVERY_N_SECONDS( 10 ) { next_animation(); }} // change patterns every 10 seconds only if button is up
  
}

///////////////////////////////////////
//Run code
void loop()
{
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