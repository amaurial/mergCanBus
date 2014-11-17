#ifndef BYTE_TYPE
#define BYTE_TYPE
//typedef char byte;
#endif // BYTE_TYPE

#ifndef MESSAGEPARSER_H
#define MESSAGEPARSER_H

#include <arduino.h>
#include <EEPROM.h>
#include <SPI.h>
#include <avr/wdt.h>

//#include "CANMessage.h"
#include "Message.h"
#include "MergNodeIdentification.h"
#include "mcp_can.h"
#include "MergMemoryManagement.h"


//#define Reset_AVR() wdt_enable(WDTO_30MS); while(1) {} //reset
#define Reset_AVR() asm volatile ("  jmp 0");



#define SELF_ENUM_TIME 1000      /** Defines the timeout used for self ennumeration mode.*/
#define TEMP_BUFFER_SIZE 128    /** Size of a internal buffer for general usage.*/
#define SELF_ENUM_BUFFER_SIZE 99

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
*/

class MergCBUS
{
    public:

        typedef void (*userHandlerType)(Message*,MergCBUS*);/**< This is the user function for processing other messages. It receives a reference to a Message and a reference to MergCBUS.*/
        MergCBUS();
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
        void doSelfEnnumeration(bool soft);
        void setDebug(bool debug);
        void doSetup();
        void doOutOfService();
        void sendERRMessage(byte code);
        bool hasThisEvent();
        bool readCanBus();
        bool readCanBus(byte *msg,byte *header,byte *length);
        void printSentMessage();
        void printReceivedMessage();
        /**\brief Set the node to slim mode.*/
        void setSlimMode();
        /**\brief Set the node to flim mode.*/
        void setFlimMode();
        /**\brief Get the node mode.*/
        byte getNodeMode(){return node_mode;};
        void initMemory();
        void setUpNewMemory();
        /**\brief Print all EEPROM values.*/
        void dumpMemory(){memory.dumpMemory();};
        void setLeds(byte green,byte yellow);
        void controlLeds();
        bool isSelfEnumMode();
        state getNodeState(){return state_mode;};
        /**\brief Set the node state to a new state.
        * @param newstate One of states @see state
        */
        void setNodeState(state newstate){ state_mode=newstate;};

        bool isAccOn();
        bool isAccOff();
        byte accExtraData();
        byte getAccExtraData(byte idx);//idx starts at 1
        void saveNodeFlags();


    protected:
    private:
        //let the bus level lib private
        MCP_CAN Can;                            /** The CAN object. Deal with the transport layer.*/
        byte node_mode;                         /** Slim or Flim*/
        void setBitMessage(byte pos,bool val);  /** set or unset the bit on pos for messageFilter*/
        CANMessage canMessage;                  //data from can bus
        byte mergCanData[CANDATA_SIZE];         //can data . CANDATA_SIZE defined in message.h
        Message message;                        //canbus message representation
        MergNodeIdentification nodeId;          //node identification:name,manufacuter , ...
        byte messageFilter;                     //bit filter about each message to handle. by default avoid reserved messages
        MergMemoryManagement memory;            //organize the eeprom memory and maintain a copy in RAM
        bool softwareEnum;
        bool DEBUG;

        state state_mode;                       //actual state of the node
        unsigned long timeDelay;                //used for self ennumeration
        byte buffer[TEMP_BUFFER_SIZE];          //buffer to store can ids for self enum
        byte bufferIndex;                     //index that indicates the buffer size
        //unsigned int (*userHandler)(message*);  //pointer to function
        userHandlerType userHandler;

        bool messageToHandle;

        //void sendCanMessage();
        void getStoredEvents();                 //events that were learned
        void getStoredIDs();                    //node number,canId,device Number
        bool matchEvent();                      //
        unsigned int runAutomatic();            //process the majority of events automatic

        byte handleConfigMessages();
        byte handleDCCMessages();
        byte handleACCMessages();
        byte handleGeneralMessages();


        void finishSelfEnumeration();
        void clearMsgToSend();
        byte sendCanMessage();
        void loadMemory();

        void sortArray(byte *a, byte n);
        void prepareMessage(byte opc);
        void prepareMessageBuff(byte data0=0,byte data1=0,byte data2=0,byte data3=0,byte data4=0,byte data5=0,byte data6=0,byte data7=0);
        byte getMessageSize(byte opc);

        byte greenLed;
        byte yellowLed;
        byte ledGreenState;
        byte ledYellowState;
        void(* resetFunc) (void);           //declare reset function @ address 0
};

#endif // MESSAGEPARSER_H
