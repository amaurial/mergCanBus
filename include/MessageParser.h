#ifndef BYTE_TYPE
#define BYTE_TYPE
typedef char byte;
#endif // BYTE_TYPE

#ifndef MESSAGEPARSER_H
#define MESSAGEPARSER_H

#include "CANMessage.h"
#include "Message.h"

#define MSGSIZE 256


class MessageParser
{
    public:
        MessageParser();
        virtual ~MessageParser();
        bool parse(Message *message,CANMessage *canMessage);
        void skipConfigMessage(){_skipCONFIG=true;};
        void processConfigMessage(){_skipCONFIG=false;};
        void skipReservedMessage(){_skipRESERVED=true;};
        void processReservedMessage(){_skipRESERVED=false;};
        void skipDCCMessage(){_skipDCC=true;};
        void processDCCMessage(){_skipDCC=false;};
        void skipAccessoryMessage(){_skipACCESSORY=true;};
        void processAccessoryMessage(){_skipACCESSORY=false;};
        void skipGeneralMessage(){_skipGENERAL=true;};
        void processGeneralMessage(){_skipGENERAL=false;};
        bool skipMessage(message_type msg);

    protected:
    private:
        CANMessage *_canMessage;
        Message *_message;
        bool getOpc();
        bool getPriority();
        bool getDataSize();
        bool getCanId();
        bool getType();
        bool getNodeNumber();
        bool getEventNumber();
        bool getRTR();
        message_type messages[MSGSIZE];//make an index of message types. opc is the array index
        void loadMessageType();
        bool _skipRESERVED;
        bool _skipCONFIG;
        bool _skipDCC;
        bool _skipACCESSORY;
        bool _skipGENERAL;

};

#endif // MESSAGEPARSER_H
