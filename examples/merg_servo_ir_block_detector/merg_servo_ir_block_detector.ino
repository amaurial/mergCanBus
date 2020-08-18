/*
This example implements a ir sensor controller to detect block occupancy and control servos.
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
#define ACTIVE_SERVO_VAR 1          //2 bytes for that
#define TOGLE_SERVO_VAR  3          //2 bytes for that

//first 2 are to indicate which servo is on. 2 bytes to indicate to togle. 2 for start and end angle
#define VAR_PER_SERVO 6  //variables per servo

#define START_ANGLE_VAR 5          //var index for the start angle
#define END_ANGLE_VAR 6            //var index for the end angle


//pins where the servos are attached
byte servopins[]={1,2,3,4,5,6,7,8};
byte servo_vars[5];
VarSpeedServo servos[NUM_SERVOS];
int tlimit;
int rlimit;

byte active_servo[2];
byte togle_servo[2];
boolean attach_servo = false;
byte start_action = 3; //move to last position

long detachservos_time = 0; //control the time to detach the servo


//###############
//sensor definitions
#define NUMSENSORS 8
#define WAIT 500 //time to reset the counters
#define D 3      //number of multiple ocurrences of 1s
#define R 400    //analog level threshold. active when lower this number
#define C 2      //number of 1s after each other
#define E 200    //number of 0s ocurrences
#define ACTUAL_STATE_BIT 0
#define LAST_STATE_BIT 1
#define TEMP_STATE_BIT 2
#define TOGGLE_STATE_BIT 3


long t;
int r;
byte i,s,x,l;
typedef struct SENSORS{
  byte port;
  long ones;
  long zeros;
  long seqones;
  byte state;//bit 1 = estado atual bit2=ultimo estado
};

enum BLOCK_STATE {S0,S1, S2 ,END};
typedef struct BLOCK{
  byte sensor1;
  byte sensor2;
  byte sensor3;
  byte enter_sensor; //sensor1, sensor2 or sensor3
  bool enter_sensor_toggled;
  bool out_sensor_toggled;
  BLOCK_STATE state;
};

SENSORS sensors[NUMSENSORS];
BLOCK blocks[NUMSENSORS];

//##############



//CBUS definitions
#define GREEN_LED 8                            //merg green led port
#define YELLOW_LED 7                           //merg yellow led port
#define PUSH_BUTTON 9                          //std merg push button
#define NODE_EVENTS 30                         //max number of events
#define EVENTS_VARS VAR_PER_SERVO              //number of variables per event
#define DEVICE_NUMBERS NUMSENSORS+NUM_SERVOS   //number of device numbers. each servo can be a device

#define NODE_VARS 12
/*
2 bytes for togle and activate the sensors
1 for servo speed
1 byte for flags
flag1 (2 first bits) action to take on starting. see setupServos
flag2 (bit 3), indicate to maintain the servo attached after moving. default is 0 = detached

[0] = sensor active
[1] = togle sensor
[2] = servo speed
[3] = flags
[4] = 1st pair of sensors 4 bits to each sensor. example 0x12, sensor 1 and 2 forms a block. The sensor count starts at 1
[5] = 2st pair of sensors
[6] = 3st pair of sensors
[7] = 4st pair of sensors
[8] = 5st pair of sensors
[9] = 6st pair of sensors
[10] = 7st pair of sensors
[11] = 8st pair of sensors
*/

#define ACTIVE_IR_VAR               1
#define TOGLE_IR_VAR                2
#define SERVO_SPEED_VAR             3
#define SERVO_FLAGS_VAR             4
#define SERVO_STARTACTION_VAR       4
#define BLOCK_VAR                   5


//create the merg object
MergCBUS cbus=MergCBUS(NODE_VARS,NODE_EVENTS,EVENTS_VARS,DEVICE_NUMBERS);

