/*
J.Teda 21/04/2013

Exsample of how to run Dynamixel in SERVO mode - tell servo what angle to move to.

Robotis e-Manual ( http://support.robotis.com )
 
*/

#include <Dynamixel_Serial.h>       // Library needed to control Dynamixal servo
#include <SoftwareSerial.h>         // Library needed for software serial

#define SERVO_ID_1 0x01
#define SERVO_ID_2 0x02               // ID of which we will set Dynamixel too 
#define SERVO_ID_3 0x03
#define SERVO_ControlPin 0x02       // Control pin of buffer chip, NOTE: this does not matter becasue we are not using a half to full contorl buffer.
#define SERVO_SET_Baudrate 1000000    // Baud rate speed which the Dynamixel will be set too (57600)
#define CW_LIMIT_ANGLE 0x001        // lowest clockwise angle is 1, as when set to 0 it set servo to wheel mode
#define CCW_LIMIT_ANGLE 0xFFF       // Highest anit-clockwise angle is 0XFFF, as when set to 0 it set servo to wheel mode


#define SLOWEST 10
SoftwareSerial mySerial(10, 11);    // RX, TX for software serial

float decelSpeed[5] = { 0.95, 0.85, 0.75, 0.55, 0.3 };
float decelPos[5] = { 0.8, 0.9, 0.95, 0.98, 0.99 };

void setup(){
 mySerial.begin(57600);                                     // Set up Software Serial

 delay(1000);                                                           // Give time for Dynamixel to start on power-up

Dynamixel.begin(SERVO_SET_Baudrate, SERVO_ControlPin);        			// We now need to set Ardiuno to the new Baudrate speed 
Dynamixel.setMode(SERVO_ID_1, SERVO, CW_LIMIT_ANGLE, CCW_LIMIT_ANGLE);    // set mode to SERVO and set angle limits
Dynamixel.setMode(SERVO_ID_2, SERVO, CW_LIMIT_ANGLE, CCW_LIMIT_ANGLE);    // set mode to SERVO and set angle limits
Dynamixel.setMode(SERVO_ID_3, SERVO, CW_LIMIT_ANGLE, CCW_LIMIT_ANGLE);    // set mode to SERVO and set angle limits


}


void loop(){

  //Move to 0 position in a linear fashion
  Dynamixel.servo(SERVO_ID_1,10,100);   // Move servo to angle 1(0.088 degree) at speed 100
  Dynamixel.servo(SERVO_ID_2,10,100);   // Move servo to angle 1(0.088 degree) at speed 100
  Dynamixel.servo(SERVO_ID_3,10,100);
  
  
  // While Moving print Servo Position to software serial
  while(Dynamixel.checkMovement(SERVO_ID_1) ==  1){
    printAllServoPositions();
  }
  
  // Arbitrary Delay
  delay(2000);
  
  // First brute force method for deceleration.
  // sevorDeceleration( targetPosition, Speed );
  // TO DO: Write a better function!  Currently can not go counter clockwise.  
  // JUST FOR TESTING!!!!
  servoDeceleration(2047, 200);
    
  delay(2000);
  
}

void printAllServoPositions(){
 mySerial.print("SERVO 1: "); 
 mySerial.print(Dynamixel.readPosition(SERVO_ID_1)); 
 mySerial.print("    SERVO 2: "); 
 mySerial.print(Dynamixel.readPosition(SERVO_ID_2)); 
 mySerial.print("    SERVO 3: ");
 mySerial.println(Dynamixel.readPosition(SERVO_ID_3)); 

}

void servoDeceleration(int pos, float servoSpeed){
  
  Dynamixel.servo(SERVO_ID_1, pos, (int)servoSpeed);
  Dynamixel.servo(SERVO_ID_2, pos, (int)servoSpeed);
  Dynamixel.servo(SERVO_ID_3, pos, (int)servoSpeed);
  
  while(Dynamixel.checkMovement(SERVO_ID_1) ==  1){
      if(servoSpeed <= SLOWEST){
          servoSpeed = SLOWEST;
      }
      
      Dynamixel.servo(SERVO_ID_1, pos, (int)servoSpeed);
      Dynamixel.servo(SERVO_ID_2, pos, (int)servoSpeed);
      Dynamixel.servo(SERVO_ID_3, pos, (int)servoSpeed);
      
      if(Dynamixel.readPosition(SERVO_ID_3) > (int)(pos * decelPos[0])){
        servoSpeed *= decelSpeed[0];
      }
      if(Dynamixel.readPosition(SERVO_ID_3) > (int)(pos * decelPos[1])){
        servoSpeed *= decelSpeed[1];
      }
      if(Dynamixel.readPosition(SERVO_ID_3) > (int)(pos * decelPos[2])){
        servoSpeed *= decelSpeed[2];
      }
      if(Dynamixel.readPosition(SERVO_ID_3) > (int)(pos * decelPos[3])){
        servoSpeed *= decelSpeed[3];
      }
      if(Dynamixel.readPosition(SERVO_ID_3) > (int)(pos * decelPos[4])){
        servoSpeed *= decelSpeed[4];
      }
      printAllServoPositions();
      mySerial.print("Moving: ");
      mySerial.println(Dynamixel.checkMovement(SERVO_ID_1));
    }
  
}
