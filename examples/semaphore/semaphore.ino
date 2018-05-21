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

#define GREEN_LED      A6    //merg green led port
#define YELLOW_LED     A7   //merg yellow led port
#define PUSH_BUTTON    9    //std merg push button
#define NUM_NODE_VARS  1    //the transition interval
#define NUM_EVENTS     20   //supported events
#define NUM_EVENT_VARS 0    //no need for supported event variables
#define NUM_DEVICES    1    //one device number
#define MODULE_ID      55   //module id
#define MANUFACTURER_ID 165 //manufacturer id
#define MIN_CODE       0    //min code version
#define MAX_CODE       1    //max code version

#define CANPORT        10   //attached mcp2551 pin. if using mega pin is 53

//semaphore leds
#define GREEN 10              //green led pin
#define YELLOW 11             //yellow led pin
#define RED 12                //red led pin
#define TRANSITION_TIME  50   //time in milli to keep the yellow light on
#define NNVar1           40   //the yellow light will stay on for 2 seconds

MergCBUS cbus=MergCBUS(NUM_NODE_VARS,NUM_EVENTS,NUM_EVENT_VARS,NUM_DEVICES);

byte lighton;//control the state
byte nextlighton;//control the state
unsigned long starttime;


void setup(){

  setLedPorts();     //setup the sensor ports
  //pinMode(PUSH_BUTTON1,INPUT_PULLUP);//debug push button
  Serial.begin(115200);


  //Configuration data for the node
  cbus.getNodeId()->setNodeName("SEMAPHOR",8);         //node name
  cbus.getNodeId()->setModuleId(MODULE_ID);            //module number
  cbus.getNodeId()->setManufacturerId(MANUFACTURER_ID);//merg code
  cbus.getNodeId()->setMinCodeVersion(MIN_CODE);       //Version 1
  cbus.getNodeId()->setMaxCodeVersion(MAX_CODE);
  cbus.getNodeId()->setProducerNode(false);
  cbus.getNodeId()->setConsumerNode(true);
  cbus.setPushButton(PUSH_BUTTON);//set the push button ports

  //used to manually reset the node. while turning on keep the button pressed
  //this forces the node for slim mode with an empty memory for learned events and devices
  if (digitalRead(PUSH_BUTTON)==LOW){
    Serial.println(F("Setup new memory"));
    cbus.setUpNewMemory();
    cbus.saveNodeFlags();
    //set the default value
    cbus.setNodeVariable(1,NNVar1);
  }

  cbus.setLeds(GREEN_LED,YELLOW_LED);//set the led ports

  cbus.setUserHandlerFunction(&myUserFunc);//function that implements the node logic
  cbus.initCanBus(CANPORT);  //initiate the transport layer

  Serial.println(F("Setup finished"));
}

void loop (){
  cbus.cbusRead();
  cbus.run();//do all logic
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
    byte interval=cbus.getNodeVar(1);
    if (interval<=0){
        interval=1;
    }
    if ((millis()-starttime)>(TRANSITION_TIME*interval)){
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
