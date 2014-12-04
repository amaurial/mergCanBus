/*
This example implements a simple consumer node that implements a semaphore.
It control 3 leds (green,yellow and red).
The on event makes the transition from red to green and the off event goes from green to red. Both passing throu yellow.
Usim FLIM mode teach on/off events.
It implements all automatic configuration, including learning events.
It does not handle short events. In phase of implementation.
It does not handle DCC messages, but you can do it on your user function.
You can change the ports to fit to your arduino.
This node uses 500 bytes of EPROM to store events and the other information.
See MemoryManagement.h for memory configuration
*/



#include <Arduino.h>
#include <SPI.h>
#include <MergCBUS.h>
#include <Message.h>
#include <EEPROM.h>

#define GREEN_LED 5       //merg green led port
#define YELLOW_LED 4      //merg yellow led port
#define PUSH_BUTTON 3     //std merg push button
#define PUSH_BUTTON1 9    //debug push button
//semaphore leds
#define GREEN 10        //green led
#define YELLOW 11        //green led
#define RED 12        //green led
#define TRANSITION_TIME  2000 //time in milli to keep the yellow light on

MergCBUS cbus=MergCBUS();
byte lighton;//control the state
byte nextlighton;//control the state
unsigned long starttime;

void setup(){
  
  setLedPorts();     //setup the sensor ports
  pinMode(PUSH_BUTTON1,INPUT_PULLUP);//debug push button
  Serial.begin(115200);

  //cbus.setUpNewMemory(); //uncomment this line to setup a new node and then comment it again
  //Configuration data for the node
  cbus.getNodeId()->setNodeName("MODTESTE",8);  //node name
  cbus.getNodeId()->setModuleId(55);            //module number
  cbus.getNodeId()->setManufacturerId(0xA5);    //merg code
  cbus.getNodeId()->setMinCodeVersion(1);       //Version 1
  cbus.getNodeId()->setMaxCodeVersion(0);
  cbus.getNodeId()->setSuportedEvents(20);      //max supported events. this can be changed in the memory config
  cbus.getNodeId()->setSuportedEventsVariables(8);
  cbus.getNodeId()->setSuportedNodeVariables(20);
  cbus.getNodeId()->setProducerNode(true);
  cbus.getNodeId()->setConsumerNode(false);
  cbus.setStdNN(999); //standard node number

  cbus.setLeds(GREEN_LED,YELLOW_LED);//set the led ports
  cbus.setPushButton(PUSH_BUTTON);//set the push button ports
  cbus.setDebug(true);//print some messages on the serial port
  cbus.setUserHandlerFunction(&myUserFunc);//function that implements the node logic
  cbus.initCanBus(53,CAN_125KBPS,10,200);  //initiate the transport layer

  Serial.println("Setup finished");
}

void loop (){

  cbus.run();//do all logic
  //debug memory
  if (digitalRead(PUSH_BUTTON1)==LOW){
    cbus.dumpMemory();
  }
}

void myUserFunc(Message *msg,MergCBUS *mcbus){
  
  //getting standard on/off events
  if (mcbus->eventMatch()){
      lighton=YELLOW;
      turnLightsOn();
      starttime=millis();
      
      if (mcbus->isAccOn()){
        nextlighton=GREEN;      
      }
      else {
        nextlighton=RED;
      }      
  }    
  if (lighton==YELLOW){
    if ((millis()-starttime)>TRANSITION_TIME){
      lighton=nextlighton;
    }
  }    
  turnLightsOn();  
}
//config the leds ports and the initial state
void setLedPorts(){

  pinMode(GREEN,OUTPUT);
  pinMode(YELLOW,OUTPUT);
  pinMode(RED,OUTPUT);  
  lighton=GREEN;
  turnLightsOn();
}

void turnLightsOn(){
  switch (lighton){
    case (GREEN):
      digitalWrite(GREEN,HIGH);
      digitalWrite(RED,LOW);
      digitalWrite(YELLOW,LOW);
    break;
    case (RED):
      digitalWrite(RED,HIGH);
      digitalWrite(GREEN,LOW);
      digitalWrite(YELLOW,LOW);
    break;
    case (YELLOW):
      digitalWrite(YELLOW,HIGH);
      digitalWrite(RED,LOW);
      digitalWrite(GREEN,LOW);
    break;
  }  
}
