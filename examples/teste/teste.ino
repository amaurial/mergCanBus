/*
This example implements a simple producer node in arduino Mega.
It implements all automatic configuration, including learning events.
It does not handle short events. In phase of implementation.
It does not handle DCC messages, but you can do it on your user function.
It supports 15 inputs. The signal detection works when the ports go to gnd.
The ports are defined by the parameters SENSORS=number of ports and START_PORT=initial port.
So this example takes the mega ports from 20 to 34 as input sensors.
You can change them to your arduino.
This node uses 500 bytes of EPROM to store events and the other information.
See MemoryManagement.h for memory configuration
*/



#include <Arduino.h>
#include <SPI.h>
#include <MergCBUS.h>
#include <Message.h>
#include <EEPROM.h>


#define SENSORS 8        //total sensors
#define START_PORT 2     //initial port sensor

#define GREEN_LED 22       //green led port
#define YELLOW_LED 23      //yellow led port
#define PUSH_BUTTON 21     //std merg push button
#define PUSH_BUTTON1 28    //debug push button
#define NODE_VARS 1                  //number o node variables.Servo speed
#define NODE_EVENTS 30              //max number of events
#define EVENTS_VARS 1             //number of variables per event
#define DEVICE_NUMBERS SENSORS   //number of device numbers. each servo can be a device
//input ports for sensors

MergCBUS cbus=MergCBUS(NODE_VARS,NODE_EVENTS,EVENTS_VARS,DEVICE_NUMBERS);

byte sensors[SENSORS];      //array with sensor ports
byte sensors_state[SENSORS];//last sensor state


void setup(){

  loadSensorsPorts();     //setup the sensor ports
  pinMode(PUSH_BUTTON1,INPUT_PULLUP);//debug push button
  Serial.begin(115200);

  //cbus.setUpNewMemory(); //uncomment this line to setup a new node and then comment it again
  //Configuration data for the node
  cbus.getNodeId()->setNodeName("MODTESTE",8);  //node name
  cbus.getNodeId()->setModuleId(55);            //module number
  cbus.getNodeId()->setManufacturerId(0xA5);    //merg code
  cbus.getNodeId()->setMinCodeVersion(1);       //Version 1
  cbus.getNodeId()->setMaxCodeVersion(0);
  cbus.getNodeId()->setProducerNode(true);
  cbus.getNodeId()->setConsumerNode(false);
  cbus.setStdNN(999); //standard node number

  if (digitalRead(PUSH_BUTTON1)==LOW){
    Serial.println(F("Setup new memory"));
    cbus.setUpNewMemory();
    cbus.setSlimMode();
    cbus.saveNodeFlags();
  }

  cbus.setLeds(GREEN_LED,YELLOW_LED);//set the led ports
  cbus.setPushButton(PUSH_BUTTON);//set the push button ports  
  cbus.setUserHandlerFunction(&myUserFunc);//function that implements the node logic
  cbus.initCanBus(53,CAN_125KBPS,10,200);  //initiate the transport layer

  Serial.println(F("Setup finished"));
}

void loop (){

  cbus.run();//do all logic
  if (digitalRead(PUSH_BUTTON1)==LOW){
    cbus.dumpMemory();
  }
}

void myUserFunc(Message *msg,MergCBUS *mcbus){
  //Serial.println(F("My function"));
  //check if there is something on some port and send and event
  for (int i=0;i<SENSORS;i++){
      if (digitalRead(sensors[i])==HIGH){
          if (sensors_state[i]==LOW){
              //transition. generate an off event
              sensors_state[i]=HIGH;
              msg->clear();//clear the message buffer
              //sends and long off event
              msg->createOffEvent(mcbus->getNodeId()->getNodeNumber(),
                                  true,i,0,0);
              mcbus->sendMessage(msg);
          }
      }else{
        if (sensors_state[i]==HIGH){
              //transition. generate an on event
              sensors_state[i]=LOW;
              msg->clear();//clear the message buffer
              //sends and long on event
              msg->createOnEvent(mcbus->getNodeId()->getNodeNumber(),
                                  true,i,0,0);
              mcbus->sendMessage(msg);
          }
      }
  }


}
//config the sensors port
void loadSensorsPorts(){

  for (int i=0;i<SENSORS;i++){
     sensors[i]=START_PORT+i;
     pinMode(START_PORT+i,INPUT_PULLUP);
     sensors_state[i]=HIGH;
  }
}
