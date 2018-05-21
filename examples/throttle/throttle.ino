/*
This example implements a simple Throtle controled by the serial port.
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
#include <MergCBUSThrottle.h>
#include <EEPROM.h>

#define GREEN_LED      8    //merg green led port
#define YELLOW_LED     7    //merg yellow led port
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

//throttle state
#define START 0
#define ACQUIRED 1
#define WAITING 2
#define SENT_SPEED 3
#define RUNNING 4
#define STOPPED 5
#define LOCO_RUN_TIME 10 //seconds

//keep alive interval
#define KEEP_ALIVE_TIMEOUT 2000//millisecs

MergCBUS cbus=MergCBUS(NUM_NODE_VARS,NUM_EVENTS,NUM_EVENT_VARS,NUM_DEVICES);
MergCBUSThrottle throttle=MergCBUSThrottle(&cbus);

unsigned long starttime;
#define MAX_SESSIONS 32
int sessions[MAX_SESSIONS];
int state;
int previous_state;
long last_time;
long stoptime;

void setup(){

  //pinMode(PUSH_BUTTON1,INPUT_PULLUP);//debug push button
  Serial.begin(115200);


  //Configuration data for the node
  cbus.getNodeId()->setNodeName("CAB",8);         //node name
  cbus.getNodeId()->setModuleId(MODULE_ID);            //module number
  cbus.getNodeId()->setManufacturerId(MANUFACTURER_ID);//merg code
  cbus.getNodeId()->setMinCodeVersion(MIN_CODE);       //Version 1
  cbus.getNodeId()->setMaxCodeVersion(MAX_CODE);
  cbus.getNodeId()->setProducerNode(true);
  cbus.getNodeId()->setConsumerNode(true);
  cbus.setPushButton(PUSH_BUTTON);//set the push button ports

  //used to manually reset the node. while turning on keep the button pressed
  //this forces the node for slim mode with an empty memory for learned events and devices
  if (digitalRead(PUSH_BUTTON)==LOW){
    Serial.println(F("Setup new memory"));
    cbus.setUpNewMemory();
    cbus.saveNodeFlags();
  }

  cbus.setLeds(GREEN_LED,YELLOW_LED);//set the led ports

  cbus.setUserHandlerFunction(&myUserFunc);//function that implements the node logic
  cbus.setDCCHandlerFunction(&myUserFuncDCC);
  cbus.initCanBus(CANPORT);  //initiate the transport layer
  cbus.setFlimMode();
  state = START;
  previous_state = START;
  cbus.doSelfEnnumeration(true);
  throttle.setKeepAliveInterval(KEEP_ALIVE_TIMEOUT);
  Serial.println(F("Setup finished"));
}

void loop (){
  cbus.cbusRead();
  cbus.run();//do all logic
  throttle.run();
  main_throttle(NULL);
  //sendKeepAlive();
}

void myUserFunc(Message *msg,MergCBUS *mcbus){
  Serial.print(F("MSG Code: "));
  Serial.println(msg->getOpc());
}

void myUserFuncDCC(Message *msg,MergCBUS *mcbus){
  Serial.print(F("DCC Code: "));
  Serial.println(msg->getOpc());
  main_throttle(msg);
}

void main_throttle(Message *msg){

  if (cbus.isSelfEnumMode()){
    Serial.println(F("Node in self enum"));
    return;
  }
  switch (state){
   //acquire on loco
    case START:
      Serial.println(F("Request session"));
      throttle.getSession(2);
      state = WAITING;
    break;
    case WAITING:
      //Serial.println(F("Process waiting"));
      switch (previous_state){
        case START:
          //check if we get a session
          //Serial.println(F("check if we get a session"));
          if (msg != NULL){            
            if (msg->getSession() > 0){
              Serial.println(F("got session"));
              Serial.println(msg->getSession());
              previous_state = WAITING;
              sessions[0] = msg->getSession();
              throttle.setSession(sessions[0],2);
              throttle.setSpeedMode(sessions[0]);
              state = ACQUIRED;
              delay(1000);
              
            }
          }
        break;
        case SENT_SPEED:
          Serial.println(F("stopping"));
          stoptime = millis();
          state = RUNNING;
        break;

      }
    break;

    case ACQUIRED:
      Serial.println(F("Setting speed"));
      cbus.sendSpeedDir(sessions[0],100,true);
      previous_state = SENT_SPEED;
      state = WAITING;
    break;

    case RUNNING:
      Serial.println(F("Running"));
      if ((millis() - stoptime)  > LOCO_RUN_TIME*1000) {
        cbus.sendSpeedDir(sessions[0],0,true);
        state = STOPPED;
        previous_state = RUNNING;
      }
      
    break;
    case STOPPED:
      //release
      Serial.println(F("Releasing"));
      cbus.sendReleaseSession(sessions[0]);
      delay(5000);
      state = START;
      previous_state = START;
    break;
  }


   //set the speed

   //set the function

   //wait a little and query the speed

   //wait a little and change the speed

   //wait a little and release the loco

   //wait a little and get it again

}

void sendKeepAlive(){
  if (millis() - last_time >= KEEP_ALIVE_TIMEOUT){
    if (sessions[0] > 0){
        Serial.println(F("sending keep alive"));
        cbus.sendKeepAliveSession(sessions[0]);
    }
    last_time = millis();
  }
}


