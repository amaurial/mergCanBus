/*
This example implements a ir sensor controller to detect block occupancy and servos.
It drives 8 ir and 8 servos. Can be more in arduino Mega, but it just an example.
The module produces ON/OFF events and drives the servos based on other events.
The events can be toogled by the first 2 node variables. 0 means ON when train enters, OFF when trains leaves.
1 means the oposit. Each bit of the 2 bytes are to ser togle or not.
Usim FLIM mode teach on/off events.
It implements all automatic configuration, including learning events.
It does not handle DCC messages, but you can do it on your user function.
You can change the ports to fit to your arduino.
This node uses 500 bytes of EPROM to store events and the other information.
See MemoryManagement.h for memory configuration
To clear the memory, press pushbutton while reseting the arduino

*/

#include <Arduino.h>
#include <SPI.h> //required by the library
#include <MergCBUS.h>
#include <Message.h>
#include <EEPROM.h> //required by the library
#include <VarSpeedServo.h>

//#define DEBUGNODE

//Module definitions servo
#define NUM_SERVOS 8      //number of servos

#define SPEED 50              //servo speed
#define SERVO_START 0         //servo start angle
#define SERVO_END 180         //servo end angle
//first 2 are to indicate which servo is on. 2 bytes to indicate to togle. 2 for start and end angle
#define VAR_PER_SERVO 6  //variables per servo

#define START_ANGLE_VAR 5          //var index for the start angle
#define END_ANGLE_VAR 6            //var index for the end angle


//pins where the servos are attached
//pins 9,10 and 15 don't work for many servos. servo library limitation
byte servopins[]={1,2,3,4,5,6,7,8};
byte servo_vars[5];
VarSpeedServo servos[NUM_SERVOS];
int tlimit;
int rlimit;

byte active_servo[2];
byte togle_servo[2];

//Module definitions sensor
#define NUMSENSORS 8
#define TLIMIT 500
#define RLIMIT 25
struct SENSOR {
  int port;
  int state;
  unsigned long time;
  unsigned long resets;
};

struct SENSOR sensors[NUMSENSORS];
int sensorport[NUMSENSORS]={A0, A1 ,A2 ,A3, A4, A5, A6, A7};

//CBUS definitions
#define GREEN_LED 8                            //merg green led port
#define YELLOW_LED 7                           //merg yellow led port
#define PUSH_BUTTON 9                          //std merg push button
#define NODE_EVENTS 30                         //max number of events
#define EVENTS_VARS VAR_PER_SERVO              //number of variables per event
#define DEVICE_NUMBERS NUMSENSORS+NUM_SERVOS   //number of device numbers. each servo can be a device

#define NODE_VARS 3
//2 bytes for togle and activate the sensors
//1 for servo speed
//1 for servo action on starting
//[0] = sensor active
//[1] = togle sensor
//[2] = servo speed

#define ACTIVE_IR_VAR               0
#define TOGLE_IR_VAR                1
#define SERVO_SPEED_VAR             2
#define SERVO_STARTACTION_VAR       3



//create the merg object
MergCBUS cbus=MergCBUS(NODE_VARS,NODE_EVENTS,EVENTS_VARS,DEVICE_NUMBERS);

