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
#define NUM_SERVOS 8      //number of servos
#define VAR_PER_SERVO 20  //variables per servo
#define SPEED 50


//CBUS definitions
#define GREEN_LED 5                       //merg green led port
#define YELLOW_LED 4                      //merg yellow led port
#define PUSH_BUTTON 3                     //std merg push button
#define PUSH_BUTTON1 9                    //debug push button
//number o node variables. first 2 are to indicate which servo is on. 2 bytes to indicate to togle. 1 for standard speed
#define NODE_VARS 5                        
#define NODE_EVENTS 30                     //max number of events
#define EVENTS_VARS VAR_PER_SERVO   //number of variables per event
#define DEVICE_NUMBERS 0                   //number of device numbers. used in case of a producer module
#define START_ANGLE_VAR 5
#define END_ANGLE_VAR 6

//arduino mega has 4K, so it is ok.


//create the merg object
MergCBUS cbus=MergCBUS(NODE_VARS,NODE_EVENTS,EVENTS_VARS,DEVICE_NUMBERS);
//servo controler
VarSpeedServo servos[NUM_SERVOS];
//pins where the servos are attached
byte servopins[]={11,12,13,14,15,16,17,18};//,19,20,21,22,23,24,25,26,27,28,29,30};
byte active_servo[2];
byte togle_servo[2];



void setup(){
  
  pinMode(PUSH_BUTTON1,INPUT_PULLUP);//debug push button
  Serial.begin(115200);

  //Configuration data for the node
  cbus.getNodeId()->setNodeName("MODSERV",7);  //node name
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
  cbus.setDebug(true);//print some messages on the serial port
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
     getServosArray(msg,mcbus);
      //get the events var and control the servos
      for (int i=0;i<NUM_SERVOS;i++){        
        
        if (isServoActive(i)){          
          servo_start=mcbus->getEventVar(msg,START_ANGLE_VAR);
          servo_end=mcbus->getEventVar(msg,END_ANGLE_VAR);
          if (onEvent){
            if (isServoToTogle(i)){              
              servos[i].write(servo_start,SPEED);       
            }  
            else {
              servos[i].write(servo_end,SPEED);
            }   
          }
          else{
            if (isServoToTogle(i)){              
              servos[i].write(servo_end,SPEED);       
            }  
            else {
              servos[i].write(servo_start,SPEED);
            }              
          }          
        }        
      }
  }
  else{
    //feedback messages
    
    //learn device number    
    
  } 
}
//create the objects for each servo
void setUpServos(){  
  for (int i=0;i<NUM_SERVOS;i++){
    servos[i].attach(servopins[i]); 
   servos[i].write(0,127); 
  }
}
//is the servo to be activated or not
boolean isServoActive(int index){  
  return checkBit(active_servo,index);
}
//is the servo by index to togle or not
boolean isServoToTogle(int index){
  return checkBit(togle_servo,index);
}

//check the if a bit is set or not
boolean checkBit(byte *array,int index){
  byte a,i;
  if (index<8){
    a=array[0];
    i=index;
  }
  else{
    a=array[1];    
    i=index-8;
  }
  
  if (bitRead(a,i)==1){
    return true;
  }
  else{
     return false;
  }  
}
//get the events vars for activate servos and to togle the servos:invert behaviour on on/off events
void getServosArray(Message *msg,MergCBUS *mcbus){
  active_servo[0]=mcbus->getEventVar(msg,1);
  active_servo[1]=mcbus->getEventVar(msg,2);
  togle_servo[0]=mcbus->getEventVar(msg,3);
  togle_servo[1]=mcbus->getEventVar(msg,4);
}

