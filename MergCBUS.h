#ifndef BYTE_TYPE
#define BYTE_TYPE
//typedef char byte;
#endif // BYTE_TYPE

// define USE_FLEX_CAN to use the internal can h/w on the Teensy3.1/3.2 boards.

#if defined(TEENSYDUINO) 

    //  --------------- Teensy -----------------

    #if defined(__MK20DX128__)       
        #define BOARD "Teensy 3.0"
	#define USE_FLEXCAN	1
    #elif defined(__MK20DX256__)       
        #define BOARD "Teensy 3.2" 
	#define USE_FLEXCAN	1
    #elif defined(__MK64FX512__)
        #define BOARD "Teensy 3.5"
	#define USE_FLEXCAN	1
    #elif defined(__MK66FX1M0__)
        #define BOARD "Teensy 3.6"
	#define USE_FLEXCAN	1
    #else
       #error "Unknown board"
    #endif
#endif



#ifndef MESSAGEPARSER_H
#define MESSAGEPARSER_H

//#define DEBUGDEF 1
//#define DEBUGMSG 1

#include <Arduino.h>
#include <EEPROM.h>
#include <SPI.h>
#include <avr/wdt.h>
#include "Message.h"
#include "MergNodeIdentification.h"
#include "MergMemoryManagement.h"
#include "CircularBuffer.h"


#ifdef USE_FLEXCAN
	#define Reset_AVR() ;
	#include <FlexCAN.h>
	#define CAN_125KBPS 125000
	#define MCP_16MHz 1
#else
	#define Reset_AVR() asm volatile ("  jmp 0");
	#include "mcp_can.h"
#endif

#define SELF_ENUM_TIME 1000      /** Defines the timeout used for self ennumeration mode.Milliseconds*/
#define TEMP_BUFFER_SIZE 128    /** Size of a internal buffer for general usage.*/
#define SELF_ENUM_BUFFER_SIZE 99
#define BLINK_RATE 100

//#define lowByte(w) ((uint8_t) ((w) & 0xff))
//#define highByte(w) ((uint8_t) ((w) >> 8))

/**
*
*/
//enum process_mode{AUTOMATIC,MANUAL};
/**
*   Enum that contains the node state.
*/
enum state {LEARN=0,              /**< Learn mode. The node is read to learn events.*/
            UNLEARN=1,            /**< Unlearn mode. The node is read to erase events.*/
            BOOT=2,               /**< Boot mode. The node is prepared to receive a new firmware. Not supported by this library. Present just to be consistent with Merg modes.*/
            NORMAL=3,             /**< Normal mode of operarion. Receives On/Off events and do actions based on them.*/
            SELF_ENUMERATION=4,   /**< Node getting a new CAN ID for transmission.*/
            SETUP=5               /**< Node getting a new Node number or device number.*/
            };

/**
*   Enum that defines the return of messages.
*/
enum can_error {OK=0,                   /**< Message sent.*/
                UNKNOWN_MSG_TYPE=1,     /**< Unknown message.*/
                NO_MESSAGE=2            /**< No message received.*/
                };

/**
*   A general class that support the MergCBUS protocol.
*   The class is used to all operations regarding the protocol, but is flexible enough to allow you to use general can messages.
*   It uses a modified version of mcp_can.h, that included the CAN header manipulation and RTR messages.
*
*   The Teensy version uses a modified FlexCAN library updated to include reading and writing of RTR bit in CAN message which is available here: https://github.com/H4nky84/FlexCAN_Library.git
*
*   When using the CBUS the user has to set the node information:
*   -The manufacturer ID as a HEX numeric (If the manufacturer has a NMRA number this can be used)
*   -Minor code version as an alphabetic character (ASCII)
*   -Manufacturer’s module identifier as a HEX numeric
*   -Number of supported events as a HEX numeric
*   -Number of Event Variables per event as a HEX numeric
*   -Number of supported Node Variables as a HEX numeric
*   -Major version as a HEX numeric. (can be 0 if no major version allocated)
*   -Node Flags
*   You has to start the CAN by calling initCanBus(...);
*   If your board follows the Merg boards with on push button and red and yellow leds, you can specify then with setLeds() and setPushButton()
*   If you want the library to take care of a lot of stuff, you need to define your user function that implements the node logic and tell the lib
*   about it calling setUserHandlerFunction() and on the arduino loop() call the run() method. See the examples.
*   If you want to take care of all messages by yourself just read the messages using readCanBus() or readCanBus(header[],data[]) and use the getMessage() to get a wrapper over the can message.
*/

