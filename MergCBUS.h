#ifndef BYTE_TYPE
#define BYTE_TYPE
//typedef char byte;
#endif // BYTE_TYPE

#ifndef MESSAGEPARSER_H
#define MESSAGEPARSER_H

#include "CANMessage.h"
#include "Message.h"
#include "MergNodeIdentification.h"
#include "mcp_can.h"
#include "MergMemoryManagement.h"




#define SELF_ENUM_TIME 500      /** Defines the timeout used for self ennumeration mode.*/
#define TEMP_BUFFER_SIZE 100    /** Size of a internal buffer for general usage.*/

//#define lowByte(w) ((uint8_t) ((w) & 0xff))
//#define highByte(w) ((uint8_t) ((w) >> 8))

/**
*
*/
//enum process_mode{AUTOMATIC,MANUAL};
/**
*   Enum that contains the node state.
*/
enum state {LEARN,              /**< Learn mode. The node is read to learn events.*/
            UNLEARN,            /**< Unlearn mode. The node is read to erase events.*/
            BOOT,               /**< Boot mode. The node is prepared to receive a new firmware. Not supported by this library. Present just to be consistent with Merg modes.*/
            NORMAL,             /**< Normal mode of operarion. Receives On/Off events and do actions based on them.*/
            SELF_ENUMERATION,   /**< Node getting a new CAN ID for transmission.*/
            SETUP               /**< Node getting a new Node number or device number.*/
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
        typedef unsigned int (*userHandlerType)(Message*,MergCBUS*);
        MergCBUS();
        virtual ~MergCBUS();
        void skipMessage(message_type msg){setBitMessage (msg,true);};
        void processMessage(message_type msg){setBitMessage (msg,false);};
        //bool run(process_mode mode);
        unsigned int run();
        bool hasMessageToHandle();
        MergNodeIdentification *getNodeId(){return &nodeId;};
        bool sendCanMessage(CANMessage *msg);
        bool initCanBus(unsigned int port,unsigned int rate, int retries,unsigned int retryIntervalMilliseconds);

        void setUserHandlerFunction(userHandlerType f) {userHandler=f;};
        void doSelfEnnumeration(bool soft);
        void setDebug(bool debug);
        void doSetup();
        void doOutOfService();
        void sendERRMessage(byte code);
        bool hasThisEvent();
        bool readCanBus();
        void printSentMessage();
        void printReceivedMessage();
        void setSlimMode(){node_mode=MTYP_SLIM;};
        void setFlimMode(){node_mode=MTYP_FLIM;};
        byte getNodeMode(){return node_mode;};
        void setUpNewMemory();
        void dumpMemory(){memory.dumpMemory();};
        void setLeds(byte green,byte yellow);
        void controlLeds();
        bool isSelfEnumMode();
        state getNodeState(){return state_mode;};
        void setNodeState(state newstate){ state_mode=newstate;};
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

};

#endif // MESSAGEPARSER_H
