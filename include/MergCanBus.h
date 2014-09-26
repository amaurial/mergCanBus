#ifndef BYTE_TYPE
#define BYTE_TYPE
typedef char byte;
#endif // BYTE_TYPE

#ifndef MESSAGEPARSER_H
#define MESSAGEPARSER_H

#include "CANMessage.h"
#include "Message.h"
#include "MergNodeIdentification.h"
//#include "mcp_can.h"


#define SELF_ENUM_TIME 200
#define TEMP_BUFFER_SIZE 99


enum process_mode{AUTOMATIC,MANUAL};
enum state {LEARN,UNLEARN,BOOT,NORMAL,SELF_ENUMERATION};
enum can_error {OK=0,UNKNOWN_MSG_TYPE=1,NO_MESSAGE=2};

class MergCanBus
{
    public:
        MergCanBus();
        virtual ~MergCanBus();
        void skipMessage(message_type msg){setBitMessage (msg,true);};
        void processMessage(message_type msg){setBitMessage (msg,false);};
        bool run(process_mode mode);
        bool hasMessageToHandle();
        MergNodeIdentification *getNodeId(){return &nodeId;};
        bool sendCanMessage(CANMessage *msg);
        bool initCanBus(unsigned int port,unsigned int rate, int retries,unsigned int retryIntervalMilliseconds);
        //let the bus level lib public
        MCP_CAN Can;
    protected:
    private:
        void setBitMessage(byte pos,bool val);
        CANMessage canMessage;
        Message message;
        MergNodeIdentification nodeId;
        byte messageFilter;//bit filter about each message to handle. by default avoid reserved messages
        process_mode runMode;
        state state_mode;
        byte otherCanID;
        unsigned long timeDelay;//used for self ennumeration
        byte buffer[TEMP_BUFFER_SIZE];
        byte bufferIndex=0;


        bool messageToHandle;
        bool readCanBus();
        void sendCanMessage();
        void getStoredEvents();//events that were learned
        void getStoredIDs();//node number,canId,device Number
        bool matchEvent();//
        unsigned int runAutomatic();

        void handleConfigMessages();
        void handleDCCMessages();
        void handleACCMessages();
        void handleGeneralMessages();

};

#endif // MESSAGEPARSER_H
