#include "CANMessage.h"

CANMessage::CANMessage()
{
    //ctor
    clear();
}

CANMessage::CANMessage(byte data[DATA_SIZE],byte header[HEADER_SIZE])
{
    CANMessage();
    setData(data);
    set_header(header);
}
void CANMessage::setData(byte val[DATA_SIZE])
{
    int i=0;
    for (i=0;i<DATA_SIZE;i++){_data[i]=val[i];}
}
void CANMessage::set_header(byte val[HEADER_SIZE])
{
    int i=0;
    for (i=0;i<HEADER_SIZE;i++){_header[i]=val[i];}
}
/**
* Clear the buffers
**/
void CANMessage::clear()
{
    int i=0;
    for (i=0;i<DATA_SIZE;i++){_data[i]=0;}
    for (i=0;i<HEADER_SIZE;i++){_header[i]=0;}
}

byte CANMessage::getCanId()
{
    //CAN id is the 7 lower bits of 11 bits
    //the other 4 are the priority
  byte temp=0;
  temp=(_header[0]<<4)&0xF0;
  temp=(temp)|(_header[1]>>5);
  return temp;
}
/**
* first byte contains the opc and the number of bytes in the message
* the first 3 bits are the number of bytes
* the 5 last bits are the OPC
**/
byte CANMessage::getOpc()
{
    return _data[0];
}
/***
* first byte contains the opc and the number of bytes in the message
* the first 3 bits are the number of bytes
* the 5 last bits are the OPC
**/
byte CANMessage::getDataSize()
{
    byte n=_data[0]>>5;
    return n;
}
