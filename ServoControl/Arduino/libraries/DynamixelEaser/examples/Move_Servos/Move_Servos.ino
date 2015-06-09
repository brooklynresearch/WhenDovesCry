/*  
DYNAMIXEL EASER EXAMPLE
by: Ezer Longinus
June 2015

This library is based on todbot ServoEaser library for standard Servos.
The library is just ported to work with Dynamixel Servos.

This example moves up to three servos to the list of positions in the "moves" array.

*/

//INCLUDE FILES
#include <Dynamixel_Serial.h>       // Library needed for dynamixel servos
#include <DynamixelEaser.h>         // Library needed for easing the dynamixels
#include <SoftwareSerial.h>         // Library needed for software serial


//DEFINE STATEMENTS
#define SERVO_ID_1 0x01               // ID of which we will set Dynamixel Servo 1 to 
#define SERVO_ID_2 0x02               // ID of which we will set Dynamixel Servo 2 to
#define SERVO_ID_3 0x03               // ID of which we will set Dynamixel Servo 3 to
#define CONTROL_PIN 0x02              // This is data control pin.  This is for converting full to half duplex serial

//MOVES LIST
int listLength = 3;
int moves[3] = {10, 1023, 2047};
int index = 0;

//INITIALIZE SOFTWARE SERIAL - FOR DEBUGGING
SoftwareSerial mySerial(10, 11);    // RX, TX for software serial

//INITIALIZE EXTERNAL INPUTS AND OUTPUTS
const int ledPin   = 13; 
const int buttonPin = 3;
boolean buttonState = false;
boolean firstTime = true;

int servoFrameMillis = 20;  // minimum time between servo updates

//INITIALIZE SERVO MOTORS
DynamixelEaser servo;
DynamixelEaser servo2;
DynamixelEaser servo3;

void setup() {
  //Set baud rate for debug serial
  mySerial.begin(57600);
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
}

void loop() {
  //Update the position/speed of the servos
  servo.update();
  servo2.update();
  servo3.update();
  
  buttonState = digitalRead(buttonPin);

  if( servo.hasArrived() || servo2.hasArrived() ) { 
    //index to the next position on the moves array
    index = (index + 1) % listLength;
    //debug to let us know we've arrived
    mySerial.print("ServoEaser arrived at ");mySerial.print(index);mySerial.println(" degrees,");
    //send the next position to the servos.
    servo.easeTo( moves[index], 2000 );
    servo2.easeTo( moves[index], 2000 );
    servo3.easeTo( moves[index], 2000 );
  } 
}
