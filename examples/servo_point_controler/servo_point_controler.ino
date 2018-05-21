/*
This example implements a servo controler.
It drives 8 servos. Can be more in arduino Mega, but it just an example.
Each event has the parameter start,end.
Start and end are integers in degrees from 0 to 180. The velocity is set by node parameter
Each event can control the 8 servos (or more), therefore each event has 6 variables. 16 bits to define which servo is activated and 16 bits to define if the
servo will togle upon an event. invert the behaviour and 2 bytes to define the start and end position.
Usim FLIM mode teach on/off events.
It implements all automatic configuration, including learning events.
It does not handle DCC messages, but you can do it on your user function.
You can change the ports to fit to your arduino.
This node uses 500 bytes of EPROM to store events and the other information.
See MemoryManagement.h for memory configuration
To clear the memory, press pushbutton1 while reseting the arduino

*/


#include <Arduino.h>
#include <SPI.h> //required by the library
//#include <TimerOne.h>
#include <MergCBUS.h>
#include <Message.h>
#include <EEPROM.h> //required by the library
#include <VarSpeedServo.h>

/* uncomment this line to see debug messages */
//#define DEBUGNODE 1

/* Module definitions */

#define NUM_SERVOS 13      //number of servos

/* first 2 are to indicate which servo is on. 2 bytes to indicate to togle. 2 for start and end angle */

#define VAR_PER_SERVO 6  //variables per servo
#define SPEED 50        //servo speed
#define SERVO_START 15        //servo start angle.start at 15 to avoid jitter
#define SERVO_END 180        //servo end angle


/* CBUS definitions */
#define GREEN_LED A0                  //merg green led port
#define YELLOW_LED 0                 //merg yellow led port
#define PUSH_BUTTON 9                //std merg push button

/* 
 * Node definition
 * number o node variables
 */
 
#define NODE_VARS 2 

/*                 
* var1 = Servo speed
* var2 = flags
* flag1 (2 first bits) action to take on starting. see setupServos
* flag2 (bit 3), indicate to maintain the servo attached after moving. default is 0 = detached
*/

#define SERVO_SPEED_VAR  1
#define SERVO_FLAGS_VAR  2 

#define NODE_EVENTS 30              //max number of events
#define EVENTS_VARS VAR_PER_SERVO   //number of variables per event
#define DEVICE_NUMBERS NUM_SERVOS   //number of device numbers. each servo can be a device

/* 
 * the indexes start at 1 not at 0  
 */
 
#define ACTIVE_SERVO_VAR 1          //2 bytes for that
#define TOGLE_SERVO_VAR  3          //2 bytes for that
#define START_ANGLE_VAR  5          //var index for the start angle
#define END_ANGLE_VAR    6          //var index for the end angle


/* create the merg object */
MergCBUS cbus=MergCBUS(NODE_VARS,NODE_EVENTS,EVENTS_VARS,DEVICE_NUMBERS);

/* servo controler */
VarSpeedServo servos[NUM_SERVOS];

 /* 
 * pins where the servos are attached
 * pins 9,10 and 15 don't work for many servos. servo library limitation 
 */
byte servopins[]={2,3,4,5,6,7,8,1,A1,A2,A3,A4,A5};//,19,20,21,22,23,24,25,26,27,28,29,30};
/*
* Store the active servo var
* example: 3 1 means pin D2, D3 and A1 active
* example: 16 2 meand pin D6 and A2
* the same logic for toggle servo
*/
byte active_servo[2]; //store the active servo event var
byte togle_servo[2]; //store the togle servo event var
boolean attach_servo = false;
byte start_action = 3; //move to last position
long detachservos_time = 0; //control the time to detach the servo

void setup(){

  /* create the servos object */
  initializeFlags();
  setupServos();  

  pinMode(PUSH_BUTTON,INPUT);//debug push button
  #ifdef DEBUGNODE
  Serial.begin(115200);
  delay(300);
  Serial.println(F("start"));
  #else
  Serial.end();
  #endif

  /* Configuration data for the node */
  cbus.getNodeId()->setNodeName("MODSERV",7);  //node name
  cbus.getNodeId()->setModuleId(56);            //module number
  cbus.getNodeId()->setManufacturerId(0xA5);    //merg code
  cbus.getNodeId()->setMinCodeVersion(1);       //Version 1
  cbus.getNodeId()->setMaxCodeVersion(0);
  cbus.getNodeId()->setProducerNode(false);
  cbus.getNodeId()->setConsumerNode(true);
  cbus.setStdNN(999); //standard node number

  if (digitalRead(PUSH_BUTTON)==LOW){
    //Serial.println(F("Setup new memory"));
    cbus.setUpNewMemory();
    cbus.setSlimMode();
    cbus.saveNodeFlags();
  }
  cbus.setLeds(GREEN_LED,YELLOW_LED);//set the led ports
  cbus.setPushButton(PUSH_BUTTON);//set the push button ports
  cbus.setUserHandlerFunction(&myUserFunc);//function that implements the node logic
  cbus.initCanBus(10,MCP_8MHz);  //initiate the transport layer. pin=53, rate=125Kbps,10 tries,200 millis between each try
    
}

