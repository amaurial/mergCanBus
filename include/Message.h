#ifndef BYTE_TYPE
#define BYTE_TYPE
typedef char byte;
#endif // BYTE_TYPE

#ifndef MESSAGE_H
#define MESSAGE_H


#define DATA_SIZE 8

enum message_type {RESERVED,DCC,CONFIG,ACCESSORY,GENERAL};

class Message
{
    public:
        Message();
        Message(unsigned int canId,
                 unsigned int opc,
                 unsigned int nodeNumber,
                 unsigned int eventNumber,
                 byte data [DATA_SIZE] ,
                 unsigned int priority);
        virtual ~Message();
        unsigned int getCanId() { return _canId; }
        void setCanId(unsigned int val) { _canId = val; }
        unsigned int getOpc() { return _opc; }
        void setOpc(unsigned int val) { _opc = val; }
        message_type getType() { return _type; }
        void setType(message_type val) { _type = val; }
        unsigned int getEventNumber() { return _eventNumber; }
        void setEventNumber(unsigned int val) { _eventNumber = val; }
        unsigned int getNodeNumber() { return _nodeNumber; }
        void setNodeNumber(unsigned int val) { _nodeNumber = val; }
        byte* getData() { return _data; }
        void setData(byte val[DATA_SIZE]);
        unsigned int getPriority() { return _priority; }
        void setPriority(unsigned int val) { _priority = val; }
        unsigned int getNumBytes() { return _numBytes; }
        void setNumBytes(unsigned int val) { _numBytes = val; }
        void setRTR() {_RTR=1;};
        void unsetRTR(){_RTR=0;};
        int getRTR(){return _RTR;};
        void setSession(unsigned int val) {_session=val;}
        unsigned int getSession(){return _session;}
        void setDecoder(byte H,byte L){_decoderAddress[0]=H;_decoderAddress[1]=L;}
        byte *getDecoder(){return _decoderAddress}
        void setEngineParameter(byte param){_engineParameter=param;}
        byte getEngineParameter (){return _engineParameter;}
        void clear();
    protected:
    private:
        unsigned int _canId;
        unsigned int _opc;
        message_type _type;
        unsigned int _eventNumber;
        unsigned int _nodeNumber;
        byte _data[DATA_SIZE] ;
        unsigned int _priority;
        unsigned int _numBytes;
        unsigned int _RTR;
        unsigned int _session;
        byte _decoderAddress[2];
        byte _engineParameter;


};

#endif // MESSAGE_H
