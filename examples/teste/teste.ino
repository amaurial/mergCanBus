//#include <CANMessage.h>
#include <SPI.h>
#include <MergCBUS.h>
#include <Message.h>
#include <EEPROM.h>

MergCBUS canbus=MergCBUS();
#define GREEN_LED 5
#define YELLOW_LED 4
#define PUSH_BUTTON 3

int pb_state=LOW;

void setup(){
  Serial.begin(115200);
  //pinMode(GREEN_LED,OUTPUT);
  //pinMode(YELLOW_LED,OUTPUT);
  pinMode(PUSH_BUTTON,INPUT);
  
  //canbus.setUpNewMemory();
  //id config
  canbus.getNodeId()->setNodeName("MODTEST");
  canbus.getNodeId()->setModuleId(1);
  canbus.getNodeId()->setManufacturerId(15);
  canbus.getNodeId()->setMinCodeVersion(1);
  canbus.getNodeId()->setMaxCodeVersion(0);
  canbus.getNodeId()->setSuportedEvents(30);
  canbus.getNodeId()->setSuportedEventsVariables(75);
  canbus.getNodeId()->setSuportedNodeVariables(25);
  
  
  canbus.dumpMemory();
  canbus.setLeds(GREEN_LED,YELLOW_LED);
  
  canbus.setDebug(true);
  canbus.initCanBus(53,CAN_125KBPS,10,200);  
}

void loop (){
  
  canbus.run();
  
  if (digitalRead(PUSH_BUTTON)==HIGH){
    pb_state=HIGH;
    delay(2000);
  }
  
  if (pb_state==HIGH && digitalRead(PUSH_BUTTON)==LOW){
      
      if (canbus.getNodeState()==SETUP){
        //finish setup
        canbus.setNodeState(NORMAL);
        return;
      }
      Serial.println("self enum");
      canbus.doSelfEnnumeration(false);
      
      while (canbus.isSelfEnumMode()){
        canbus.run();
      }      
      canbus.doSetup();      
      pb_state=LOW;
  }
  
  
    
  
}