void loop (){
  cbus.cbusRead();
  cbus.run();//do all logic
  //debug memory
  if (digitalRead(PUSH_BUTTON)==LOW){
    cbus.dumpMemory();    
  }
  if (detachservos_time > 0 && detachservos_time < millis()){
      detachServos();
      detachservos_time = 0;
  }
}

/* user defined function. contains the module logic.called every time run() is called.*/
void myUserFunc(Message *msg,MergCBUS *mcbus){
  /* getting standard on/off events */

  boolean onEvent;
  unsigned int converted_speed;
  uint8_t varidx=0;  

  byte servo_start,servo_end;

  #ifdef DEBUGNODE
  Serial.println(F("got a message"));  
  #endif
  
  if (mcbus->eventMatch()){
     initializeFlags();
     onEvent=mcbus->isAccOn();
     getServosArray(msg,mcbus);
         
     servo_start=mcbus->getEventVar(msg,START_ANGLE_VAR);
     servo_end=mcbus->getEventVar(msg,END_ANGLE_VAR);
     
      /* get the events var and control the servos */
      for (uint8_t i=0;i<NUM_SERVOS;i++){
        if (isServoActive(i)){          
          moveServo(onEvent,i,servo_start,servo_end);
          delay(200);
        }
      }
      /* set time to detach the servos. detach after 2 seconds */
      detachservos_time=millis() + 2000;
  }  
}


void moveServo(boolean event,byte servoidx,byte servo_start,byte servo_end){
    byte lastPos;    
    if (event){
      if (isServoToTogle(servoidx)){        
        lastPos = servo_start;
      }
      else {
        lastPos = servo_end;
      }
    }
    else{
      if (isServoToTogle(servoidx)){        
	      lastPos = servo_end;
      }
      else {        
        lastPos = servo_start;
      }
    }
    /* 
     * write last pos to eprom
     * variables start with number 1        
     */
    cbus.setInternalNodeVariable(servoidx+1,lastPos);

    servos[servoidx].write(lastPos,SPEED);
    /* servos already attached */
    //if (!attach_servo){
	  servos[servoidx].attach(servopins[servoidx]);
    //}
    servos[servoidx].write(lastPos,SPEED);
   
  
}

/* create the objects for each servo */
void setupServos(){
  
  for (uint8_t i=0;i<NUM_SERVOS;i++){     
    switch (start_action){
      case 0:
        //do nothing
      break;
      case 1:
        //move to start position
        //servos[i].write(15,SPEED);
      break;
      case 2:
        //mode to end position
        //servos[i].write(170,SPEED);
      break;      
      case 3://move to last position          
          moveServoToLastPosition(i);
      break;
    }   
    delay(100); 
  }
    
  detachServos();
}

void detachServos(){

  if (attach_servo) return; //don't dettach

  for (uint8_t i=0;i<NUM_SERVOS;i++){  
     if (servos[i].attached())  servos[i].detach();
  }
}

void moveServoToLastPosition(byte idx){
    byte pos=cbus.getInternalNodeVar(idx+1);    
    if (pos < 175){    
      servos[idx].write(pos,SPEED);//avoid the kick when power on
      delay(300);  //wait the timmer to be ok
      servos[idx].attach(servopins[idx]); 
      servos[idx].write(pos,SPEED);
    }
}

/* is the servo to be activated or not */
boolean isServoActive(uint8_t index){
  return checkBit(active_servo,index);
}
/* is the servo by index to togle or not */
boolean isServoToTogle(uint8_t index){
  return checkBit(togle_servo,index);
}

/* check the if a bit is set or not */
boolean checkBit(byte *array,uint8_t index){
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
/* get the events vars for activate servos and to togle the servos:invert behaviour on on/off events */
void getServosArray(Message *msg,MergCBUS *mcbus){
  active_servo[0]=mcbus->getEventVar(msg,ACTIVE_SERVO_VAR);
  active_servo[1]=mcbus->getEventVar(msg,ACTIVE_SERVO_VAR + 1);
  togle_servo[0]=mcbus->getEventVar(msg,TOGLE_SERVO_VAR);
  togle_servo[1]=mcbus->getEventVar(msg,TOGLE_SERVO_VAR + 1);
}

/* the the flags var and initialize them */
void initializeFlags(){
    byte flag = cbus.getNodeVar(SERVO_FLAGS_VAR);
    start_action = flag && 0x03; //first 2 bits    
    attach_servo = flag && 0x04; //bit 3        
}