class MergCBUS
{
    public:

        typedef void (*userHandlerType)(Message*,MergCBUS*);/**< This is the user function for processing other messages. It receives a reference to a Message and a reference to MergCBUS.*/

        MergCBUS(byte num_node_vars,byte num_events,byte num_events_vars,byte max_device_numbers);
        virtual ~MergCBUS();
        /**\brief Set to skip processing certain message.
        * @param msg Message type not to process.
        */
        void skipMessage(message_type msg){setBitMessage (msg,true);};
        /**\brief Set to process certain message.
        * @param msg Message type to process.
        */
        void processMessage(message_type msg){setBitMessage (msg,false);};

        uint8_t run();

        bool hasMessageToHandle();
        /**\brief Get a reference to node identification.
        * @return Pointer to a @see MergNodeIdentification
        */
        MergNodeIdentification *getNodeId(){return &nodeId;};

        bool initCanBus(uint8_t port,unsigned int rate, unsigned int retries,unsigned int retryIntervalMilliseconds);
        bool initCanBus(uint8_t port);
        bool initCanBus(uint8_t port,unsigned int rate, const uint8_t clock, unsigned int retries,unsigned int retryIntervalMilliseconds);
        bool initCanBus(uint8_t port, const uint8_t clock);
        /**\brief Set the user function to handle other messages.*/
        void setUserHandlerFunction(userHandlerType f) {userHandler=f;};
        void setDCCHandlerFunction(userHandlerType f) {dccHandler=f;};
        void sendERRMessage(byte code);
        bool hasThisEvent();
        void cbusRead();
        void printSentMessage();
        void printReceivedMessage();
        /**\brief Set the node to slim mode.*/
        void setSlimMode();
        /**\brief Set the node to flim mode.*/
        void setFlimMode();
        /**\brief Get the node mode.*/
        byte getNodeMode(){return node_mode;};
        /**\brief Reset EEPROM.*/
        void setUpNewMemory();
        /**\brief Print all EEPROM values. Works just if DEBUGREF is set.*/
        void dumpMemory(){memory.dumpMemory();};
        /**\brief Set the CBUS modules stardard leds.*/
        void setLeds(byte green,byte yellow);
        /**\brief Set the CBUS modules stardard push button.*/
        void setPushButton(byte pb) {push_button=pb;pinMode(pb,INPUT_PULLUP);};

        /**\brief Set the standard node number for slim mode.
        * The user of this library has to create its own way of letting a customer set a node number in SLIM mode.
        * If a standard value is not set and a push button is set then the library will use the value 0 if it is a consumer and 4444 if it is a producer.
        * @param new node number
        */
        void setStdNN(unsigned int val){std_nn=val;};
        /**\brief Get the standard node number.
        * @return node number
        */
        unsigned int getStdNN(){return std_nn;};
        void saveNodeFlags();
        void sendMessage(Message *msg);
        //methods for getting parameters
        bool isAccOn();
        bool isAccOff();
        bool eventMatch(){return eventmatch;};
        uint8_t getEventIndex(Message *msg);
        byte getNodeVar(byte varIndex);
        bool setNodeVariable(byte ind, byte val);
        byte getInternalNodeVar(byte varIndex);
        void setInternalNodeVariable(byte ind, byte val);

        byte getEventVar(Message *msg,byte varIndex);

        void setDeviceNumber(unsigned int val,byte port);           //2 bytes representation for dn
        unsigned int getDeviceNumber(byte port);                    //2 bytes representation
        byte getDeviceNumberIndex(){return deviceNumberIdx;};
        state getNodeState(){return state_mode;};

        //send on/off events
        byte sendOnEvent(bool longEvent,unsigned int event);
        byte sendOffEvent(bool longEvent,unsigned int event);
        byte sendOnEvent1(bool longEvent,unsigned int event,byte var1);
        byte sendOffEvent1(bool longEvent,unsigned int event,byte var1);
        byte sendOnEvent2(bool longEvent,unsigned int event,byte var1,byte var2);
        byte sendOffEvent2(bool longEvent,unsigned int event,byte var1,byte var2);
        byte sendOnEvent3(bool longEvent,unsigned int event,byte var1,byte var2,byte var3);
        byte sendOffEvent3(bool longEvent,unsigned int event,byte var1,byte var2,byte var3);

