#ifndef CANMESSAGE_H
#define CANMESSAGE_H

#include <Arduino.h>

#define DATA_SIZE 8
#define HEADER_SIZE 2

class CANMessage
{
    public:
        CANMessage();
        CANMessage(byte data[DATA_SIZE],byte header[HEADER_SIZE]);
        byte* getData() { return &_data[0]; }
        void setData(byte val[DATA_SIZE] );
        byte* get_header() { return _header; }
        void set_header(byte val[HEADER_SIZE] );
        //has to be set by the message reader. the mcp library knows about the RTR bits
        void setRTR() {_RTR=true;};
        void unsetRTR(){_RTR=false;};
        bool getRTR(){return _RTR;};
        void clear();                                                  //clear the buffers
        byte getCanId();
        byte getOpc();
        byte getDataSize();                                         //canBusMessage size
        void setCanMsgSize(byte val){canMsgSize=val;};              //number of byte read from can
        byte getCanMsgSize(){return canMsgSize;};                   //number of byte read from can

    protected:
    private:
        byte _data[DATA_SIZE];          //CANBUS Data
        byte _header[HEADER_SIZE];      //CAN Header
        bool _RTR;                        //1 if RTR
        byte canMsgSize;                //number of byte read from can
};

#endif // CANMESSAGE_H