void setup(){

  initializeFlags();
  //create the sensors
  setupSensors();
  //create the servos object
  setupServos();

  #ifdef DEBUGNODE
  Serial.begin(115200);
  delay(100);
  #else
  Serial.end();
  #endif

  //Configuration data for the node
  cbus.getNodeId()->setNodeName("MSERVOBL",8);  //node name
  cbus.getNodeId()->setModuleId(59);            //module number
  cbus.getNodeId()->setManufacturerId(0xA5);    //merg code
  cbus.getNodeId()->setMinCodeVersion(1);       //Version 1
  cbus.getNodeId()->setMaxCodeVersion(0);
  cbus.getNodeId()->setProducerNode(true);
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
  //if using mega. the pin is 53
  cbus.initCanBus(10);  //initiate the transport layer. pin=53, rate=125Kbps,10 tries,200 millis between each try

  #ifdef DEBUGNODE
  Serial.println(F("Setup finished"));
  Serial.print(F("NNN: "));
  Serial.print(cbus.getNN());
  Serial.print(F("\t"));
  Serial.println(cbus.getPromNN());
  Serial.print(F("CANID: "));
  Serial.println(cbus.getNodeId()->getCanID());

  Serial.print(F("Flim: "));
  Serial.println(cbus.getNodeId()->isFlimMode());
  #endif
  //config the blocks
  //TODO: add some call back func to reconfigure the node vars when config messages received
  setupBlocks();
}

void loop (){
  cbus.cbusRead();
  cbus.run();//do all logic

  if (cbus.getNodeState()==NORMAL){
    checkSensors();

    for (i=0;i<NUMSENSORS;i++){
        switch (blocks[i].state){
        case S0:
            //check if any of the sensors are on
            if (bitRead(sensors[blocks[i].sensor1].state, LAST_STATE_BIT)){
		            blocks[i].state = S1;
                blocks[i].enter_sensor=blocks[i].sensor1;
		            sendMessage(true,i);
            }
            else if (bitRead(sensors[blocks[i].sensor2].state, LAST_STATE_BIT)){
		            blocks[i].state = S2;
                blocks[i].enter_sensor=blocks[i].sensor2;
		            sendMessage(true,i);
            }
            else if (bitRead(sensors[blocks[i].sensor3].state, LAST_STATE_BIT)){
		            blocks[i].state = S1;
                blocks[i].enter_sensor=blocks[i].sensor3;
		            sendMessage(true,i);
            }
        break;
        case S1:
            //check for transition states. We change state just if the middle sensor toggle or another sensor
            //other then the block ones
            if (bitRead(sensors[blocks[i].sensor2].state, TOGGLE_STATE_BIT)){
          		blocks[i].state = S2;
            }

            if (bitRead(sensors[blocks[i].sensor1].state, TOGGLE_STATE_BIT)){
          		if (blocks[i].enter_sensor == blocks[i].sensor1){
          		    blocks[i].enter_sensor_toggled = true;
          		}
            }
            else if (bitRead(sensors[blocks[i].sensor3].state, TOGGLE_STATE_BIT)){
          		if (blocks[i].enter_sensor == blocks[i].sensor3){
          		    blocks[i].enter_sensor_toggled = true;
          		}
           }


           /*check if other sensor than the pair for this block toggled
            * if so, send message to untoggle and set the start state
            */
            for (int j=0;j<NUMSENSORS;j++){
              if ((j!=blocks[i].sensor1) && (j!=blocks[i].sensor2) && (j!=blocks[i].sensor3)){
                  if (bitRead(sensors[j].state, TOGGLE_STATE_BIT)){
                      sendMessage(false,i);
                      blocks[i].state = S0;
                      continue;
                  }
                }
            }
        break;
        case S2:
	   //check for transition states. corner sensors are the ones to toggle in this state
           //check if any of the sensors toggled
            if (bitRead(sensors[blocks[i].sensor1].state, TOGGLE_STATE_BIT)){
          		if (blocks[i].enter_sensor != blocks[i].sensor1){
          		    blocks[i].out_sensor_toggled = true;
          		    blocks[i].state = END;
          		}
            }
            else if (bitRead(sensors[blocks[i].sensor3].state, TOGGLE_STATE_BIT)){
          		if (blocks[i].enter_sensor != blocks[i].sensor3){
          		    blocks[i].out_sensor_toggled = true;
          		    blocks[i].state = END;
          		}
           }
        break;
        case END:
             //both sensor have to be unset
             //check if any of the sensors are on
            byte s1,s3;
            s1 = bitRead(sensors[blocks[i].sensor1].state, LAST_STATE_BIT) ;
	          s3 = bitRead(sensors[blocks[i].sensor3].state, LAST_STATE_BIT) ;
            if (s1 == s3 == 0){
		          blocks[i].state = S0;
		          sendMessage(false,i);
            }
        break;
	      }
    }
  }

  //debug memory
  if (digitalRead(PUSH_BUTTON)==LOW){
    cbus.dumpMemory();
  }

  if (detachservos_time > 0 && detachservos_time < millis()){
      detachServos();
      detachservos_time = 0;
  }

}