void setup(){
  
  #ifdef DEBUGNODE
  Serial.begin(115200);
  #elseif
  Serial.end();
  #endif

  //Configuration data for the node
  cbus.getNodeId()->setNodeName("MSERVOIR",8);  //node name
  cbus.getNodeId()->setModuleId(58);            //module number
  cbus.getNodeId()->setManufacturerId(0xA5);    //merg code
  cbus.getNodeId()->setMinCodeVersion(1);       //Version 1
  cbus.getNodeId()->setMaxCodeVersion(0);
  cbus.getNodeId()->setProducerNode(true);
  cbus.getNodeId()->setConsumerNode(true);
  cbus.setStdNN(999); //standard node number

  if (digitalRead(PUSH_BUTTON)==LOW){
    //Serial.println("Setup new memory");
    cbus.setUpNewMemory();
    cbus.setSlimMode();
    cbus.saveNodeFlags();
  }
  cbus.setLeds(GREEN_LED,YELLOW_LED);//set the led ports
  cbus.setPushButton(PUSH_BUTTON);//set the push button ports
  cbus.setUserHandlerFunction(&myUserFunc);//function that implements the node logic
  //if using mega. the pin is 53
  cbus.initCanBus(10);  //initiate the transport layer. pin=53, rate=125Kbps,10 tries,200 millis between each try

  //create the sensors
  setupSensors();
  //create the servos object
  setupServos();  
  #ifdef DEBUGNODE
  Serial.println("Setup finished");
  #endif
  
}

void loop (){
  cbus.cbusRead();
  cbus.run();//do all logic

  if (cbus.getNodeState()==NORMAL){
    checkSensors();
  }

  //debug memory
  if (digitalRead(PUSH_BUTTON)==LOW){
    cbus.dumpMemory();
  }
}

//user defined function. contains the module logic.called every time run() is called.
void myUserFunc(Message *msg,MergCBUS *mcbus){
    boolean onEvent;
  unsigned int converted_speed;
  int varidx=0;

  byte servo_start,servo_end;
  if (mcbus->eventMatch()){
     onEvent=mcbus->isAccOn();
     getServosArray(msg,mcbus);
    // Serial.println("event match");
     servo_start=mcbus->getEventVar(msg,START_ANGLE_VAR);
     servo_end=mcbus->getEventVar(msg,END_ANGLE_VAR);
     //Serial.println(servo_start);
     //Serial.println(servo_end);
      //get the events var and control the servos
      for (int i=0;i<NUM_SERVOS;i++){
        if (isServoActive(i)){
          //Serial.print("S ");
          //Serial.println(i);
          moveServo(onEvent,i,servo_start,servo_end);
        }
      }
  }
  else{
    //feedback messages


  }
}

//########## START SERVO CODE ##############

void moveServo(boolean event,byte servoidx,byte servo_start,byte servo_end){
    byte lastPos;
    if (event){
      if (isServoToTogle(servoidx)){
        //Serial.println("on-moving servo t");
        servos[servoidx].write(servo_start,SPEED);
        lastPos = servo_start;
      }
      else {
        //Serial.println("on-moving servo");
        servos[servoidx].write(servo_end,SPEED);
        lastPos = servo_end;
      }
    }
    else{
      if (isServoToTogle(servoidx)){
        //Serial.println("off-moving servo t");
        servos[servoidx].write(servo_end,SPEED);
        lastPos = servo_end;
      }
      else {
        //Serial.println("off-moving servo");
        servos[servoidx].write(servo_start,SPEED);
        lastPos = servo_start;
      }
    }
   //write last pos to eprom
   //variables start with number 1   
   cbus.setInternalNodeVariable(servoidx+1,lastPos);
}

//is the servo to be activated or not
boolean isServoActive(uint8_t index){
  return checkBit(active_servo,index);
}
//is the servo by index to togle or not
boolean isServoToTogle(uint8_t index){
  return checkBit(togle_servo,index);
}

//check the if a bit is set or not
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

//create the objects for each servo
void setupServos(){
  byte ac = cbus.getNodeVar(SERVO_STARTACTION_VAR);
  for (uint8_t i=0;i<NUM_SERVOS;i++){    
    servos[i].attach(servopins[i]);    
    switch (ac){
      case 0:
        //do nothing
      break;
      case 1:
        //move to start position
        servos[i].write(15,SPEED);
      break;
      case 2:
        //mode to end position
        servos[i].write(170,SPEED);
      break;      
      case 3://move to last position          
          moveServoToLastPosition(i);
      break;
    }    
  }
}

