# ToniDove
Robotic Control and Communication.  

## OSC Messaging
The OSC messaging protocol to control the servos.
The current two messages to control the shoulder of the robot arm are `/sPitch` and `/sRoll`.  The Pitch message controlling the base rotation (single servo), and the Roll message controlling the swing angle (two servos).  Each message requires two integer arguments.  
### Arguments
`servoPosition` which has a range of 0 - 2047
 `servoSpeed` which has a range of 1 - inf. servoSpeed is roughly in milliseconds and the value given is the amount of time you gave to go from currentPosition to the previously supplied servePosition
### OSC Message Orientation
```{r, engine='bash', count_lines}
/sPitch servoPosition servoSpeed
/sRoll servoPosition servoSpeed
```
### OSC Example
```{r, engine='bash', count_lines}
/sPitch 512 18
/sRoll 1024 100
```
