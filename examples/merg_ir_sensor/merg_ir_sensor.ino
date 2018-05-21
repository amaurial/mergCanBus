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


//Module definitions
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
int sensorport[NUMSENSORS]={A8,A9 ,A10 ,A11, A12 ,A13, A14,A15};


//first 2 are to indicate which servo is on. 2 bytes to indicate to togle. 2 for start and end angle
#define VAR_PER_SENSOR 1  //variables per servo. used as reserve. it is mostly used by consumers

//CBUS definitions
#define GREEN_LED 27                  //merg green led port
#define YELLOW_LED 26                 //merg yellow led port
#define PUSH_BUTTON 25                //std merg push button
#define PUSH_BUTTON1 28               //debug push button
#define NODE_VARS 4                  //2 for togle events, 2 for spare
#define NODE_EVENTS NUMSENSORS*2     //max number of events in case of teaching short events
#define EVENTS_VARS VAR_PER_SENSOR   //number of variables per event
#define DEVICE_NUMBERS NUMSENSORS   //number of device numbers. each servo can be a device

//arduino mega has 4K, so it is ok.


//create the merg object
MergCBUS cbus=MergCBUS(NODE_VARS,NODE_EVENTS,EVENTS_VARS,DEVICE_NUMBERS);

//timer function to read the can messages
void readCanMessages(){
  //read the can message and put then in a circular buffer
  cbus.cbusRead();
}

void setup(){

  pinMode(PUSH_BUTTON1,INPUT_PULLUP);//debug push button
  Serial.begin(115200);

  //Configuration data for the node
  cbus.getNodeId()->setNodeName("MODIRSEN",8);  //node name
  cbus.getNodeId()->setModuleId(57);            //module number
  cbus.getNodeId()->setManufacturerId(0xA5);    //merg code
  cbus.getNodeId()->setMinCodeVersion(1);       //Version 1
  cbus.getNodeId()->setMaxCodeVersion(0);
  cbus.getNodeId()->setProducerNode(true);
  cbus.getNodeId()->setConsumerNode(false);
  cbus.setStdNN(999); //standard node number

  if (digitalRead(PUSH_BUTTON1)==LOW){
    //Serial.println(F("Setup new memory"));
    cbus.setUpNewMemory();
    cbus.setSlimMode();
    cbus.saveNodeFlags();
  }
  cbus.setLeds(GREEN_LED,YELLOW_LED);//set the led ports
  cbus.setPushButton(PUSH_BUTTON);//set the push button ports
  cbus.setUserHandlerFunction(&myUserFunc);//function that implements the node logic
  cbus.initCanBus(53,CAN_125KBPS,10,200);  //initiate the transport layer. pin=53, rate=125Kbps,10 tries,200 millis between each try

  //create the servos object
  setupSensors();
  //using timer
  Timer1.initialize(10000);//microseconds
  Timer1.attachInterrupt(readCanMessages);
  //Serial.println(F("Setup finished"));
}

void loop (){

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
        Serial.print(F("Sensor "));
        Serial.print(i);
        Serial.println(F(" ON"));
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
              Serial.print(F("Sensor "));
              Serial.print(i);
              Serial.print(F(" OFF time: "));
              Serial.print(actime-sensors[i].time);
              Serial.print(F(" resets:"));
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
byte getSensorState(int i){
  //return digitalRead(sensors[i].port);

  int j;
  float ntimes;
  ntimes=30;
  for (j=0;j<ntimes;j++){
    if (digitalRead(sensors[i].port)==0){
      return LOW;
    }
  }
  return HIGH;

}