        //send DCC Events
        byte sendGetSession(uint16_t loco);
        byte sendReleaseSession(uint8_t locsession);
        byte sendShareSession(uint16_t loco);
        byte sendStealSession(uint16_t loco);
        byte sendKeepAliveSession(uint8_t locsession);
        byte sendSpeedDir(uint8_t locsession,uint8_t speed,bool dforward);
        byte sendSetFun(uint8_t locsession,uint8_t fn);
        byte sendUnsetFun(uint8_t locsession, uint8_t fn);
        byte sendSpeedMode(uint8_t locsession, uint8_t mode);

        void doSelfEnnumeration(bool soft); //put the node in the self enummeration mode
        void doSetup();                     // put the node in the setup mode
        bool isSelfEnumMode();              //check if the node is in the self enum mode
        uint16_t getPromNN();
        uint16_t getNN();
        MergMemoryManagement memory;            //organize the eeprom memory and maintain a copy in RAM

    protected:
    private:
        //let the bus level lib private
#ifdef USE_FLEXCAN
    	FlexCAN CANbus;
#else
    	MCP_CAN Can;                            /** The CAN object. Deal with the transport layer.*/
#endif
        byte node_mode;                         /** Slim or Flim*/
        byte mergCanData[CANDATA_SIZE];         //can data . CANDATA_SIZE defined in message.h
        Message message;                        //canbus message representation
        MergNodeIdentification nodeId;          //node identification:name,manufacuter , ...
        byte messageFilter;                     //bit filter about each message to handle. by default avoid reserved messages

        bool softwareEnum;                      //true if the node is doing self ennumeration
        bool eventmatch;                        //true if the received message is found on learned events
        unsigned long std_nn;                   //standard node number for slim
        bool typeEventMatch;                    //true is long event, false is short event
        byte deviceNumberIdx;                   //in case of match set the devicenumber index starting on 0
        CircularBuffer msgBuffer;

        state state_mode;                       //actual state of the node
        unsigned long startTime;                //used for self ennumeration
        byte buffer[TEMP_BUFFER_SIZE];          //buffer to store can ids for self enum
        byte bufferIndex;                     //index that indicates the buffer size
        //unsigned int (*userHandler)(message*);  //pointer to function
        userHandlerType userHandler;
        userHandlerType dccHandler;
        bool messageToHandle;                   //true if the message was not automatically handled

        void setBitMessage(byte pos,bool val);  /** set or unset the bit on pos for messageFilter*/
        void getStoredEvents();                 //events that were learned
        void getStoredIDs();                    //node number,canId,device Number
        bool matchEvent();                      //
        uint8_t runAutomatic();            //process the majority of events automatic
        bool setNodeVariableAuto(byte ind, byte val,bool autoErr);

        bool readCanBus();
        bool readCanBus(byte buf_num);
        bool readCanBus(byte *msg,byte *header,byte *length,byte buf_num);

        byte handleConfigMessages();            //process config messages
        byte handleDCCMessages();               //process dcc messages
        byte handleACCMessages();               //process accessory messages
        byte handleGeneralMessages();           //process general messages


        void finishSelfEnumeration();           //finish the self ennumeration procedures
        void clearMsgToSend();                  //clear internal buffer
        byte sendCanMessage();                  //send the message in the buffer
        void loadMemory();                      //load the saved data to ram. not the events

        void sortArray(byte *a, byte n);        //used in selfennum
        void prepareMessage(byte opc);          //fill the buffer with specific messages
        void prepareMessageBuff(byte data0=0,byte data1=0,byte data2=0,byte data3=0,byte data4=0,byte data5=0,byte data6=0,byte data7=0);//fill the buffer byte by byte
        byte getMessageSize(byte opc);          //return the
        //leds and the push button controls
        void controlLeds();
        byte greenLed;
        byte yellowLed;
        byte ledGreenState;
        byte ledYellowState;
        long ledtimer;
        void controlPushButton();
        byte push_button;
        byte pb_state;

        void(* resetFunc) (void);           //declare reset function @ address 0. resets the arduino
        void learnEvent();                  //put the node in the learn event mode
        uint8_t mainProcess();

        void doOutOfService();              //put the node in the out of service node
        void initMemory();                  //load the eprom memory


        /**\brief Set the node state to a new state.
        * @param newstate One of states @see state
        */
        void setNodeState(state newstate){ state_mode=newstate;};
        byte accExtraData();
        byte getAccExtraData(byte idx);//idx starts at 1

        //timer functions for reading messages
        long timerInterval;

        //DCC functions
        byte sendShareStealSession(uint16_t loco,uint8_t mode);

};

#endif // MESSAGEPARSER_H
