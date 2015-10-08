import oscP5.*;
import netP5.Logger;
import processing.serial.*;

Serial port;
OscSerial osc;

int ledSTATE = 0;


void setup() {
  println(Serial.list());
  port = new Serial(this, Serial.list()[8], 9600);
  osc = new OscSerial(this, port);
 
  osc.plug(this,"plugTest", "/helloFromArduino");  
}

void draw() {
  background(ledSTATE*255);
}
 
void mousePressed() {
  OscMessage msg = new OscMessage("/led");
  ledSTATE = 1 - ledSTATE;
  msg.add(ledSTATE);
  osc.send(msg);  
  
}

void plugTest(int value) {
  println("Plugged from /helloFromArduino: " + value);
}

void oscEvent(OscMessage theMessage) {
  println("Message: " + theMessage + ", " + theMessage.isPlugged());  
}
