/*
 * oscP5sendreceive by andreas schlegel
 * example shows how to send and receive osc messages.
 * oscP5 website at http://www.sojamo.de/oscP5
 */
 
 // YOU WILL NEED TO CONNECT YOUR LAPTOP VIA ETHERNET TO RECEIVE IN PROCESSING

 
import oscP5.*;
import netP5.*;
  
OscP5 oscP5;

// listening
int iNet_myListeningPort  = 12000;  // port I am listening on 

// speaking
NetAddress  iNet_DestinationAddress;  // someone to talk to
String      iNet_DestinationIP = "141.117.45.176";
int         iNet_DestinationPort = 12001;   


int ledState = 0;

void setup() {
  size(400,400);
  frameRate(25);
  /* start oscP5, listening for incoming messages at port 12000 */
  oscP5 = new OscP5(this,iNet_myListeningPort);
  
  /* myRemoteLocation is a NetAddress. a NetAddress takes 2 parameters,
   * an ip address and a port number. myRemoteLocation is used as parameter in
   * oscP5.send() when sending osc packets to another computer, device, 
   * application. usage see below. for testing purposes the listening port
   * and the port of the remote location address are the same, hence you will
   * send messages back to this sketch.
   */
  iNet_DestinationAddress = new NetAddress( iNet_DestinationIP, iNet_DestinationPort );
}


void draw() {
  background(ledState *255 );  
}

void mousePressed() {
  /* in the following different ways of creating osc messages are shown by example */
  OscMessage myMessage = new OscMessage("/led");
  ledState = 1 - ledState;
  myMessage.add(ledState); /* add an int to the osc message */

  /* send the message */
  oscP5.send(myMessage, iNet_DestinationAddress); 
}


/* incoming osc message are forwarded to the oscEvent method. */
void oscEvent(OscMessage theOscMessage) {
  /* print the address pattern and the typetag of the received OscMessage */
  print("### received an osc message.");
  print(" addrpattern: "+theOscMessage.addrPattern());
  println(" typetag: "+theOscMessage.typetag());
  
}


