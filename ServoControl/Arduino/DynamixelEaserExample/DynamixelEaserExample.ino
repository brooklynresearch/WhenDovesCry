/*  
DYNAMIXEL EASER EXAMPLE
by: Ezer Longinus
June 2015

This library is based on todbot ServoEaser library for standard Servos.
The library is just ported to work with Dynamixel Servos.

This example moves up to three servos to the list of positions in the "moves" array.

*/

//INCLUDE FILES
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Dynamixel_Serial.h>       // Library needed for dynamixel servos
#include <DynamixelEaser.h>         // Library needed for easing the dynamixels
#include <SoftwareSerial.h>         // Library needed for software serial

#define OLED_RESET 3
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

//DEFINE STATEMENTS
#define SERVO_ID_1 0x01               // ID of which we will set Dynamixel Servo 1 to 
#define SERVO_ID_2 0x02               // ID of which we will set Dynamixel Servo 2 to
#define SERVO_ID_3 0x03               // ID of which we will set Dynamixel Servo 3 to
#define CONTROL_PIN 0x02              // This is data control pin.  This is for converting full to half duplex serial

//MOVES LIST
int listLength = 3;
int shoulderMoves[3] = {10, 1023, 2047};
int upperArmMoves[3] = {10, 2047, 10};
int index = 0;

//INITIALIZE SOFTWARE SERIAL - FOR DEBUGGING
SoftwareSerial mySerial(8, 9);    // RX, TX for software serial

//INITIALIZE EXTERNAL INPUTS AND OUTPUTS
const int ledPin   = 13; 
const int buttonPin = 3;
boolean buttonState = false;
boolean firstTime = true;

int servoFrameMillis = 5;  // minimum time between servo updates

//INITIALIZE SERVO MOTORS
DynamixelEaser servo;
DynamixelEaser servo2;
DynamixelEaser servo3;

void setup() {
  //Set baud rate for debug serial
  mySerial.begin(57600);
  mySerial.println("Starting Debugger");
  
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  //display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  // init done
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  //display.display();
  
  //Set default baud rate and control pin for all servos.  
  //Only need to call init once.  Any servo will do.  Just need to establish
  //baud and control pin.
  //DEFAULT BAUD = 1000000 :: DEFAULT CONTROL PIN = 2
  servo.init();
  //Assign servos to their ids and frame rate
  servo.begin(SERVO_ID_1, servoFrameMillis);
  servo2.begin(SERVO_ID_2, servoFrameMillis);
  servo3.begin(SERVO_ID_3, servoFrameMillis);
  //servo.easeTo(10, 2000);
  //set buttonPin to digital input
  pinMode(buttonPin, INPUT_PULLUP);
  //send message to debug serial so we know we're up and running
  mySerial.println("Dynamixel Example: ");
  mySerial.println("Move one dynamixel servo back and forth");
//  servo.setEasingFunc(easeOut);
//  servo2.setEasingFunc(easeOut);
//  servo3.setEasingFunc(easeOut);
}

void loop() {
  //Update the position/speed of the servos
  servo.update();
  servo2.update();
  servo3.update();
  
  buttonState = digitalRead(buttonPin);

  if( servo.hasArrived() || servo2.hasArrived()) { 
    mySerial.print("Running: ");mySerial.println(servo.isRunning());
    //index to the next position on the moves array
    index = (index + 1) % listLength;
    //debug to let us know we've arrived
    mySerial.print("ServoEaser arrived at ");mySerial.print(shoulderMoves[index]);mySerial.println(" degrees,");
    //send the next position to the servos.
    servo.easeTo( shoulderMoves[index], 2000 );
    servo2.easeTo( upperArmMoves[index], 2000 );
    servo3.easeTo( upperArmMoves[index], 2000 );
  } 
}

float easeIn (float t,float b , float c, float d) {
	return c - easeOut (d-t, 0, c, d) + b;
}
float easeOut(float t,float b , float c, float d) {
	if ((t/=d) < (1/2.75f)) {
		return c*(7.5625f*t*t) + b;
	} else if (t < (2/2.75f)) {
		float postFix = t-=(1.5f/2.75f);
		return c*(7.5625f*(postFix)*t + .75f) + b;
	} else if (t < (2.5/2.75)) {
			float postFix = t-=(2.25f/2.75f);
		return c*(7.5625f*(postFix)*t + .9375f) + b;
	} else {
		float postFix = t-=(2.625f/2.75f);
		return c*(7.5625f*(postFix)*t + .984375f) + b;
	}
}

float easeInOut(float t,float b , float c, float d) {
	if (t < d/2) return easeIn (t*2, 0, c, d) * .5f + b;
	else return easeOut (t*2-d, 0, c, d) * .5f + c*.5f + b;
}
