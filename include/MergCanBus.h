#ifndef BYTE_TYPE
#define BYTE_TYPE
typedef char byte;
#endif // BYTE_TYPE

#ifndef MESSAGEPARSER_H
#define MESSAGEPARSER_H

#include "CANMessage.h"
#include "Message.h"


#define MSGSIZE 256

enum {}

class MergCanBus
{
    public:
        MessageParser();
        virtual ~MessageParser();
        void skipConfigMessage(){setBitMessage (message_type.CONFIG,true);};
        void processConfigMessage(){setBitMessage (message_type.CONFIG,false);};
        void skipReservedMessage(){setBitMessage (message_type.RESERVED,true);};
        void processReservedMessage(){_setBitMessage (message_type.RESERVED,false);};
        void skipDCCMessage(){_setBitMessage (message_type.DCC,true);};
        void processDCCMessage(){_setBitMessage (message_type.DCC,false);};
        void skipAccessoryMessage(){_setBitMessage (message_type.ACCESSORY,true);};
        void processAccessoryMessage(){_setBitMessage (message_type.ACCESSORY,false);};
        void skipGeneralMessage(){_setBitMessage (message_type.GENERAL,true);};
        void processGeneralMessage(){_setBitMessage (message_type.GENERAL,false);};
        void skipMessage(message_type msg);
        int parseACCMessage();

    protected:
    private:
        void setBitMessage(byte pos,bool val);
        CANMessage *_canMessage;
        Message *_message;
        message_type messages[MSGSIZE];//make an index of message types. opc is the array index
        void loadMessageType();
        byte messagesToSkip;

};

#endif // MESSAGEPARSER_H
