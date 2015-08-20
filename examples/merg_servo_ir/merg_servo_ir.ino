/*
This example implements a ir sensor controller to detect block occupancy.
It drives 16 ir. Can be more in arduino Mega, but it just an example.
The module produces ON/OFF events.
The vents can be toogled by the first 2 node variables. 0 means ON when train enters, OFF when trains leaves.
1 means the oposit. Each bit of the 2 bytes are to ser togle or not.
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
#include <TimerOne.h>
#include <MergCBUS.h>
#include <Message.h>
#include <EEPROM.h> //required by the library
#include <VarSpeedServo.h>

#define DEBUGNODE

//Module definitions servo
#define NUM_SERVOS 8      //number of servos

#define SPEED 50              //servo speed
#define SERVO_START 0         //servo start angle
#define SERVO_END 180         //servo end angle
//pins where the servos are attached
//pins 9,10 and 15 don't work for many servos. servo library limitation
byte servopins[]={2,3,4,5,6,7,8,9};
byte servo_vars[5];
VarSpeedServo servos[NUM_SERVOS];
int tlimit;
int rlimit;
//Module definitions sensor
#define NUMSENSORS 6
#define TLIMIT 500
#define RLIMIT 25
struct SENSOR {
  int port;
  int state;
  unsigned long time;
  unsigned long resets;
};
byte togle_ir;
byte active_ir;

struct SENSOR sensors[NUMSENSORS];
int sensorport[NUMSENSORS]={A0, A1 ,A2 ,A3, A6,A7};

//CBUS definitions
#define GREEN_LED A4                            //merg green led port
#define YELLOW_LED A5                           //merg yellow led port
#define PUSH_BUTTON 0                          //std merg push button
#define PUSH_BUTTON1 1                         //debug push button
#define NODE_EVENTS 30                          //max number of events
#define EVENTS_VARS 1                          //number of variables per event
#define DEVICE_NUMBERS NUMSENSORS+NUM_SERVOS   //number of device numbers. each servo can be a device

#define NODE_VARS 4 + 3*NUM_SERVOS
//4 bytes for togle and activate the sensors and the servos
//2 * NUM_SERVOS of start, end angle, speed (3 bits when on and 3 bit when off) and initial position (2 bits)
#define TOGLE_IR_VAR       1
#define ACTIVE_IR_VAR      2


//create the merg object
MergCBUS cbus=MergCBUS(NODE_VARS,NODE_EVENTS,EVENTS_VARS,DEVICE_NUMBERS);

void setup(){

  pinMode(PUSH_BUTTON1,INPUT_PULLUP);//debug push button
  #ifdef DEBUGNODE
  Serial.begin(115200);
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
  setUpServos();  
  #ifdef DEBUGNODE
  Serial.println("Setup finished");
  #endif
  
}

void loop (){
  cbus.cbusRead();
  cbus.run();//do all logic

  if (cbus.getNodeState()==NORMAL){
    //checkSensors();
  }

  //debug memory
  if (digitalRead(PUSH_BUTTON1)==LOW){
    cbus.dumpMemory();
  }
}

//user defined function. contains the module logic.called every time run() is called.
void myUserFunc(Message *msg,MergCBUS *mcbus){
  boolean onEvent;  
  byte p;

  
  if (mcbus->eventMatch()){
     onEvent=mcbus->isAccOn();
     
     #ifdef DEBUGNODE
      Serial.println("event match");
     #endif
     
     //get the events var and control the servos
     p=mcbus->getEventVar(msg,1);
      for (int i=0;i<NUM_SERVOS;i++){
        getServoNodeVar(i);
        if (isServoAffected(i,p)){          
          if (onEvent){
            servos[i].write(servo_vars[1],map(servo_vars[2],0,7,127,1));
          }
          else{
            servos[i].write(servo_vars[0],map(servo_vars[3],0,7,127,1));
          }         
        }
      }
  }  
}

void checkSensors(){
  int state;
  int i;
  unsigned long actime;
  //int s=7;
  togle_ir=cbus.getNodeVar(1);  
  active_ir=cbus.getNodeVar(2); 
  
  for (i=0;i<NUMSENSORS;i++){
    state=getSensorState(i);
    //Serial.println(state);
    actime=millis();
    if (state==LOW){
      if (sensors[i].state==HIGH){
        //if (i==s){
        /*
        Serial.print("Sensor ");
        Serial.print(i);
        Serial.println(" ON");
        */
        //}
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
              sendMessage(false,i) ;
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
void sendMessage(bool state,unsigned int sensor){
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
bool togleSensor(int sensor){
  byte first,second;
  bool resp=false;
  first=cbus.getNodeVar(0);
  second=cbus.getNodeVar(1);
  //check if the bit is set
  if (sensor>0 && sensor<9){
    if (bitRead(first,sensor)==1){
      resp=true;
    }
  }
  else if (sensor>8 && sensor<17){
     if (bitRead(second,sensor)==1){
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
int getSensorState(int i){
  //return digitalRead(sensors[i].port);

  int j;
  float ntimes;
  ntimes=30;
  for (j=0;j<ntimes;j++){
    if (digitalRead(sensors[i].port)==0){
      return 0;
    }
  }
  return 1;

}

//create the objects for each servo
void setUpServos(){
  for (int i=0;i<NUM_SERVOS;i++){    
    servos[i].attach(servopins[i]);
    getServoNodeVar(i);
    switch (servo_vars[4]){
      case 0:
        //do nothing
      break;
      case 1:
        //move to start position
        servos[i].write(servo_vars[0],127);
      break;
      case 2:
        //mode to end position
        servos[i].write(servo_vars[1],127);
      break;
      case 3:
        //do nothing
      break;
      default: {
             
      }
    }
    
  }
}

//is the servo by index to togle or not
boolean isIrToTogle(int index){
  if (bitRead(togle_ir,index)==1){
    return true;
  }
  else{
     return false;
  }  
}

//is the servo by index affected by the event?
boolean isServoAffected(byte index,byte flag){
  if (bitRead(flag,index)==1){
    return true;
  }
  else{
     return false;
  }  
}

//
//get the events vars for activate servos and to togle the servos:invert behaviour on on/off events
void getServoNodeVar(byte servo_idx){
  byte p;
  
  servo_vars[0]=cbus.getNodeVar(3+3*servo_idx);//angle when off event. start angle
  servo_vars[1]=cbus.getNodeVar(4+3*servo_idx);//angle when on event. end angle
  p=cbus.getNodeVar(5+3*servo_idx);  
  servo_vars[2]=p & B11100000;//speed when on event
  servo_vars[3]=p & B00011100;//speed when off event
  servo_vars[4]=p & B00000011;//where to move on startup
      
}


