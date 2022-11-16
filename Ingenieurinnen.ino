/*
 * A simple Script made by 077 for a workshop concearning led stripes (WS2812B) and Arduinos.
 * Buttons serve as inputs and (de-)activate the stripe and/or change the light pattern of the stripe
 * 
 * License: GNU GPLv3
 * 
 * 077, 16/11/2022
 */

#include <Adafruit_NeoPixel.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        8 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 10 // Popular NeoPixel ring size

// constants won't change. They're used here to set pin numbers:
const int button1Pin = 2;     // the number of the 1st pushbutton pin
const int button2Pin = 4;     // the number of the 2nd pushbutton pin

// variables for button1 which will change:
int button1PushCounter = 0;   // counter for the number of 1st button presses
int button1State = 0;         // current state of the 1st button
int lastButton1State = 0;     // previous state of the 1st button

// variables for button2 which will change:
int button2PushCounter = 0;   // counter for the number of 2nd button presses
int button2State = 0;         // current state of the 2nd button
int lastButton2State = 0;     // previous state of the 2nd button

bool activated = true;        // boolean to show/save if leds are currently activated or disabled by button 1
int ledstripemode = 1;        // int to show/save wich led mode/pattern we're in

// variables for mode1() function
unsigned long timer1 = 0;
// variables for mode2() function
unsigned long timer2 = 0;
int hsvColor = 0;
// variables for mode3() function
unsigned long timer3 = 0;
int counter = 0;

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel stripe(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels

void setup() {
  stripe.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  stripe.setBrightness(200); // set overall brightness
  
  // initialize the pushbutton pins as an inputs:
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);

  Serial.begin(115200); // initailize serial output for debugging
}

void button1StateChangeDetection() {
  // read the pushbutton input pin:
  button1State = digitalRead(button1Pin);

  // compare the buttonState to its previous state
  if (button1State != lastButton1State) {
    // if the state has changed, increment the counter
    if (button1State == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      // switch status of boolean activated to its opposite (true/false)
      activated = !activated;
    } else {
      // if the current state is LOW then the button went from on to off:
      Serial.println("Button 1 was released.");
    }
    // Delay a little bit to avoid bouncing
    delay(10);
  }
  // save the current state as the last state, for next time through the loop
  lastButton1State = button1State;
}

void button2StateChangeDetection() {
  // read the pushbutton input pin:
  button2State = digitalRead(button2Pin);

  // compare the buttonState to its previous state
  if (button2State != lastButton2State) {
    // if the state has changed, increment the counter
    if (button2State == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      ledstripemode++;
      // reset ledstripemode to 1 if limit was reached
      if (ledstripemode > 3) {
        ledstripemode = 1;
      }
      Serial.println(String(ledstripemode));
    } else {
      // if the current state is LOW then the button went from on to off:
      Serial.println("Button 2 was released.");
    }
    // Delay a little bit to avoid bouncing
    delay(10);
  }
  // save the current state as the last state, for next time through the loop
  lastButton2State = button2State;
}

void modeCaseSwitch() {
  // checks which light pattern mode we're in and refers to the corresponding function
  switch (ledstripemode) {
    case 1:
      mode1();
      break;
    case 2:
      mode2();
      break;
    case 3:
      mode3();
      break;
    default: // debugging & and error catching
      Serial.println("Unknown input for mode case switch!!!!");
      break;
  }
}

bool checkForDelayPassed(unsigned long timer, unsigned long timeout) {
  // check if stopwatch "timer" + requested delaytime "timeout" are larger than current time
  if (millis() > timeout + timer) { 
    // if this is the case, return true
    return true;
  }
  else {
    // if this is yet not the case, return false
    return false;
    //Serial.println("delay not reached!!");
  }
}

void mode1() {                            // WHOLE STRIPE POLICE LIGHT FLASH MODE
  static bool ledsactivated = false;      // a boolean to save state, wether the stripe is currently activated or not
  
  if (checkForDelayPassed(timer1, 150)) { // use checkForDelayPassed function to check if our requested time has already passed
    timer1 = millis();                    // reset stopwatch "timer1"
    ledsactivated = !ledsactivated;       // change state to opposite of itself
    if (ledsactivated) {
      for(int i=0; i<NUMPIXELS; i++) {    // cycle through all leds
        stripe.setPixelColor(i, stripe.Color(0, 0, 255));   // and turn each one blue
      }
      stripe.show();                      // update the changes to the real strip
    }
    else {
      stripe.clear();                     // set all leds to off
      stripe.show();                      // update the changes to the real strip
    }
  }
}

void mode2() {                            // WHOLE STRIPE RAINBOW MODE
  
  if (checkForDelayPassed(timer2, 10)) {  // use checkForDelayPassed function to check if our requested time has already passed
    timer2 = millis();                    // reset stopwatch "timer2"
    hsvColor += 50;                       // increment colorwheel value
    if (hsvColor>65536) {hsvColor = 0;}   // reset back to 0 if maximum colorwheel value of 65536 is reached
    for(int i=0; i<NUMPIXELS; i++) {      // cycle through all leds
        stripe.setPixelColor(i, stripe.gamma32(stripe.ColorHSV(hsvColor)));   // and give it the current color
    }
    stripe.show();                        // update the changes to the real strip
  }
}

void mode3() {                            // CLIMBING AND DESCENDING RAINBOW CYCLING MODE
  stripe.clear();                         // set all leds to off

  if (checkForDelayPassed(timer3, 100)) { // use checkForDelayPassed function to check if our requested time has already passed
    timer3 = millis();                    // reset stopwatch "timer3"
    if (counter < NUMPIXELS) {counter++;} // increment counter if counter isnt yet larger than the number of leds on the stripe
    else {counter = 0;}                   // if counter variable is larger, then reset the counter variable to 0
    for(int i=0; i<counter; i++) {        // cycle to all leds on strip between position 0 and the current value of counter variable
        stripe.setPixelColor(i, stripe.gamma32(stripe.ColorHSV(65536/10*i)));   // and give it the color, depending on position on the strip
    }
    stripe.show();                        // update the changes to the real strip
  }
}

void loop() {                             // REPEAT everything in this loop until infinity
  // Check if State of Button 1 and 2 have changed since last loop iteration
  button1StateChangeDetection();
  button2StateChangeDetection();

  // Check wether leds are currently activated or deactivated by Button 1
  if (activated) {    // if so, go to function modeCaseSwitch()
    modeCaseSwitch();
  }
  else {              // if not, clear the led stripe and go on with the loop
    stripe.clear();
    stripe.show();
  }

}
