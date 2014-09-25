#include "MergCanBus.h"



MergCanBus::MergCanBus()
{
    //ctor
    messageFilter=0;
    //Can=MCP_CAN();
    canMessage=CANMessage();
    nodeId=MergNodeIdentification();

    message=Message();
    skipMessage(RESERVED);
}

MergCanBus::~MergCanBus()
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
        return OK;
    }

    if (message.getRTR()){
        //we are a device with can id
        //we need to answer this message
        if (nodeId.getCanID()!=0){
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
            return UNKNOWN_MSG_TYPE;

    }
    return OK;

}

