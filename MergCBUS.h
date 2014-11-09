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




#define SELF_ENUM_TIME 500
#define TEMP_BUFFER_SIZE 100

//#define lowByte(w) ((uint8_t) ((w) & 0xff))
//#define highByte(w) ((uint8_t) ((w) >> 8))


enum process_mode{AUTOMATIC,MANUAL};
enum state {LEARN,UNLEARN,BOOT,NORMAL,SELF_ENUMERATION,SETUP};
enum can_error {OK=0,UNKNOWN_MSG_TYPE=1,NO_MESSAGE=2};

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
        MCP_CAN Can;
        byte node_mode;                         //Slim or Flim
        void setBitMessage(byte pos,bool val);  //set or unset the bit on pos for messageFilter
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
        byte getMessageSize(byte opc);

        byte greenLed;
        byte yellowLed;

};

#endif // MESSAGEPARSER_H
