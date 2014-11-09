#ifndef BYTE_TYPE
#define BYTE_TYPE
//typedef char byte;
#endif // BYTE_TYPE

#ifndef MESSAGE_H
#define MESSAGE_H

//#define bitRead(val,pos){return 1;};
//#define bitSet(val,pos){return;};
//#define bitClear(val,pos){return;};


#include "CANMessage.h"
#include "opcodes.h"


#define CANDATA_SIZE 8
#define MSGSIZE 255

enum message_type {RESERVED=0,DCC=1,CONFIG=2,ACCESSORY=3,GENERAL=4};
enum message_config_pos {NODE_NUMBER=0,DEVICE_NUMBER=1,EVENT_NUMBER=2,SESSION=3,DECODER=4,CV=5};

class Message
{
    public:
        Message();
        Message(unsigned int canId,
                 unsigned int opc,
                 unsigned int nodeNumber,
                 unsigned int eventNumber,
                 byte data [CANDATA_SIZE] ,
                 unsigned int priority);
        virtual ~Message();
        unsigned int setCanMessage(CANMessage *canMessage);
        CANMessage *getCanMessage(){return _canMessage;}
        byte getByte(byte pos);

        byte getCanId() { return canId; }
        void setCanId(byte val) { canId = val; }

        byte getOpc() { return opc; }
        void setOpc(byte val) {opc = val; }

        message_type getType() { return _type; }
        void setType(message_type val) { _type = val; }

        unsigned int getEventNumber();
        void setEventNumber(unsigned int val) { _eventNumber = val; }

        unsigned int getNodeNumber();
        void setNodeNumber(unsigned int val) { _nodeNumber = val; }

        unsigned int getDeviceNumber();
        void setDeviceNumber(unsigned int val) { _deviceNumber = val; }

        byte* getData() { return _data; }
        void setData(byte val[CANDATA_SIZE]);

        byte getPriority() { return _priority;}
        void setPriority(byte val) { _priority = val; }

        byte getMessageSize();
        unsigned int getNumBytes() { return _numBytes; }
        void setNumBytes(unsigned int val) { _numBytes = val; }

        void setRTR() {_RTR=true;};
        void unsetRTR(){_RTR=false;};
        bool getRTR(){return _RTR;};

        void setSession(byte val) {_session=val;}
        byte getSession();

        unsigned int getDecoder();
        void setDecoder(unsigned int val) { _decoder = val; }

        unsigned int getCV();
        void setCV(unsigned int val) { _cv = val; }

        unsigned int getCVMode();
        unsigned int getCVValue();
        byte getConsist();
        byte getSpeedDir();
        byte getEngineFlag();
        byte getAvailableEventsLeft();
        byte getStoredEvents();
        byte getFunctionNumber();
        byte functionValue();
        byte getStatus();
        byte getParaIndex();
        byte getParameter();
        byte getNodeVariableIndex();
        byte getNodeVariable();
        byte getEventIndex();
        byte getEventVarIndex();
        byte getEventVar();
        void clear();

    protected:
    private:
        byte canId;
        byte opc;
        message_type _type;
        unsigned int _eventNumber;//2 bytes
        unsigned int _nodeNumber;//2 bytes
        unsigned int _deviceNumber;//2 bytes
        unsigned int _decoder;//2 bytes
        unsigned int _cv;//2 bytes
        byte _session;
        byte _data[CANDATA_SIZE] ;
        byte _priority;
        unsigned int _numBytes;
        bool _RTR;
        CANMessage* _canMessage;
        message_type messages[MSGSIZE];//make an index of message types. opc is the array index

        unsigned int message_params[MSGSIZE];//use each bit to hold a true false information about the message
        bool hasThisData(byte opc, message_config_pos pos);
        void loadMessageConfig();
        void loadMessageType();
        //erase later

        //void bitSet(unsigned int &val,unsigned int pos){return;};
        //byte bitRead(unsigned int &val,unsigned int pos){return 1;};


};

#endif // MESSAGE_H
