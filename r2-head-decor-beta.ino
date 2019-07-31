

// A basic everyday NeoPixel strip test program.

// NEOPIXEL BEST PRACTICES for most reliable operation:
// - Add 1000 uF CAPACITOR between NeoPixel strip's + and - connections.
// - MINIMIZE WIRING LENGTH between microcontroller board and first pixel.
// - NeoPixel strip's DATA-IN should pass through a 300-500 OHM RESISTOR.
// - AVOID connecting NeoPixels on a LIVE CIRCUIT. If you must, ALWAYS
//   connect GROUND (-) first, then +, then data.
// - When using a 3.3V microcontroller with a 5V-powered NeoPixel strip,
//   a LOGIC-LEVEL CONVERTER on the data line is STRONGLY RECOMMENDED.
// (Skipping these may work OK on your workbench but can fail in the field)

#include <Metro.h> //Metro Timing Library

//For NeoPixel manipulation , index, address /pin locations
#define LED1 1;
#define LED2 2;
#define LED3 3;
// Create variables to hold the LED states
int state1 = HIGH;
int state2 = HIGH;
int state3 = HIGH;
boolean stepper_center = true;

// Instantiate a metro object and set the interval
Metro metro1 = Metro(700);  //neopixel 0
Metro metro2 = Metro(500);  //neopixel 1
Metro metro3 = Metro(3000); //neopixel 2
Metro metroStepper = Metro(5); //Stepper motion thread


#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    6

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 3


//0 is center (from startup pos)  1 is right   -1 is left
int stepper_pos = 0;

//prev_route
int prev_route =0;  // sets the previous route taken, for reversal
int route = 0;


//For Serial Controller
int incomingByte = 0;


#include <Stepper.h>

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
// for your motor
int steps2go = 0;


// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 7, 9, 10, 11);


// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)


//------System Variables
int tik = 0;  //clock cycler millis out of 1000 / set cycle loop
int steps2wait=0;
int wait_count =0;
int wait_limit = 10000; //millis of random wait time


// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}




// setup() function -- runs once at startup --------------------------------
void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  // set the speed at 60 rpm:
  myStepper.setSpeed(60);
  // initialize the serial port:
  Serial.begin(9600);

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  colorWipe(strip.Color(0,150,20), 10);
  strip.show();
  delay(3000);
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
}


// loop() function -- runs repeatedly as long as board is on ---------------

void loop() {


  if (metro1.check() == 1) { // check if the metro has passed its interval .
    if (state1==HIGH)  { 
      state1=LOW;
      strip.setPixelColor(0, 25,0,0);
      strip.show();
    } else {
      state1=HIGH;
      strip.setPixelColor(0,0,0,25);
      strip.show();
    }
    //digitalWrite(LED1,state1);
  }


  
  if (metro2.check() == 1) { // check if the metro has passed its interval .
    if (state2==HIGH)  { 
      state2=LOW;
      strip.setPixelColor(1, 0,5,25);
      strip.show();
    } else {
      state2=HIGH;
      strip.setPixelColor(1,100,50,105);
      strip.show();
    }
    //digitalWrite(LED1,state1);
  }


  
  if (metro3.check() == 1) { // check if the metro has passed its interval .
    if (state3==HIGH)  { 
      state3=LOW;
      strip.setPixelColor(2, 0,0,0);
      strip.show();
    } else {
      state3=HIGH;
      strip.setPixelColor(2,25,25,25);
      strip.show();
    }
    //digitalWrite(LED1,state1);
  }

  
  if (metroStepper.check() == 1) { // check if the metro has passed its interval .

    if(route ==0) {  //check if center and no routes
        if (stepper_center == true){
          wait_count = wait_count + 1;
          if (wait_count >= steps2wait){
            wait_count = 0;
            steps2wait = random(wait_limit);
            int ww = random(2);  //sets a course
              if (ww == 1){
                route=1;
                stepper_center = false;
              }else
                route=-1;
                stepper_center = false;
              }
          }
        }
    } 
    
    if (route==1)  { 
      //going to the right;

      //if checks if at destination
      if(steps2go >= stepsPerRevolution){
        if ( prev_route == 0 ){
          stepper_center = false;
        }
        if ((prev_route == -1) && (stepper_center == false)){
          stepper_center = true;
        }

        
        route= 0; //kill route
        prev_route = 1;
        steps2go = 0;
      } else {
        myStepper.step(1);
        steps2go = steps2go + 1;
      }
    }///////end motor stepper route 1

    if (route == -1)  { 
      //going to the right;
      
      //if checks if at destination
      if(steps2go >= stepsPerRevolution){
        if ( prev_route == 0 ){
          stepper_center = false;
        }
        if ( (prev_route == 1) && (stepper_center == false)){
          stepper_center = true;
        }
        
        route= 0; //end routes
        prev_route = -1;
        steps2go = 0;
      } else {
        myStepper.step(-1);
        steps2go = steps2go + 1;
      }      
    }///////end motor stepper route 1   
    //digitalWrite(LED1,state1);
  

  //do step motor

  if (Serial.available() > 0){
    incomingByte = Serial.read();
    Serial.println(incomingByte);
    Serial.println(incomingByte, DEC);
    if (incomingByte == 97){
      //step one revolution in the other direction:
      Serial.print("counterclockwise");
      //myStepper.step(-stepsPerRevolution);
      route = 1;
      stepper_center = true;
      delay(3);
    }
    if (incomingByte == 100){
      //step one revolution in the other direction:
      Serial.print("counterclockwise");
      //myStepper.step(stepsPerRevolution);
      route = -1;
      stepper_center=true;
      delay(3);
    }
    
  }

}

// Some functions of our own for creating animated effects -----------------

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait) {
  for(int a=0; a<10; a++) {  // Repeat 10 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++) {  // Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}
/*
 Stepper Motor Control - one revolution

 This program drives a unipolar or bipolar stepper motor.
 The motor is attached to digital pins 8 - 11 of the Arduino.

 The motor should revolve one revolution in one direction, then
 one revolution in the other direction.


 Created 11 Mar. 2007
 Modified 30 Nov. 2009
 by Tom Igoe

 */

 void left_right(){

        
      strip.setPixelColor(1,0,0,0);
      strip.setPixelColor(2,0,0,100);
      strip.setPixelColor(3,0,20,26);
      strip.show();
      myStepper.step(-40);
      delay(3);
      strip.setPixelColor(1,0,0,0);
      strip.setPixelColor(2,100,0,0);
      strip.setPixelColor(3,0,20,26);
      strip.show();
      myStepper.step(-40);
      strip.setPixelColor(1,0,0,0);
      strip.setPixelColor(2,0,0,100);
      strip.setPixelColor(3,0,20,26);
      strip.show();
      myStepper.step(40);
      strip.setPixelColor(1,0,0,0);
      strip.setPixelColor(2,0,0,100);
      strip.setPixelColor(3,0,20,26);
      strip.show();
      myStepper.step(40);
      strip.setPixelColor(1,0,0,0);
      strip.setPixelColor(2,0,0,100);
      strip.setPixelColor(3,0,20,26);
      strip.show();
 
 }