//user defined function. contains the module logic.called every time run() is called.
void myUserFunc(Message *msg,MergCBUS *mcbus){
  boolean onEvent;
  unsigned int converted_speed;
  int varidx=0;

  byte servo_start,servo_end;
  if (mcbus->eventMatch()){
     initializeFlags();
     onEvent=mcbus->isAccOn();
     getServosArray(msg,mcbus);
    // Serial.println(F("event match"));
     servo_start=mcbus->getEventVar(msg,START_ANGLE_VAR);
     servo_end=mcbus->getEventVar(msg,END_ANGLE_VAR);
     //Serial.println(servo_start);
     //Serial.println(servo_end);
      //get the events var and control the servos
      for (int i=0;i<NUM_SERVOS;i++){
        if (isServoActive(i)){
          //Serial.print(F("S "));
          //Serial.println(i);
          moveServo(onEvent,i,servo_start,servo_end);
        }
      }
      //set time to detach the servos. detach after 2 seconds
      detachservos_time=millis() + 2000;
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
    //write last pos to eprom
   //variables start with number 1
    cbus.setInternalNodeVariable(servoidx+1,lastPos);
    //if (!attach_servo){
    servos[servoidx].write(lastPos,SPEED);
    servos[servoidx].attach(servopins[servoidx]);
    //}
    servos[servoidx].write(lastPos,SPEED);

   //write last pos to eprom
   //variables start with number 1
   #ifdef  DEBUGNODE
      Serial.print(F("saving: "));
      Serial.print(servoidx+1);
      Serial.print(F("\t"));
      Serial.println(lastPos);
   #endif

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
  initializeFlags();
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
  }
  delay(1000);
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

    #ifdef  DEBUGNODE
      Serial.print(F("moving: "));
      Serial.print(idx+1);
      Serial.print(F("\tlast"));
      Serial.println(pos);
   #endif

    if (pos < 175){
      servos[idx].write(pos,SPEED);//avoid the kick when power on
      delay(300);  //wait the timmer to be ok
      servos[idx].attach(servopins[idx]);
      servos[idx].write(pos,SPEED);
    }
}

//get the events vars for activate servos and to togle the servos:invert behaviour on on/off events
void getServosArray(Message *msg,MergCBUS *mcbus){
  active_servo[0]=mcbus->getEventVar(msg,ACTIVE_SERVO_VAR);
  active_servo[1]=mcbus->getEventVar(msg,ACTIVE_SERVO_VAR + 1);
  togle_servo[0]=mcbus->getEventVar(msg,TOGLE_SERVO_VAR);
  togle_servo[1]=mcbus->getEventVar(msg,TOGLE_SERVO_VAR + 1);
}

