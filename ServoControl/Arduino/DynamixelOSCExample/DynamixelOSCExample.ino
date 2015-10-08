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
#include <Dynamixel_Serial.h>       // Library needed for dynamixel servos
#include <DynamixelEaser.h>         // Library needed for easing the dynamixels
#include <SoftwareSerial.h>         // Library needed for software serial
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <OscUDP.h>

/*------------ ETHERNET SETUP ---------------*/
byte arduionMac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0x85, 0x2A };
// listeningIP ==  SHIELD initialization IP
IPAddress arduinoIP(192,168,0,200); // you need to set this
// listening -- port
unsigned int arduinoPort = 12345;      // local port to listen on
// speaking 
// set up our destination NODE
NetAddress myComputer;
IPAddress myComputerIP(192,168,0,124); 
int myComputerPort = 12346;
// setup a UDP object
EthernetUDP UDP;
/*-------------------------------------------*/

/*-------------- OSC SETUP ------------------*/
// OUR OSC MESSAGE OBJECT
OscUDP etherOSC;  
// Various osc messages we'll be prepending and sending back to 
// the main computer
OscMessage shoulderPitchMsg("/Pos/sPitch/");
OscMessage shoulderRollMsg("/Pos/sRoll/");
OscMessage elbowPitchMsg("/Pos/ePitch/");
OscMessage elbowYawMsg("/Pos/eYaw/");
/*-------------------------------------------*/

//DEFINE STATEMENTS
#define SERVO_ID_1 0x01               // ID of which we will set Dynamixel Servo 1 to 
#define SERVO_ID_2 0x02               // ID of which we will set Dynamixel Servo 2 to
#define SERVO_ID_3 0x03               // ID of which we will set Dynamixel Servo 3 to
#define CONTROL_PIN 0x02              // This is data control pin.  This is for converting full to half duplex serial
#define SHOULDER_PITCH 0x01               // ID of which we will set Dynamixel Servo 1 to 
#define SHOULDER_ROLL_1 0x02               // ID of which we will set Dynamixel Servo 2 to
#define SHOULDER_ROLL_2 0x03               // ID of which we will set Dynamixel Servo 3 to
#define CONTROL_PIN 0x02              // This is data control pin.  This is for converting full to half duplex serial
#define SERVO_BAUD 57600
#define CW_LIMIT_ANGLE 0x001        // lowest clockwise angle is 1, as when set to 0 it set servo to wheel mode
#define CCW_LIMIT_ANGLE 0xFFF       // Highest anit-clockwise angle is 0XFFF, as when set to 0 it set servo to wheel mode

//MOVES LIST
int listLength = 3;
int servoCurrentMove[3][2] = {{0,0},{0,0},{0,0}};

//INITIALIZE SOFTWARE SERIAL - FOR DEBUGGING
SoftwareSerial mySerial(8, 9);    // RX, TX for software serial

//INITIALIZE EXTERNAL INPUTS AND OUTPUTS
const int ledPin   = 13; 
const int buttonPin = 3;
boolean buttonState = false;
boolean firstTime = true;

int servoFrameMillis = 1;  // minimum time between servo updates

DynamixelEaser shoulderPitchServo;
DynamixelEaser shoulderRollServo1;
DynamixelEaser shoulderRollServo2;

void setup() {
  //Set baud rate for debug serial
  mySerial.begin(57600);
  mySerial.println("Starting Debugger");
  
  // Initialize Ethernets
  Ethernet.begin(arduionMac ,arduinoIP);
  
  // start UDP object, listening on port listeningPort
  UDP.begin(arduinoPort);
  // set up our communication protocol
  // pass the UDP object to OSC
  etherOSC.begin(UDP);
  // define our destination location
  myComputer.set(myComputerIP, myComputerPort);
  
  //INITIALIZE SERVO MOTORS
  shoulderPitchServo.init();
  //Assign servos to their ids and frame rate
  shoulderPitchServo.begin(SHOULDER_PITCH, servoFrameMillis);   // Move servo to angle 1(0.088 degree) at speed 100
  shoulderRollServo1.begin(SHOULDER_ROLL_1, servoFrameMillis);   // Move servo to angle 1(0.088 degree) at speed 100
  shoulderRollServo2.begin(SHOULDER_ROLL_2, servoFrameMillis);
  
  shoulderPitchServo.reset();
  shoulderRollServo1.reset();
  shoulderRollServo2.reset();

  //set buttonPin to digital input
  pinMode(buttonPin, INPUT_PULLUP);
  //send message to debug serial so we know we're up and running
  mySerial.println("Dynamixel Example: ");
  mySerial.println("Move one dynamixel servo back and forth");
//  servo.setEasingFunc(easeOut);
//  servo2.setEasingFunc(easeOut);
//  servo3.setEasingFunc(easeOut);
  mySerial.print("IP:  ");
  printMyIP();
}

void loop() {

  shoulderPitchServo.update();
  shoulderRollServo1.update();
  shoulderRollServo2.update();
  
  if( shoulderPitchServo.hasArrived() || shoulderRollServo1.hasArrived() ) { 
      printAllServoPositions();
  }

  // important! non-blocking listen routine
  etherOSC.listen();  // if there is data waiting, this will trigger OSC EVENT
}

void printAllServoPositions(){
 mySerial.print("SHOULDER_PITCH: "); 
 mySerial.print(shoulderPitchServo.getCurrPos()); 
 mySerial.print("    SHOULDER_ROLL: "); 
 mySerial.println(shoulderRollServo1.getCurrPos()); 

}

/*--------------- DISPLAY IP ----------------*/
void printMyIP(){
  mySerial.print("IP:");
  mySerial.print(arduinoIP[0]);
  mySerial.print(".");
  mySerial.print(arduinoIP[1]);
  mySerial.print(".");
  mySerial.print(arduinoIP[2]);
  mySerial.print(".");
  mySerial.println(arduinoIP[3]);
}
/*-------------------------------------------*/

void singleServoMove(OscMessage &m){
  int servPos, servSpeed;
  servPos = m.getInt(0);
  servSpeed = m.getInt(1);
  shoulderPitchServo.easeTo(servPos, servSpeed);
  
  mySerial.print("ID: 1");mySerial.print("  POS: ");mySerial.print(servPos);mySerial.print("  SPD: ");mySerial.println(servSpeed);
}

void doubleServoMove(OscMessage &m){
  int servPos, servSpeed;
  servPos = m.getInt(0);
  servSpeed = m.getInt(1);
  shoulderRollServo1.easeTo(servPos, servSpeed);
  shoulderRollServo2.easeTo(servPos, servSpeed);
  
  mySerial.print("ID: 2,3");mySerial.print("  POS: ");mySerial.print(servPos);mySerial.print("  SPD: ");mySerial.println(servSpeed);

}

/*------------ OSC LISTENER -----------------*/
void oscEvent(OscMessage &m) { // *note the & before msg
  //lcd.setCursor(0,1);
  //lcd.print("Message Received");
  // receive a message 
  mySerial.println("got a message");
  // in arduino, we plug events in the EVENT method
  m.plug("/sPitch", singleServoMove); 
  m.plug("/sRoll", doubleServoMove);
}
/*-------------------------------------------*/