void moveServoToLastPosition(byte idx){
    byte pos=cbus.getInternalNodeVar(idx+1);    
    if (pos < 175){
       servos[idx].write(pos,SPEED);
    }
}

//get the events vars for activate servos and to togle the servos:invert behaviour on on/off events
void getServosArray(Message *msg,MergCBUS *mcbus){
  active_servo[0]=mcbus->getEventVar(msg,1);
  active_servo[1]=mcbus->getEventVar(msg,2);
  togle_servo[0]=mcbus->getEventVar(msg,3);
  togle_servo[1]=mcbus->getEventVar(msg,4);
}

//################## END SERVO CODE ##################

//########## START SENSOR CODE ##############

void checkSensors(){
  uint8_t state;
  uint8_t i;
  unsigned long actime;
    
  for (i=0;i<NUMSENSORS;i++){
    state=getSensorState(i);
    //Serial.println(state);
    actime=millis();
    if (state==LOW){
      if (sensors[i].state==HIGH){        
        sendMessage(true,i);
        sensors[i].state=LOW;
      }
      sensors[i].state=LOW;
      sensors[i].time=actime;
      sensors[i].resets++;
    }
    else{
      if (actime-sensors[i].time>TLIMIT){
        if (sensors[i].resets<RLIMIT){
          //give extra time
          sensors[i].time=actime;
        }
       else {
          if (sensors[i].state==LOW){
          //    if (i==s){
          /*
              Serial.print("Sensor ");
              Serial.print(i);
              Serial.print(" OFF time: ");
              Serial.print(actime-sensors[i].time);
              Serial.print(" resets:");
              Serial.println(sensors[i].resets);
            */
              if (isSensorActive(i)){
                  sendMessage(false,i) ;
	      }
            //  }
            sensors[i].state=HIGH;
            sensors[i].resets=0;
          }
       }
      }
    }
  }
}

//send the can message
void sendMessage(bool state,uint8_t sensor){
   unsigned int event;
   bool onEvent=true;    
   
   event=sensor;
   if (togleSensor(sensor)){
     onEvent=false;
   }
   if (onEvent){
     cbus.sendOnEvent(true,event);
   }
   else{
     cbus.sendOffEvent(true,event);
   }
}

//check if we have to togle the event
bool togleSensor(uint8_t sensor){
  
  bool resp=false;
  byte togle_ir=cbus.getNodeVar(TOGLE_IR_VAR); 
    
  //check if the bit is set
  if (sensor>0 && sensor<9){
    if (bitRead(togle_ir,sensor)==1){
      resp=true;
    }
  }  
  return resp;
}

//check if we have the sensor has active
bool isSensorActive(uint8_t sensor){
  
  bool resp=false;
  byte active_ir=cbus.getNodeVar(ACTIVE_IR_VAR); 
    
  //check if the bit is set
  if (sensor>0 && sensor<9){
    if (bitRead(active_ir,sensor)==1){
      resp=true;
    }
  }  
  return resp;
}

//configure the sensors
void setupSensors(){
  int i=0;
  for (i=0;i<NUMSENSORS;i++)  {
    sensors[i].state=HIGH;
    sensors[i].port=sensorport[i];
    pinMode(sensors[i].port,INPUT);
  }
}
//read the sensor state
uint8_t getSensorState(uint8_t i){
  //return digitalRead(sensors[i].port);

  uint8_t j;
  uint8_t ntimes;
  ntimes=30;
  for (j=0;j<ntimes;j++){
    if (digitalRead(sensors[i].port)==0){
      return 0;
    }
  }
  return 1;
}

//is the servo by index to togle or not
boolean isIrToTogle(uint8_t index){
  byte togle_ir=cbus.getNodeVar(TOGLE_IR_VAR); 
  if (bitRead(togle_ir,index)==1){
    return true;
  }
  else{
     return false;
  }  
}

