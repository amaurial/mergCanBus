#include "MergCanBus.h"



MergCanBus::MergCanBus()
{
    //ctor
    messageFilter=0;
    otherCanID=1;//used for self ennumeration schema
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

bool MergCanBus::initCanBus(unsigned int port,unsigned int rate, int retries,unsigned int retryIntervalMilliseconds){

    unsigned int r=0;
    Can=MCP_CAN(port);

    do {
        if (CAN_OK==Can.begin(rate)){
            return true;
        }
        r++;
        delay(retryIntervalMilliseconds);
    }while (r<retries)

   return false;
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
        return NO_MESSAGE;
    }

    if (message.getRTR()){
        //if we are a device with can id
        //we need to answer this message
        if (nodeId.getNodeNumber()!=0){
            //create the response message with no data
            canMessage.clear();
            Can.sendMsgBuf(nodeId.getCanID(),0,0,canMessage.getData());
            return OK;
        }
    }
    //do self enumeration
    //collect the canid from messages with 0 size
    if (state_mode==SELF_ENUMERATION){
        unsigned long tdelay=millis()-timeDelay;

        if (tdelay>SELF_ENUM_TIME){
            finishSelfEnumeration();
        }

        if (canMessage.getCanMsgSize()==0){
            buffer[bufferIndex]=canMessage.getCanId();
        }
        return OK;
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

//read the can bus and load the data in canMessage
bool MergCanBus::readCanBus(){
    unsigned int len=0;
    if(CAN_MSGAVAIL == Can.checkReceive()) // check if data coming
    {
        canMessage.clear();
        Can.readMsgBuf(&len, canMessage.getData());
        Can.getCanHeader(canMessage.get_header());
        if (Can.isRTMMessage()!=0){
            canMessage.setRTR();
        }
        canMessage.setCanMsgSize(len);
        return true;
    }
    return false;
}

void MergCanBus::doSelfEnnumeration(){
    state_mode=SELF_ENNUMERATION;
    Can.sendRTMMessage(nodeId.getCanID());
    timeDelay=millis();
}

void MergCanBus::finishSelfEnumeration(){
    state_mode=NORMAL;
    //TODO:get the lowest free can id value
}

