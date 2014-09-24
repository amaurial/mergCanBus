#include "MessageParser.h"



MergCanBus::MessageParser()
{
    //ctor
    messageFilter=0;
    canMessage=CANMessage();
    nodeId=MergNodeIdentification();

    message=Message();
    skipMessage(RESERVED);
}

MergCanBus::~MessageParser()
{
    //dtor
}

void MergCanBus::setBitMessage(byte pos,bool val){
    if (val){
        bitSet(messageFilter,pos);
    }
    else{
        bitClear(messageFilter,pos);
    }
}



