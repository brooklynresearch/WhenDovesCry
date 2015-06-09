//
// ServoEaser.cpp -- Arduino library for Servo easing
//
// Uses concepts from:
// -- http://portfolio.tobiastoft.dk/331886/Easing-library-for-Arduino
// -- http://jesusgollonet.com/processing/pennerEasing/
// -- http://robertpenner.com/easing/
// -- http://robertpenner.com/easing/easing_demo.html
//
// 2011, TeamPneumo, Tod E. Kurt, http://todbot.com/blog/
// 
//

#include "DynamixelEaser.h"
#include <math.h>

#define DEBUG 0
#define SERVO_ControlPin 0x02       // Control pin of buffer chip, NOTE: this does not matter becasue we are not using a half to full contorl buffer.
#define SERVO_SET_Baudrate 1000000    // Baud rate speed which the Dynamixel will be set too (57600)
#define CW_LIMIT_ANGLE 0x001        // lowest clockwise angle is 1, as when set to 0 it set servo to wheel mode
#define CCW_LIMIT_ANGLE 0xFFF       // Highest anit-clockwise angle is 0XFFF, as when set to 0 it set servo to wheel mode

// FIXME: this debug setup sucks
#if (DEBUG > 0)
#define debug_update() { \
        Serial.print("update: currPos:"); Serial.print(currPos);        \
        Serial.print(" startPos:"); Serial.print(startPos);             \
        Serial.print(" changePos:"); Serial.print(changePos);           \
        Serial.print(" tick:"); Serial.print(tick);                     \
        Serial.print(" arrived:");Serial.println(((arrived)?"true":"false"));}
#define debug_getnextpos() { \
        Serial.print("getNextPos:"); Serial.print(movesIndex); \
        Serial.print(" changePos:"); Serial.print(changePos);     \
        Serial.print(" moves[i].pos:"); Serial.print(moves[movesIndex].pos); \
        Serial.print(" durMillis:"); Serial.print(durMillis); \
        Serial.print(" tickCount:"); Serial.println(tickCount); }
#define debug_reset() { \
        Serial.print("reset: currPos:"); Serial.print(currPos); \
        Serial.print(" changePos:"); Serial.print(changePos); \
        Serial.print(" durMillis:"); Serial.print(durMillis); \
        Serial.print(" movesIndex:"); Serial.print(movesIndex); \
        Serial.print(" tickCount:"); Serial.println(tickCount); }
#else
#define debug_update() {}
#define debug_getnextpos() {}
#define debug_reset() {}
#endif

// default easing function
// this is from Easing::easeInOutCubic()
// t: current time, b: beginning value, c: change in value, d: duration
// t and d can be in frames or seconds/milliseconds
inline float DynamixelEaser_easeInOutCubic(float t, float b, float c, float d)
{
    if ((t/=d/2) < 1) return c/2*t*t*t + b;
	return c/2*((t-=2)*t*t + 2) + b;
}
// set up the Dynamixels Baud rate and control pin.
void DynamixelEaser::init()
{
    Dynamixel.begin(SERVO_SET_Baudrate, SERVO_ControlPin);
}
void DynamixelEaser::init(int baud)
{
    Dynamixel.begin(baud, SERVO_ControlPin);
}
void DynamixelEaser::init(int baud, int controlPin)
{
    Dynamixel.begin(baud, controlPin);
}
// set up an easer with a servo and a moves list
void DynamixelEaser::begin(int servoID, int frameTime, 
                       ServoMove* mlist, int mcount)
{
    begin(servoID, frameTime ); //, servo.read() );
    play( mlist, mcount );
}

// set up an easer with just a servo and a starting position
void DynamixelEaser::begin(int servoID, int frameTime)
{
    Dynamixel.setMode(servoID, SERVO, CW_LIMIT_ANGLE, CCW_LIMIT_ANGLE);
    servo = servoID;
    frameMillis = frameTime;

    flipped = false;
    arrived = true;

    easingFunc = DynamixelEaser_easeInOutCubic;
    arrivedFunc = NULL;

    useMicros = false;

    movesIndex = 0;
    
    reset();

}

// reset easer to initial conditions, does not nuke easingFunc or arrivedFunc
void DynamixelEaser::reset()
{
    currPos = Dynamixel.readPosition(servo);
    startPos = currPos;  // get everyone in sync
    changePos = 0;       // might be overwritten below

    if( movesCount > 0 ) {
        changePos = moves[ movesIndex ].pos - startPos ;
        durMillis = moves[ movesIndex ].dur;
    }

    tickCount = durMillis / frameMillis;
    tick = 0;
    
    debug_reset();
}

