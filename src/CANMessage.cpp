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
