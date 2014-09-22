#ifndef BYTE_TYPE
#define BYTE_TYPE
typedef char byte;
#endif // BYTE_TYPE

#ifndef MESSAGE_H
#define MESSAGE_H


#define CANDATA_SIZE 8

enum message_type {RESERVED,DCC,CONFIG,ACCESSORY,GENERAL};

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
        byte getCanId() { return _canId; }
        void setCanId(byte val) { _canId = val; }
        byte getOpc() { return _opc; }
        void setOpc(byte val) { _opc = val; }
        message_type getType() { return _type; }
        void setType(message_type val) { _type = val; }
        unsigned int getEventNumber() { return _eventNumber; }
        void setEventNumber(unsigned int val) { _eventNumber = val; }
        unsigned int getNodeNumber() { return _nodeNumber; }
        void setNodeNumber(unsigned int val) { _nodeNumber = val; }
        byte* getData() { return _data; }
        void setData(byte val[CANDATA_SIZE]);
        byte getPriority() { return _priority; }
        void setPriority(byte val) { _priority = val; }
        unsigned int getNumBytes() { return _numBytes; }
        void setNumBytes(unsigned int val) { _numBytes = val; }
        void setRTR() {_RTR=1;};
        void unsetRTR(){_RTR=0;};
        int getRTR(){return _RTR;};
        void setSession(byte val) {_session=val;}
        byte getSession(){return _session;}
        void setDecoder(byte H,byte L){_decoderAddress[0]=H;_decoderAddress[1]=L;}
        byte *getDecoder(){return _decoderAddress}
        void setEngineParameter(byte param){_engineParameter=param;}
        byte getEngineParameter (){return _engineParameter;}
        void clear();
    protected:
    private:
        byte int _canId;
        byte int _opc;
        message_type _type;
        unsigned int _eventNumber;//2 bytes
        unsigned int _nodeNumber;//2 bytes
        byte _data[CANDATA_SIZE] ;
        byte _priority;
        unsigned int _numBytes;
        unsigned int _RTR;


};

#endif // MESSAGE_H
