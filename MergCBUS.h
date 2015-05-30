#ifndef BYTE_TYPE
#define BYTE_TYPE
//typedef char byte;
#endif // BYTE_TYPE

#ifndef MESSAGEPARSER_H
#define MESSAGEPARSER_H

#include <Arduino.h>
#include <EEPROM.h>
#include <SPI.h>
#include <avr/wdt.h>
//#include <TimerOne.h>

//#include "CANMessage.h"
#include "Message.h"
#include "MergNodeIdentification.h"
#include "mcp_can.h"
#include "MergMemoryManagement.h"
#include "CircularBuffer.h"



//#define Reset_AVR() wdt_enable(WDTO_30MS); while(1) {} //reset
#define Reset_AVR() asm volatile ("  jmp 0");



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
        //bool run(process_mode mode);
        unsigned int run();
        bool hasMessageToHandle();
        /**\brief Get a reference to node identification.
        * @return Pointer to a @see MergNodeIdentification
        */
        MergNodeIdentification *getNodeId(){return &nodeId;};

        bool initCanBus(unsigned int port,unsigned int rate, unsigned int retries,unsigned int retryIntervalMilliseconds);
        /**\brief Set the user function to handle other messages.*/
        void setUserHandlerFunction(userHandlerType f) {userHandler=f;};

        void setDebug(bool debug);
        void sendERRMessage(byte code);
        bool hasThisEvent();
        bool readCanBus();
        bool readCanBus(byte buf_num);
        bool readCanBus(byte *msg,byte *header,byte *length,byte buf_num);
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
        /**\brief Print all EEPROM values.*/
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
        //methods for getthing parameters
        bool isAccOn();
        bool isAccOff();
        bool eventMatch(){return eventmatch;};
        unsigned int getEventIndex(Message *msg);
        byte getNodeVar(byte varIndex);
        byte getEventVar(Message *msg,byte varIndex);


        void setDeviceNumber(unsigned int val,byte port);           //2 bytes representation for dn
        unsigned int getDeviceNumber(byte port);                             //2 bytes representation
        byte getDeviceNumberIndex(){return deviceNumberIdx;};
        state getNodeState(){return state_mode;};
        //send events
        byte sendOnEvent(bool longEvent,unsigned int event);
        byte sendOffEvent(bool longEvent,unsigned int event);
        byte sendOnEvent1(bool longEvent,unsigned int event,byte var1);
        byte sendOffEvent1(bool longEvent,unsigned int event,byte var1);
        byte sendOnEvent2(bool longEvent,unsigned int event,byte var1,byte var2);
        byte sendOffEvent2(bool longEvent,unsigned int event,byte var1,byte var2);
        byte sendOnEvent3(bool longEvent,unsigned int event,byte var1,byte var2,byte var3);
        byte sendOffEvent3(bool longEvent,unsigned int event,byte var1,byte var2,byte var3);

        //set timmer interval
        void setTimerInterval(long value){timerInterval=value;};
        long getTimerInterval(){return timerInterval;}
        //void startTimer();
        //void stopTimer();
        void cbusRead();

    protected:
    private:
        //let the bus level lib private
        MCP_CAN Can;                            /** The CAN object. Deal with the transport layer.*/
        byte node_mode;                         /** Slim or Flim*/
        byte mergCanData[CANDATA_SIZE];         //can data . CANDATA_SIZE defined in message.h
        Message message;                        //canbus message representation
        MergNodeIdentification nodeId;          //node identification:name,manufacuter , ...
        byte messageFilter;                     //bit filter about each message to handle. by default avoid reserved messages
        MergMemoryManagement memory;            //organize the eeprom memory and maintain a copy in RAM
        bool softwareEnum;                      //true if the node is doing self ennumeration
        bool DEBUG;                             //true if debug mode
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
        bool messageToHandle;                   //true if the message was not automatically handled

        void setBitMessage(byte pos,bool val);  /** set or unset the bit on pos for messageFilter*/
        //void sendCanMessage();
        void getStoredEvents();                 //events that were learned
        void getStoredIDs();                    //node number,canId,device Number
        bool matchEvent();                      //
        unsigned int runAutomatic();            //process the majority of events automatic

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
        unsigned int mainProcess();
        void doSelfEnnumeration(bool soft); //put the node in the self enummeration mode
        void doSetup();                     // put the node in the setup mode
        void doOutOfService();              //put the node in the out of service node
        void initMemory();                  //load the eprom memory
        bool isSelfEnumMode();              //check if the node is in the self enum mode

        /**\brief Set the node state to a new state.
        * @param newstate One of states @see state
        */
        void setNodeState(state newstate){ state_mode=newstate;};
        byte accExtraData();
        byte getAccExtraData(byte idx);//idx starts at 1

        //timer functions for reading messages
        long timerInterval;

};

#endif // MESSAGEPARSER_H
