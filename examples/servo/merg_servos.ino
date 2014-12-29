/*
This example implements a servo controler.
It drives 20 servos. Can be more in arduino Mega, but it just an example.
Each servo has the parameter start,end,speed.
For each servo needs 3 bytes of parameter.
Start and end are integers in degrees from 0 to 180. and the velocity from 1 to 10: 1 slow and 10 fast. 0 indicates the servo wont be activated
Each event can control the 20 servos, therefore each event has 60 variables.
Usim FLIM mode teach on/off events.
It implements all automatic configuration, including learning events.
It does not handle short events. In phase of implementation.
It does not handle DCC messages, but you can do it on your user function.
You can change the ports to fit to your arduino.
This node uses 500 bytes of EPROM to store events and the other information.
See MemoryManagement.h for memory configuration
*/



#include <Arduino.h>
#include <SPI.h> //required by the library
#include <MergCBUS.h>
#include <Message.h>
#include <EEPROM.h> //required by the library
#include <VarSpeedServo.h>

//Module definitions
#define NUM_SERVOS 20
#define VAR_PER_SERVO 3


//CBUS definitions
#define GREEN_LED 5                       //merg green led port
#define YELLOW_LED 4                      //merg yellow led port
#define PUSH_BUTTON 3                     //std merg push button
#define PUSH_BUTTON1 9                    //debug push button
#define NODE_VARS 5                        //number o node variables
#define NODE_EVENTS 30                     //max number of events
#define EVENTS_VARS NUM_SERVOS*VAR_PER_SERVO   //number of variables per event
#define DEVICE_NUMBERS 0                   //number of device numbers. used in case of a producer module

//this will use (4+60)*30 bytes of eprom = 1920 bytes
//arduino mega has 4K, so it is ok.


//create the merg object
MergCBUS cbus=MergCBUS(NODE_VARS,NODE_EVENTS,EVENTS_VARS,DEVICE_NUMBERS);
//servo controler
VarSpeedServo servos[NUM_SERVOS];
byte servopins[]={11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30};


void setup(){
  
  pinMode(PUSH_BUTTON1,INPUT_PULLUP);//debug push button
  Serial.begin(115200);

  //Configuration data for the node
  cbus.getNodeId()->setNodeName("MODSERVO",8);  //node name
  cbus.getNodeId()->setModuleId(56);            //module number
  cbus.getNodeId()->setManufacturerId(0xA5);    //merg code
  cbus.getNodeId()->setMinCodeVersion(1);       //Version 1
  cbus.getNodeId()->setMaxCodeVersion(0);
  cbus.getNodeId()->setProducerNode(false);
  cbus.getNodeId()->setConsumerNode(true);
  cbus.setStdNN(999); //standard node number

  if (digitalRead(PUSH_BUTTON1)==LOW){
    Serial.println("Setup new memory");
    cbus.setUpNewMemory();
    cbus.setSlimMode();
    cbus.saveNodeFlags();
  }
  cbus.setLeds(GREEN_LED,YELLOW_LED);//set the led ports
  cbus.setPushButton(PUSH_BUTTON);//set the push button ports
  cbus.setDebug(false);//print some messages on the serial port
  cbus.setUserHandlerFunction(&myUserFunc);//function that implements the node logic
  cbus.initCanBus(53,CAN_125KBPS,10,200);  //initiate the transport layer. pin=53, rate=125Kbps,10 tries,200 millis between each try
  
  setUpServos();
  
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
  
  boolean onEvent;
  unsigned int converted_speed;  
  int varidx=0;
  
  byte servo_start,servo_end,servo_speed;
  if (mcbus->eventMatch()){
     onEvent=mcbus->isAccOn();
      //get the events var and control the servos
      for (int i=0;i<NUM_SERVOS;i++){
        
        servo_speed=mcbus->getEventVar(msg,varidx);
        
        if (servo_speed>0){
          converted_speed=map(servo_speed,0,10,1,255);
          servo_start=mcbus->getEventVar(msg,varidx+1);
          servo_end=mcbus->getEventVar(msg,varidx+2);
          if (onEvent){
            servos[i].write(servo_end,converted_speed);            
          }
          else{
            servos[i].write(servo_start,converted_speed);            
          }
          varidx=varidx+3;
        }        
      }
  }  
}

void setUpServos(){
  
  for (int i=0;i<NUM_SERVOS;i++){
    servos[i].attach(servopins[i]);    
  }
}

