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

//Module definitions servo
#define NUM_SERVOS 8      //number of servos
// first byte is to indicate which servo is on.
// 1 byte to indicate to togle. 
// 2 for start and end angle in % of 360 grad
#define VAR_PER_SERVO 6       //variables per servo
#define SPEED 50              //servo speed
#define SERVO_START 0         //servo start angle
#define SERVO_END 180         //servo end angle
//pins where the servos are attached
//pins 9,10 and 15 don't work for many servos. servo library limitation
byte servopins[]={1,2,3,4,5,6,7,8};
VarSpeedServo servos[NUM_SERVOS];
byte active_servo;
byte togle_servo;

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
byte togle_ir;

struct SENSOR sensors[NUMSENSORS];
int sensorport[NUMSENSORS]={A0, A1 ,A2 ,A3, A4 ,A5, A6,A7};

//first 2 are to indicate which servo is on. 2 bytes to indicate to togle. 2 for start and end angle
#define VAR_PER_SENSOR 1  //variables per servo. used as reserve. it is mostly used by consumers

//CBUS definitions
#define GREEN_LED 27                            //merg green led port
#define YELLOW_LED 26                           //merg yellow led port
#define PUSH_BUTTON 25                          //std merg push button
#define PUSH_BUTTON1 28                         //debug push button
#define NODE_VARS 4                             //2 for togle events, 2 for spare
#define NODE_EVENTS 30                          //max number of events
#define EVENTS_VARS VAR_PER_SENSOR              //number of variables per event
#define DEVICE_NUMBERS NUMSENSORS+NUM_SERVOS   //number of device numbers. each servo can be a device


#define EVENTS_VARS VAR_PER_SERVO   //number of variables per event
#define DEVICE_NUMBERS NUM_SERVOS   //number of device numbers. each servo can be a device
#define TOGLE_IR_VAR       1
#define TOGLE_SERVO_VAR    2
#define ACTIVE_SERVO_VAR   3
#define START_ANGLE_VAR    4          //var index for the start angle
#define END_ANGLE_VAR      5            //var index for the end angle

//arduino mega has 4K, so it is ok.


//create the merg object
MergCBUS cbus=MergCBUS(NODE_VARS,NODE_EVENTS,EVENTS_VARS,DEVICE_NUMBERS);

void setup(){

  pinMode(PUSH_BUTTON1,INPUT_PULLUP);//debug push button
  Serial.begin(115200);

  //Configuration data for the node
  cbus.getNodeId()->setNodeName("MSERVOIR",8);  //node name
  cbus.getNodeId()->setModuleId(58);            //module number
  cbus.getNodeId()->setManufacturerId(0xA5);    //merg code
  cbus.getNodeId()->setMinCodeVersion(1);       //Version 1
  cbus.getNodeId()->setMaxCodeVersion(0);
  cbus.getNodeId()->setProducerNode(true);
  cbus.getNodeId()->setConsumerNode(true);
  cbus.setStdNN(999); //standard node number

  if (digitalRead(PUSH_BUTTON1)==LOW){
    //Serial.println("Setup new memory");
    cbus.setUpNewMemory();
    cbus.setSlimMode();
    cbus.saveNodeFlags();
  }
  cbus.setLeds(GREEN_LED,YELLOW_LED);//set the led ports
  cbus.setPushButton(PUSH_BUTTON);//set the push button ports
  cbus.setUserHandlerFunction(&myUserFunc);//function that implements the node logic
  cbus.initCanBus(53);  //initiate the transport layer. pin=53, rate=125Kbps,10 tries,200 millis between each try

  //create the servos object
  setupSensors();
  setUpServos();  
  //Serial.println("Setup finished");
}

void loop (){
  cbus.cbusRead();
  cbus.run();//do all logic

  if (cbus.getNodeState()==NORMAL){
    checkSensors();
  }

  //debug memory
  if (digitalRead(PUSH_BUTTON1)==LOW){
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
     Serial.println("event match");
     servo_start=mcbus->getEventVar(msg,START_ANGLE_VAR);
     servo_end=mcbus->getEventVar(msg,END_ANGLE_VAR);
     Serial.println(servo_start);
     Serial.println(servo_end);
      //get the events var and control the servos
      for (int i=0;i<NUM_SERVOS;i++){

        if (isServoActive(i)){
          moveServo(onEvent,i,servo_start,servo_end);
        }
      }
  }
  else{
    //feedback messages


  }
}

void checkSensors(){
  int state;
  int i;
  unsigned long actime;
  //int s=7;

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

void moveServo(boolean event,byte servoidx,byte servo_start,byte servo_end){
    if (event){
      if (isServoToTogle(servoidx)){
        servos[servoidx].write(servo_start,SPEED);
      }
      else {
        Serial.println("moving servo");
        servos[servoidx].write(servo_end,SPEED);
      }
    }
    else{
      if (isServoToTogle(servoidx)){
        servos[servoidx].write(servo_end,SPEED);
      }
      else {
        servos[servoidx].write(servo_start,SPEED);
      }
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
  
  if (bitRead(active_servo,index)==1){
    return true;
  }
  else{
     return false;
  }  
}
//is the servo by index to togle or not
boolean isServoToTogle(int index){
  if (bitRead(togle_servo,index)==1){
    return true;
  }
  else{
     return false;
  }  
}

//
//get the events vars for activate servos and to togle the servos:invert behaviour on on/off events
void getServosArray(Message *msg,MergCBUS *mcbus){
  active_servo=mcbus->getEventVar(msg,2);  
  togle_servo=mcbus->getEventVar(msg,1);  
  togle_ir=mcbus->getEventVar(msg,0);  
}

