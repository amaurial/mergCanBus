#include "MessageReader.h"

MessageReader::MessageReader()
{
    //ctor
}
MessageReader::~MessageReader()
{
    //dtor
}
bool MessageReader::readMessage(CANMessage *canMessage)
{

    _canMessage=canMessage;
    createTestData();
    _canMessage->setData(data);
    _canMessage->set_header(header);
    _canMessage->unsetRTR();

}
void MessageReader::createTestData()
{
    header[0]=0xB0;
    header[1]=0x20;
    data[0]=0x23;
    data[1]=0x01;
    data[2]=0x00;
    data[3]=0x00;
    data[4]=0x00;
    data[5]=0x00;
    data[6]=0x00;
    data[7]=0x00;
}
