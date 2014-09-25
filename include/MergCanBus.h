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


enum process_mode{AUTOMATIC,MANUAL};
enum state {LEARN,UNLEARN,BOOT,NORMAL};
enum can_error {OK=0,UNKNOWN_MSG_TYPE=1};

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
        void setup();
        //let the bus level lib public
        //MCP_CAN Can;
    protected:
    private:
        void setBitMessage(byte pos,bool val);
        CANMessage canMessage;
        Message message;
        MergNodeIdentification nodeId;
        byte messageFilter;//bit filter about each message to handle. by default avoid reserved messages
        process_mode runMode;
        bool messageToHandle;
        bool readCanBus();
        void sendCanMessage();
        void getStoredEvents();//events that were learned
        void getStoredIDs();//node number,canId,device Number
        bool matchEvent();//
        unsigned int runAutomatic();
        state state_mode;
        void handleConfigMessages();
        void handleDCCMessages();
        void handleACCMessages();
        void handleGeneralMessages();

};

#endif // MESSAGEPARSER_H