//
void DynamixelEaser::play( ServoMove* mlist, int mcount)
{
    play( mlist, mcount, 0, 0);
}
//
void DynamixelEaser::play( ServoMove* mlist, int mcount, int mreps )
{
    play( mlist, mcount, mreps, 0 );
}

void DynamixelEaser::play( ServoMove* mlist, int mcount, int mreps, int mindex)
{
    moves = mlist;
    movesCount = (mcount>0) ? mcount : 0;
    movesReps  = (mreps>0)  ? mreps  : 0;
    movesIndex = (mindex>0) ? mindex : 0;
    //movesIndex = (mindex>0) ? ((mindex<mcount) ? mindex : mcount) : 0; 

    running = true;
    arrived = false;

    reset();
}

// manual, non-moves list, control of easer position
void DynamixelEaser::easeTo( int pos, int dur )
{
    movesCount = 0;  // no longer doing moves list
    startPos = currPos;
    changePos = pos - startPos;
    durMillis = dur;
    tickCount = durMillis / frameMillis;
    tick = 0;
    arrived = false;
    running = true;
}

// used internally to select next servo position
void DynamixelEaser::getNextPos()
{
    arrived = true;
    if( arrivedFunc != NULL ) { arrivedFunc( currPos, movesIndex ); }

    if( movesCount==0 ) { // not in a moves list
        return;
    }

    movesIndex++;
    if( movesIndex == movesCount ) {
        movesIndex = 0;  // loop
        movesReps--;
        if( movesReps == 0 ) { // we are done
            stop();
        } else if( movesReps == -1 ) { // we are infinite
            movesReps = 0;
        }
    }
    startPos  = currPos; // current position becomes new start position

    changePos = moves[ movesIndex ].pos - startPos ;
    durMillis = moves[ movesIndex ].dur;

    tickCount = durMillis / frameMillis;
    tick = 0;
    arrived = false;

    debug_getnextpos();
}

// call this regularly in loop()
void DynamixelEaser::update()
{
    if( ((millis() - lastMillis) < frameMillis) || !running ) {  // time yet?
        return;
    }
    lastMillis = millis();

    currPos = easingFunc( tick, startPos, changePos, tickCount );
    
    debug_update();

    if( !arrived ) tick++;
    if( tick == tickCount ) { // time for new position
        getNextPos(); 
    }
    float p = (flipped) ? 2047.0 - currPos : currPos;

    Dynamixel.servo(servo, (int)p, 800);
}

//
void DynamixelEaser::setMinMaxMicroseconds(int mi, int ma)
{
    min = mi;
    max = ma;
    useMicros = true;
}

// from Servo.cpp
//#define SERVO_MIN() (MIN_PULSE_WIDTH - this->min * 4)  // min value in uS 
//#define SERVO_MAX() (MAX_PULSE_WIDTH - this->max * 4)  // max value in uS

//#define in_min 0
//#define in_max 180
//#define out_min SERVO_MIN()
//#define out_max SERVO_MAX()
/*
int DynamixelEaser::angleToMicros(float angle)
{
    //                 x   in_min,in_max, out_min, out_max
    //int value = map(value, 0, 180, SERVO_MIN(),  SERVO_MAX());
    return (angle-in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
*/

//
void DynamixelEaser::useMicroseconds(boolean t)
{
    useMicros = t;
}
//
boolean DynamixelEaser::usingMicroseconds()
{
    return useMicros;
}
void DynamixelEaser::setFlipped(boolean t)
{
    flipped = t;
}
boolean DynamixelEaser::isFlipped()
{
    return flipped;
}
//
void DynamixelEaser::start()
{
    running = true;
}
//
void DynamixelEaser::stop()
{
    running = false;
}
//
boolean DynamixelEaser::isRunning()
{ 
    return running;
}
//
boolean DynamixelEaser::hasArrived()
{ 
    return arrived;
}

float DynamixelEaser::getCurrPos()
{
    return currPos;
}

// 
void DynamixelEaser::setEasingFunc( EasingFunc func )
{
    easingFunc = func;
}
//
void DynamixelEaser::setArrivedFunc( ArrivedFunc func ) 
{
    arrivedFunc = func;
}

//
/*
void DynamixelEaser::setMovesList( ServoMove* mlist, int mcount )
{
    moves = mlist;
    movesCount = mcount;
    reset();
}
*/