#ifndef BYTE_TYPE
#define BYTE_TYPE
typedef char byte;
#endif // BYTE_TYPE

#ifndef CANMESSAGE_H
#define CANMESSAGE_H

#define DATA_SIZE 8
#define HEADER_SIZE 2

class CANMessage
{
    public:
        CANMessage();
        CANMessage(byte data[DATA_SIZE],byte header[HEADER_SIZE]);
        byte* getData() { return _data; }
        void setData(byte val[DATA_SIZE] );
        byte* get_header() { return _header; }
        void set_header(byte val[HEADER_SIZE] );
        void setRTR() {_RTR=1;};
        void unsetRTR(){_RTR=0;};
        int getRTR(){return _RTR;};
        void clear();                   //clear the buffers
        byte getCanId();
        byte getOpc();
        byte getDataSize();
    protected:
    private:
        byte _data[DATA_SIZE];          //CANBUS Data
        byte _header[HEADER_SIZE];      //CAN Header
        int _RTR;                        //1 if RTR
};

#endif // CANMESSAGE_H
