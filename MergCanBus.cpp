#include "MergCanBus.h"

MergCanBus::MergCanBus()
{
    //ctor
    messageFilter=0;
    bufferIndex=0;
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
    Can.set_cs(port);

    do {
        if (CAN_OK==Can.begin(rate)){
            return true;
        }
        r++;
        delay(retryIntervalMilliseconds);
    }while (r<retries);

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
            if (bufferIndex<TEMP_BUFFER_SIZE){
                buffer[bufferIndex]=canMessage.getCanId();
                bufferIndex++;
            }
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
    byte len=0;
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
    state_mode=SELF_ENUMERATION;
    Can.sendRTMMessage(nodeId.getCanID());
    timeDelay=millis();
}

void MergCanBus::finishSelfEnumeration(){
    state_mode=NORMAL;
    sortArray(buffer,bufferIndex);
    //run the buffer and find the lowest can_id
    byte cid=1;
    for (int i=0;i<bufferIndex;i++){
        if (cid<buffer[i]){
            break;
        }
        cid++;
    }
    nodeId.setCanID(cid);
    //TODO: store the nodId on the memory
}



byte MergCanBus::handleConfigMessages(){
    switch (message.getOpc()){
    case OPC_QNN:
        //response with a OPC_PNN if we have a node ID
        //[<MjPri><MinPri=3><CANID>]<B6><NN Hi><NN Lo><Manuf Id><Module Id><Flags>
        if (nodeId.getNodeNumber()>0){
            clearMsgToSend();
            mergCanData[0]=OPC_PNN;
            mergCanData[1]=highByte(nodeId.getNodeNumber());
            mergCanData[2]=lowByte(nodeId.getNodeNumber());
            mergCanData[3]=nodeId.getManufacturerId();
            mergCanData[4]=nodeId.getModuleId();
            mergCanData[5]=nodeId.getFlags();
            return sendCanMessage(6);
        }
    break;
    case OPC_RQNP:
        //Answer with OPC_PARAMS
        //<0xEF><PARA 1><PARA 2><PARA 3> <PARA 4><PARA 5><PARA 6><PARA 7>
        //The parameters are defined as:
        //Para 1 The manufacturer ID as a HEX numeric (If the manufacturer has a NMRA
        //number this can be used)
        //Para 2 Minor code version as an alphabetic character (ASCII)
        //Para 3 Manufacturer’s module identifier as a HEX numeric
        //Para 4 Number of supported events as a HEX numeric
        //Para 5 Number of Event Variables per event as a HEX numeric
        //Para 6 Number of supported Node Variables as a HEX numeric
        //Para 7 Major version as a HEX numeric. (can be 0 if no major version allocated)
        //Para 8 Node Flags
        if (state_mode==SETUP){
            clearMsgToSend();
            mergCanData[0]=OPC_PARAMS;
            mergCanData[1]=nodeId.getManufacturerId();
            mergCanData[2]=nodeId.getMinCodeVersion();
            mergCanData[3]=nodeId.getSuportedEvents();
            mergCanData[4]=nodeId.getSuportedEventsVariables();
            mergCanData[5]=nodeId.getSuportedNodeVariables();
            mergCanData[6]=nodeId.getMaxCodeVersion();
            mergCanData[7]=nodeId.getFlags();
            return sendCanMessage(8);
        }
    break;
    case OPC_RQNN:
        //Answer with OPC_NAME
        if (state_mode==SETUP){
            clearMsgToSend();
            mergCanData[0]=OPC_NAME;
            mergCanData[1]=nodeId.getNodeName()[0];
            mergCanData[2]=nodeId.getNodeName()[1];
            mergCanData[3]=nodeId.getNodeName()[2];
            mergCanData[4]=nodeId.getNodeName()[3];
            mergCanData[5]=nodeId.getNodeName()[4];
            mergCanData[6]=nodeId.getNodeName()[5];
            mergCanData[7]=nodeId.getNodeName()[6];
            return sendCanMessage(8);
        }
    break;

    case OPC_SNN:
        //set the node number
        if (state_mode==SETUP){
            nodeId.setNodeNumber(message.getNodeNumber());
            //[TODO:save the data in memory]
            clearMsgToSend();
            mergCanData[0]=OPC_NNACK;
            mergCanData[1]=highByte(nodeId.getNodeNumber());
            mergCanData[2]=lowByte(nodeId.getNodeNumber());
            return sendCanMessage(3);
            state_mode=NORMAL;
        }
    break;


    }

}

void MergCanBus::sortArray(byte *a, byte n){

  for (byte i = 1; i < n; ++i)
  {
    byte j = a[i];
    byte k;
    for (k = i - 1; (k >= 0) && (j < a[k]); k--)
    {
      a[k + 1] = a[k];
    }
    a[k + 1] = j;
  }
}

void MergCanBus::clearMsgToSend(){
    for (int i=0;i<CANDATA_SIZE;i++){
        mergCanData[i]=0;
    }
}

byte MergCanBus::sendCanMessage(byte message_size){
    byte r=Can.sendMsgBuf(nodeId.getCanID(),0,message_size,mergCanData);
    if (CAN_OK!=r){
        return r;
    }
    return OK;
}


