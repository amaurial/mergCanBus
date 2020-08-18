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
#include <MergCBUS.h>
#include <Message.h>
#include <EEPROM.h> //required by the library


//CBUS definitions
#define GREEN_LED A0                  //merg green led port
#define YELLOW_LED A1                 //merg yellow led port
#define PUSH_BUTTON 9                //std merg push button
#define NODE_VARS 0                  //2 for togle events, 2 for spare
#define NODE_EVENTS 0     //max number of events in case of teaching short events
#define EVENTS_VARS 0   //number of variables per event
#define DEVICE_NUMBERS 0   //number of device numbers. each servo can be a device

//arduino mega has 4K, so it is ok.


//create the merg object
MergCBUS cbus=MergCBUS(NODE_VARS,NODE_EVENTS,EVENTS_VARS,DEVICE_NUMBERS);

//timer function to read the can messages
void readCanMessages(){
  //read the can message and put then in a circular buffer
  //cbus.cbusRead();
}
long t;
void setup(){

  pinMode(PUSH_BUTTON,INPUT_PULLUP);//debug push button
  Serial.begin(115200);

  //Configuration data for the node
  cbus.getNodeId()->setNodeName("MESGGENE",8);  //node name
  cbus.getNodeId()->setModuleId(58);            //module number
  cbus.getNodeId()->setManufacturerId(0xA5);    //merg code
  cbus.getNodeId()->setMinCodeVersion(1);       //Version 1
  cbus.getNodeId()->setMaxCodeVersion(0);
  cbus.getNodeId()->setProducerNode(true);
  cbus.getNodeId()->setConsumerNode(false);
  cbus.setStdNN(999); //standard node number

  if (digitalRead(PUSH_BUTTON)==LOW){
    Serial.println(F("Setup new memory"));
    cbus.setUpNewMemory();
    cbus.setSlimMode();
    cbus.saveNodeFlags();
  }
  cbus.setLeds(GREEN_LED,YELLOW_LED);//set the led ports
  cbus.setPushButton(PUSH_BUTTON);//set the push button ports
  cbus.setUserHandlerFunction(&myUserFunc);//function that implements the node logic
  cbus.initCanBus(10);  //initiate the transport layer. pin=53, rate=125Kbps,10 tries,200 millis between each try

  //using timer
  //Timer1.initialize(10000);//microseconds
  //Timer1.attachInterrupt(readCanMessages);
  Serial.println(F("START"));
  t=millis();
}

int i=0;
void loop (){
  
  cbus.run();//do all logic

  if (cbus.getNodeState()==NORMAL){
    
  }
  //debug memory
  if (digitalRead(PUSH_BUTTON)==LOW){
    /*
      Serial.println(F("Sending messages"));
      for (int i=0;i<10;i++){
         sendMessage(i);
         //delay(1);
      } 
      Serial.println(F("Sent")); 
      */     
  }

  //if ((millis()-t)>0){
     sendMessage(i);
     i++;
     //t=millis();
     
  //}

  if (i>255){
    i=0;
  }
  //delay(8);
}

//user defined function. contains the module logic.called every time run() is called.
void myUserFunc(Message *msg,MergCBUS *mcbus){

}

//send the can message
void sendMessage(unsigned int n){  
     cbus.sendOnEvent(true,n); 
}


