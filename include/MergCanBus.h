#ifndef BYTE_TYPE
#define BYTE_TYPE
typedef char byte;
#endif // BYTE_TYPE

#ifndef MESSAGEPARSER_H
#define MESSAGEPARSER_H

#include "CANMessage.h"
#include "Message.h"
#include "MergNodeIdentification.h"


#define MSGSIZE 256

enum process_mode{AUTOMATIC,MANUAL};

enum {}

class MergCanBus
{
    public:
        MessageParser();
        virtual ~MessageParser();
        void skipMessage(message_type msg){setBitMessage (msg,true);};
        void processMessage(message_type msg){setBitMessage (msg,false);};
        bool run(process_mode mode);
        bool hasMessageToHandle();
        MergNodeIdentification *getNodeId(){return &nodeId;};
        bool sendCanMessage(CANMessage *msg);
    protected:
    private:
        void setBitMessage(byte pos,bool val);
        CANMessage canMessage;
        Message message;
        MergNodeIdentification nodeId;
        byte messageFilter;//bit filter about each message to handle. by default avoid reserved messages
        process_mode runMode;
        bool messageToHandle;
        void readCanBus();
        void sendCanMessage();
        void storedEvents();//events that were learned
        void storedIDs();//node number,canId,device Number


};

#endif // MESSAGEPARSER_H
