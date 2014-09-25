#include "MessageParser.h"



MergCanBus::MessageParser()
{
    //ctor
    messageFilter=0;
    Can=MCP_CAN();
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

unsigned int MergCanBus::runAutomatic(){

    if (!readCanBus()){
        //nothing to do
        return can_error.OK;
    }

    if (message.getRTR()){
        //we are a device with can id
        //we need to answer this message
        if (nodeId.canID!=0){
            //create the response message
            //TODO
            sendCanMessage(canMessage);
        }
    }

    //treat each message individually to interpret the code
    switch (message.getType()){
        case (DCC):
            handleDCCMessages();
        break;
        case (ACCESSORY):
            handleACCMessages();
        break;
        case (GENERAL):
            handleGeneralMessages();
        break;
        case (CONFIG):
            handleConfigMessages();
        break;
        default:
            return can_error.UNKNOWN_MSG_TYPE;

    }
    return can_error.OK;

}