//the the flags var and initialize them
void initializeFlags(){
    byte flag = cbus.getNodeVar(SERVO_FLAGS_VAR);
    start_action = flag && 0x03; //first 2 bits
    attach_servo = flag && 0x04; //bit 3
}

//################## END SERVO CODE ##################

//########## START SENSOR CODE ##############

void checkSensors(){

    for (i=0;i<NUMSENSORS;i++){
        r=analogRead(sensors[i].port);
        //Serial.print(F("S"));
        //Serial.print(i);
        //Serial.print(F("\t"));
        //Serial.print(r);
        //Serial.print(F("\t"));
        if (r<R) r=1;
        else r=0;

        //read last read
        s = bitRead(sensors[i].state,TEMP_STATE_BIT);
	//clear the togle
        bitWrite(sensors[i].state,TOGGLE_STATE_BIT,0); //togle

        if ((r == s) && (s == 1)){
          sensors[i].seqones++;
          if (sensors[i].seqones>C){
            sensors[i].ones++;
            //Serial.println(F("d"));
            sensors[i].zeros=0;
          }
        }
        else{
          sensors[i].seqones=0;
          sensors[i].zeros++;
        }
        //save last read
        bitWrite(sensors[i].state,TEMP_STATE_BIT,r);

        //set the actual state
        if (sensors[i].ones>D){
          bitWrite(sensors[i].state,ACTUAL_STATE_BIT,1);
        }

        if (sensors[i].zeros>E){
          bitWrite(sensors[i].state,ACTUAL_STATE_BIT,0);
        }
        //compare last state
        s = bitRead(sensors[i].state,ACTUAL_STATE_BIT);
        x = bitRead(sensors[i].state,LAST_STATE_BIT);
        if (s!=x){
	  //change of state
          bitWrite(sensors[i].state,LAST_STATE_BIT,s);
	  bitWrite(sensors[i].state,TOGGLE_STATE_BIT,1); //togle
           #ifdef  DEBUGNODE
            Serial.print(F("s"));
            Serial.print(i);
            Serial.print(F(": "));
            Serial.println(s);
           #endif
            /*
            if (isSensorActive(i)){
               sendMessage(s,i);
            }
           */
        }
    }
    //Serial.println();
    /*
    for (i=0;i<NUMSENSORS;i++){
      Serial.print(F("S"));
      Serial.print(i);
      Serial.print(F("\t"));
      Serial.print(sensors[i].ones);
      Serial.print(F("\t"));
      Serial.print(sensors[i].seqones);
      Serial.print(F("\t"));
      Serial.print(sensors[i].zeros);
      Serial.println();
    }
    Serial.println();
    */
    if ((millis() - t) > WAIT ){
      t=millis();
      for (i=0;i<NUMSENSORS;i++){
        sensors[i].ones=0;
        sensors[i].zeros=0;
        sensors[i].seqones=0;
      }
    }



}

//send the can message
void sendMessage(bool state,uint8_t sensor){
   unsigned int event;
   bool onEvent=state;

   event=sensor+1;
   if (togleSensor(sensor)){
     onEvent=!onEvent;
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
  if (sensor<NUMSENSORS){
    if (bitRead(active_ir,sensor)==1){
      resp=true;
    }
  }
  return resp;
}

//configure the sensors
void setupSensors(){
   sensors[0].port=A0;
   sensors[1].port=A1;
   sensors[2].port=A2;
   sensors[3].port=A3;
   sensors[4].port=A4;
   sensors[5].port=A5;
   sensors[6].port=A6;
   sensors[7].port=A7;
}
//config the blocks
//configure the sensors
void setupBlocks(){
   byte var;
   byte i;
   for (i=0;i<NUMSENSORS;i++){
       var = cbus.getNodeVar(BLOCK_VAR+i);
       blocks[i].sensor1= ((var & 0xf0) >> 4) - 1;//first part of the byte
       blocks[i].sensor2= (var & 0x0f) - 1;
   }
}

