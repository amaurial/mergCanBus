#include "Message.h"

Message::Message()
{
    //ctor

    clear();
    loadMessageType();
    loadMessageConfig();
}
Message::Message(unsigned int canId,
                 unsigned int opc,
                 unsigned int nodeNumber,
                 unsigned int eventNumber,
                 byte data [DATA_SIZE] ,
                 unsigned int priority)
{
    //ctor
    Message();
    setCanId(canId);
    setOpc(opc);
    setNodeNumber(nodeNumber);
    setEventNumber(eventNumber);
    setPriority(priority);
    int i=0;
    for (i=0;i<DATA_SIZE;i++){_data[i]=data[i];}
}

Message::~Message()
{
    //dtor
}

void Message::setData(byte val[DATA_SIZE] )
{
    int i=0;
    for (i=0;i<DATA_SIZE;i++){_data[i]=val[i];}

}

void Message::clear(){
    setCanId(0);
    setOpc(0);
    setType(RESERVED);
    setEventNumber(0);
    setNodeNumber(0);
    setData((byte*)"00000000");
    setPriority(0);
    setNumBytes(0);
    unsetRTR();
    setSession(0);
    setDecoder(0);
    setDeviceNumber(0);

}

//return 0 if OK
unsigned int Message::setCanMessage(CANMessage *canMessage){
    _canMessage=canMessage;
    if (_canMessage->getRTR()){
        setRTR();
    }
    if (getRTR()){
        return 0;
    }

    setCanId(_canMessage->getCanId());
    setOpc(_canMessage->getOpc());
    setType(messages[getOpc()]);
    setNumBytes(_canMessage->getDataSize());
    return 0;
}

//used to get the data fields directly
byte Message::getByte(byte pos){
    if (pos>=CANDATA_SIZE){
        return 0;
    }
    if (_canMessage->getOpc()==0){
        return 0;
    }
    return _canMessage->getData()[pos];

}

unsigned int Message::getNodeNumber(){
    //node number is always at the position 1 and 2
    byte* data=_canMessage->getData();
    unsigned int r=0;
    if (hasThisData(opc,NODE_NUMBER)){
            r=data[1];
            r=r<<8;
            r=r|data[2];
            //r=(unsigned int)word(data[1],data[2]);
    }
    return r;
}

byte Message::getSession(){
    //session is always at the position 1
    byte* data=_canMessage->getData();
    byte r=0;
    if (hasThisData(opc,SESSION)){
            r=data[1];
    }

    return r;

}

unsigned int Message::getEventNumber(){
    //node number is always at the position 3 and 4
    byte* data=_canMessage->getData();
    unsigned int r=0;
    if (hasThisData(opc,EVENT_NUMBER)){
            r=data[3];
            r=r<<8;
            r=r|data[4];
            //r=(unsigned int)word(data[3],data[4]);
    }

    return r;
}

unsigned int Message::getDeviceNumber(){
    //node number is always at the position 3 and 4 with exception from the messages
    //OPC_DDRS OPC_DDES OPC_RQDDS
    byte* data=_canMessage->getData();
    unsigned int r=0;
    if (hasThisData(opc,DEVICE_NUMBER)){
            if ((opc==OPC_DDRS) || (opc==OPC_DDES) || (opc==OPC_RQDDS)){
                r=data[1];
                r=r<<8;
                r=r|data[2];
                //r=(unsigned int)word(data[1],data[2]);
            }
            else {
                //r=(unsigned int)word(data[3],data[4]);
                r=data[3];
                r=r<<8;
                r=r|data[4];
            }
    }

    return r;
}

unsigned int Message::getDecoder(){
    //node number is always at the position 2 and 3
    byte* data=_canMessage->getData();
    unsigned int r=0;
    if (hasThisData(opc,DECODER)){
            //r=(unsigned int)word(data[2],data[3]);
            r=data[2];
            r=r<<8;
            r=r|data[3];
    }

    return r;
}
//CV#
unsigned int Message::getCV(){
    //node number is always at the position 2 and 3
    byte* data=_canMessage->getData();
    unsigned int r=0;
    if (hasThisData(opc,CV)){
            //r=(unsigned int)word(data[2],data[3]);
            r=data[2];
            r=r<<8;
            r=r|data[3];
    }

    return r;
}

//CV Val
unsigned int Message::getCVValue(){

    byte* data=_canMessage->getData();
    byte r=0;
    if (opc==OPC_WCVO || opc==OPC_WCVB || opc==OPC_QCVS || opc==OPC_PCVS){
        return data[4];
    }
    else if (opc==OPC_WCVS){
        return data[5];
    }
    return r;
}

//CV Mode
unsigned int Message::getCVMode(){
    //node number is always at the position 2 and 3
    byte* data=_canMessage->getData();
    byte r=0;
    if (opc==OPC_WCVS){
        return data[4];
    }
    return r;
}



byte Message::getConsist(){
    byte* data=_canMessage->getData();
    byte r=0;
    if (opc==OPC_PCON || opc==OPC_KCON){
        return data[2];
    }
    return r;
}

byte Message::getSpeedDir(){
    byte* data=_canMessage->getData();
    byte r=0;
    if (opc==OPC_DSPD){
        return data[2];
    }
    else if (opc==OPC_PLOC){
        return data[4];
    }
    return r;

}

byte Message::getEngineFlag(){
    byte* data=_canMessage->getData();
    byte r=0;
    if (opc==OPC_DFLG ){
        return data[2];
    }
    else if (opc==OPC_GLOC){
        return data[3];
    }
    return r;

}
//Space left to store events
byte Message::getAvailableEventsLeft(){
    byte* data=_canMessage->getData();
    byte r=0;
    if (opc==OPC_EVNLF ){
        return data[3];
    }
    return r;

}
//Stored events
byte Message::getStoredEvents(){
    byte* data=_canMessage->getData();
    byte r=0;
    if (opc==OPC_NUMEV ){
        return data[3];
    }
    return r;

}
//Fn
byte Message::getFunctionNumber(){
    byte* data=_canMessage->getData();
    byte r=0;
    if (opc==OPC_DFNON||opc==OPC_DFNOF||opc==OPC_DFUN){
        return data[2];
    }
    return r;
}
//Fn value
byte Message::functionValue(){
    byte* data=_canMessage->getData();
    byte r=0;
    if (opc==OPC_DFUN){
        return data[3];
    }
    return r;
}

//Status
byte Message::getStatus(){
    byte* data=_canMessage->getData();
    byte r=0;
    if (opc==OPC_SSTAT||opc==OPC_DFNOF){
        return data[2];
    }
    return r;
}
//Para#
byte Message::getParaIndex(){
    byte* data=_canMessage->getData();
    byte r=0;
    if (opc==OPC_RQNP || opc==OPC_NVSET || opc==OPC_NVANS){
        return data[3];
    }
    return r;
}
//Para
byte Message::getParameter(){
    byte* data=_canMessage->getData();
    byte r=0;
    if (opc==OPC_NVSET||opc==OPC_NVANS){
        return data[4];
    }
    return r;
}
//NV#
byte Message::getNodeVariableIndex(){
    byte* data=_canMessage->getData();
    byte r=0;
    if (opc==OPC_NVRD || opc==OPC_NVSET || opc==OPC_NVANS){
        return data[3];
    }
    return r;
}
//NV
byte Message::getNodeVariable(){
    byte* data=_canMessage->getData();
    byte r=0;
    if (opc==OPC_NVSET||opc==OPC_NVANS){
        return data[4];
    }
    return r;
}
//EN#
byte Message::getEventIndex(){
    byte* data=_canMessage->getData();
    byte r=0;
    if (opc==OPC_NENRD || opc==OPC_REVAL || opc==OPC_NEVAL){
        return data[3];
    }
    else if (opc==OPC_EVLRNI){
        return data[5];
    }
     else if (opc==OPC_ENRSP){
        return data[7];
    }

    return r;
}
//EV#
byte Message::getEventVarIndex(){
    byte* data=_canMessage->getData();
    byte r=0;
    if ( opc==OPC_REVAL || opc==OPC_NEVAL){
        return data[4];
    }
    else if (opc==OPC_REQEV || opc==OPC_EVLRN){
        return data[5];
    }
     else if (opc==OPC_EVLRNI){
        return data[6];
    }

    return r;
}
//EV
byte Message::getEventVar(){
    byte* data=_canMessage->getData();
    byte r=0;
    if ( opc==OPC_NEVAL){
        return data[5];
    }
     else if (opc==OPC_EVLRN || opc==OPC_EVANS){
        return data[6];
    }
    else if (opc==OPC_EVLRNI){
        return data[7];
    }

    return r;
}

//check the filter for specific information
bool Message::hasThisData(byte opc, message_config_pos pos){

    if (!((opc>=0) && (opc<MSGSIZE))){
        return false;
    }

    if (bitRead(message_params[opc],pos)==1){
        return true;
    }
    else {
        return false;
    }

}

void Message::loadMessageConfig(){

    int i=0;
    for(i=0;i<MSGSIZE;i++)
    {
        message_params[i]=0;
    }
    bitSet(message_params[OPC_KLOC],SESSION);

    bitSet(message_params[OPC_QLOC],SESSION);

    bitSet(message_params[OPC_DKEEP],SESSION);

    //message_params[OPC_EXTC]=GENERAL;
    bitSet(message_params[OPC_RLOC],DECODER);

    bitSet(message_params[OPC_SNN],NODE_NUMBER);

    bitSet(message_params[OPC_STMOD],SESSION);

    bitSet(message_params[OPC_PCON],SESSION);

    bitSet(message_params[OPC_KCON],SESSION);

    bitSet(message_params[OPC_DSPD],SESSION);

    bitSet(message_params[OPC_DFLG],SESSION);

    bitSet(message_params[OPC_DFNON],SESSION);

    bitSet(message_params[OPC_DFNOF],SESSION);

    bitSet(message_params[OPC_SSTAT],SESSION);

    bitSet(message_params[OPC_RQNN],NODE_NUMBER);

    bitSet(message_params[OPC_NNREL],NODE_NUMBER);

    bitSet(message_params[OPC_NNACK],NODE_NUMBER);

    bitSet(message_params[OPC_NNLRN],NODE_NUMBER);

    bitSet(message_params[OPC_NNULN],NODE_NUMBER);

    bitSet(message_params[OPC_NNCLR],NODE_NUMBER);

    bitSet(message_params[OPC_NNEVN],NODE_NUMBER);

    bitSet(message_params[OPC_NERD],NODE_NUMBER);

    bitSet(message_params[OPC_RQEVN],NODE_NUMBER);

    bitSet(message_params[OPC_WRACK],NODE_NUMBER);

    bitSet(message_params[OPC_RQDAT],NODE_NUMBER);

    bitSet(message_params[OPC_RQDDS],DEVICE_NUMBER);

    bitSet(message_params[OPC_BOOT],NODE_NUMBER);

    bitSet(message_params[OPC_ENUM],NODE_NUMBER);


    //message_params[OPC_EXTC1]=GENERAL;

    bitSet(message_params[OPC_DFUN],SESSION);

    bitSet(message_params[OPC_GLOC],DECODER);

    //message_params[OPC_ERR]=DCC;

    bitSet(message_params[OPC_CMDERR],NODE_NUMBER);

    bitSet(message_params[OPC_EVNLF],NODE_NUMBER);

    bitSet(message_params[OPC_NVRD],NODE_NUMBER);

    bitSet(message_params[OPC_NENRD],NODE_NUMBER);

    bitSet(message_params[OPC_RQNPN],NODE_NUMBER);

    bitSet(message_params[OPC_NUMEV],NODE_NUMBER);

    bitSet(message_params[OPC_CANID],NODE_NUMBER);

    //message_params[OPC_EXTC2]=GENERAL;

    bitSet(message_params[OPC_RDCC3],SESSION);
    bitSet(message_params[OPC_RDCC3],CV);

    bitSet(message_params[OPC_WCVO],SESSION);
    bitSet(message_params[OPC_WCVO],CV);

    bitSet(message_params[OPC_WCVB],SESSION);
    bitSet(message_params[OPC_WCVB],CV);

    bitSet(message_params[OPC_QCVS],SESSION);
    bitSet(message_params[OPC_QCVS],CV);

    bitSet(message_params[OPC_PCVS],SESSION);
    bitSet(message_params[OPC_PCVS],CV);

    bitSet(message_params[OPC_ACON],NODE_NUMBER);
    bitSet(message_params[OPC_ACON],EVENT_NUMBER);

    bitSet(message_params[OPC_ACOF],NODE_NUMBER);
    bitSet(message_params[OPC_ACOF],EVENT_NUMBER);

    bitSet(message_params[OPC_AREQ],NODE_NUMBER);
    bitSet(message_params[OPC_AREQ],EVENT_NUMBER);

    bitSet(message_params[OPC_AREQ],NODE_NUMBER);
    bitSet(message_params[OPC_AREQ],EVENT_NUMBER);

    bitSet(message_params[OPC_ARON],NODE_NUMBER);
    bitSet(message_params[OPC_ARON],EVENT_NUMBER);

    bitSet(message_params[OPC_AROF],NODE_NUMBER);
    bitSet(message_params[OPC_AROF],EVENT_NUMBER);

    bitSet(message_params[OPC_EVULN],NODE_NUMBER);
    bitSet(message_params[OPC_EVULN],EVENT_NUMBER);

    bitSet(message_params[OPC_NVSET],NODE_NUMBER);

    bitSet(message_params[OPC_NVANS],NODE_NUMBER);

    bitSet(message_params[OPC_ASON],NODE_NUMBER);
    bitSet(message_params[OPC_ASON],DEVICE_NUMBER);

    bitSet(message_params[OPC_ASOF],NODE_NUMBER);
    bitSet(message_params[OPC_ASOF],DEVICE_NUMBER);

    bitSet(message_params[OPC_ASRQ],NODE_NUMBER);
    bitSet(message_params[OPC_ASRQ],DEVICE_NUMBER);

    bitSet(message_params[OPC_PARAN],NODE_NUMBER);

    bitSet(message_params[OPC_REVAL],NODE_NUMBER);

    bitSet(message_params[OPC_ARSON],NODE_NUMBER);
    bitSet(message_params[OPC_ARSON],DEVICE_NUMBER);

    bitSet(message_params[OPC_ARSOF],NODE_NUMBER);
    bitSet(message_params[OPC_ARSOF],DEVICE_NUMBER);

    //message_params[OPC_EXTC3]=GENERAL;
    //message_params[OPC_RDCC4]=DCC;

    bitSet(message_params[OPC_WCVS],SESSION);
    bitSet(message_params[OPC_WCVS],CV);

    bitSet(message_params[OPC_ACON1],NODE_NUMBER);
    bitSet(message_params[OPC_ACON1],EVENT_NUMBER);

    bitSet(message_params[OPC_ACOF1],NODE_NUMBER);
    bitSet(message_params[OPC_ACOF1],EVENT_NUMBER);

    bitSet(message_params[OPC_REQEV],NODE_NUMBER);
    bitSet(message_params[OPC_REQEV],EVENT_NUMBER);

    bitSet(message_params[OPC_ARON1],NODE_NUMBER);
    bitSet(message_params[OPC_ARON1],EVENT_NUMBER);

    bitSet(message_params[OPC_AROF1],NODE_NUMBER);
    bitSet(message_params[OPC_AROF1],EVENT_NUMBER);

    bitSet(message_params[OPC_NEVAL],NODE_NUMBER);

    bitSet(message_params[OPC_PNN],NODE_NUMBER);

    bitSet(message_params[OPC_ASON1],NODE_NUMBER);
    bitSet(message_params[OPC_ASON1],DEVICE_NUMBER);

    bitSet(message_params[OPC_ASOF1],NODE_NUMBER);
    bitSet(message_params[OPC_ASOF1],DEVICE_NUMBER);

    bitSet(message_params[OPC_ARSON1],NODE_NUMBER);
    bitSet(message_params[OPC_ARSON1],DEVICE_NUMBER);

    bitSet(message_params[OPC_ARSOF1],NODE_NUMBER);
    bitSet(message_params[OPC_ARSOF1],DEVICE_NUMBER);

    //message_params[OPC_EXTC4]=GENERAL;
    //message_params[OPC_RDCC5]=DCC;

    bitSet(message_params[OPC_WCVOA],DECODER);
    bitSet(message_params[OPC_WCVOA],CV);

    //message_params[OPC_FCLK]=ACCESSORY;

    bitSet(message_params[OPC_ACON2],NODE_NUMBER);
    bitSet(message_params[OPC_ACON2],EVENT_NUMBER);

    bitSet(message_params[OPC_ACOF2],NODE_NUMBER);
    bitSet(message_params[OPC_ACOF2],EVENT_NUMBER);

    bitSet(message_params[OPC_EVLRN],NODE_NUMBER);
    bitSet(message_params[OPC_EVLRN],EVENT_NUMBER);

    bitSet(message_params[OPC_EVANS],NODE_NUMBER);
    bitSet(message_params[OPC_EVANS],EVENT_NUMBER);

    bitSet(message_params[OPC_ARON2],NODE_NUMBER);
    bitSet(message_params[OPC_ARON2],EVENT_NUMBER);

    bitSet(message_params[OPC_AROF2],NODE_NUMBER);
    bitSet(message_params[OPC_AROF2],EVENT_NUMBER);

    bitSet(message_params[OPC_ASON2],NODE_NUMBER);
    bitSet(message_params[OPC_ASON2],DEVICE_NUMBER);

    bitSet(message_params[OPC_ASOF2],NODE_NUMBER);
    bitSet(message_params[OPC_ASOF2],DEVICE_NUMBER);

    bitSet(message_params[OPC_ARSON2],NODE_NUMBER);
    bitSet(message_params[OPC_ARSON2],DEVICE_NUMBER);

    bitSet(message_params[OPC_ARSOF2],NODE_NUMBER);
    bitSet(message_params[OPC_ARSOF2],DEVICE_NUMBER);

    //message_params[OPC_EXTC5]=GENERAL;
    //message_params[OPC_RDCC6]=DCC;

    bitSet(message_params[OPC_PLOC],SESSION);
    bitSet(message_params[OPC_PLOC],DECODER);

    //message_params[OPC_NAME]=CONFIG;

    bitSet(message_params[OPC_STAT],NODE_NUMBER);

    //message_params[OPC_PARAMS]=CONFIG;

    bitSet(message_params[OPC_ACON3],NODE_NUMBER);
    bitSet(message_params[OPC_ACON3],EVENT_NUMBER);

    bitSet(message_params[OPC_ACOF3],NODE_NUMBER);
    bitSet(message_params[OPC_ACOF3],EVENT_NUMBER);

    //message_params[OPC_ENRSP]=CONFIG;

    bitSet(message_params[OPC_ARON3],NODE_NUMBER);
    bitSet(message_params[OPC_ARON3],EVENT_NUMBER);

    bitSet(message_params[OPC_AROF3],NODE_NUMBER);
    bitSet(message_params[OPC_AROF3],EVENT_NUMBER);

    bitSet(message_params[OPC_EVLRNI],NODE_NUMBER);
    bitSet(message_params[OPC_EVLRNI],EVENT_NUMBER);

    bitSet(message_params[OPC_ACDAT],NODE_NUMBER);

    bitSet(message_params[OPC_ARDAT],NODE_NUMBER);

    bitSet(message_params[OPC_ASON3],NODE_NUMBER);
    bitSet(message_params[OPC_ASON3],DEVICE_NUMBER);

    bitSet(message_params[OPC_ASOF3],NODE_NUMBER);
    bitSet(message_params[OPC_ASOF3],DEVICE_NUMBER);

    bitSet(message_params[OPC_DDES],DEVICE_NUMBER);

    bitSet(message_params[OPC_DDRS],DEVICE_NUMBER);

    bitSet(message_params[OPC_ARSON3],NODE_NUMBER);
    bitSet(message_params[OPC_ARSON3],DEVICE_NUMBER);

    bitSet(message_params[OPC_ARSOF3],NODE_NUMBER);
    bitSet(message_params[OPC_ARSOF3],DEVICE_NUMBER);

    //message_params[OPC_EXTC6]=ACCESSORY;



}



void Message::loadMessageType()
{
    //clear all types
    int i=0;
    for(i=0;i<MSGSIZE;i++)
    {
        messages[i]=RESERVED;
    }
    messages[OPC_ACK]=GENERAL;
    messages[OPC_NAK]=GENERAL;
    messages[OPC_HLT]=GENERAL;
    messages[OPC_BON]=GENERAL;
    messages[OPC_TOF]=DCC;
    messages[OPC_TON]=DCC;
    messages[OPC_ESTOP]=DCC;
    messages[OPC_ARST]=GENERAL;
    messages[OPC_RTOF]=DCC;
    messages[OPC_RTON]=DCC;
    messages[OPC_RESTP]=DCC;
    messages[OPC_RSTAT]=CONFIG;
    messages[OPC_QNN]=CONFIG;
    messages[OPC_RQNP]=CONFIG;
    messages[OPC_RQMN]=CONFIG;
    messages[OPC_KLOC]=DCC;
    messages[OPC_QLOC]=DCC;
    messages[OPC_DKEEP]=DCC;
    messages[OPC_EXTC]=GENERAL;
    messages[OPC_RLOC]=DCC;
    messages[OPC_QCON]=RESERVED;
    messages[OPC_SNN]=GENERAL;
    messages[OPC_STMOD]=DCC;
    messages[OPC_PCON]=DCC;
    messages[OPC_KCON]=DCC;
    messages[OPC_DSPD]=DCC;
    messages[OPC_DFLG]=DCC;
    messages[OPC_DFNON]=DCC;
    messages[OPC_DFNOF]=DCC;
    messages[OPC_SSTAT]=DCC;
    messages[OPC_RQNN]=CONFIG;
    messages[OPC_NNREL]=CONFIG;
    messages[OPC_NNACK]=CONFIG;
    messages[OPC_NNLRN]=CONFIG;
    messages[OPC_NNULN]=CONFIG;
    messages[OPC_NNCLR]=CONFIG;
    messages[OPC_NNEVN]=CONFIG;
    messages[OPC_NERD]=CONFIG;
    messages[OPC_RQEVN]=CONFIG;
    messages[OPC_WRACK]=CONFIG;
    messages[OPC_RQDAT]=ACCESSORY;
    messages[OPC_RQDDS]=ACCESSORY;
    messages[OPC_BOOT]=CONFIG;
    messages[OPC_ENUM]=CONFIG;
    messages[OPC_EXTC1]=GENERAL;
    messages[OPC_DFUN]=DCC;
    messages[OPC_GLOC]=DCC;
    messages[OPC_ERR]=DCC;
    messages[OPC_CMDERR]=CONFIG;
    messages[OPC_EVNLF]=CONFIG;
    messages[OPC_NVRD]=CONFIG;
    messages[OPC_NENRD]=CONFIG;
    messages[OPC_RQNPN]=CONFIG;
    messages[OPC_NUMEV]=CONFIG;
    messages[OPC_CANID]=CONFIG;
    messages[OPC_EXTC2]=GENERAL;
    messages[OPC_RDCC3]=DCC;
    messages[OPC_WCVO]=DCC;
    messages[OPC_WCVB]=DCC;
    messages[OPC_QCVS]=DCC;
    messages[OPC_PCVS]=DCC;
    messages[OPC_ACON]=ACCESSORY;
    messages[OPC_ACOF]=ACCESSORY;
    messages[OPC_AREQ]=ACCESSORY;
    messages[OPC_ARON]=ACCESSORY;
    messages[OPC_AROF]=ACCESSORY;
    messages[OPC_EVULN]=CONFIG;
    messages[OPC_NVSET]=CONFIG;
    messages[OPC_NVANS]=CONFIG;
    messages[OPC_ASON]=ACCESSORY;
    messages[OPC_ASOF]=ACCESSORY;
    messages[OPC_ASRQ]=ACCESSORY;
    messages[OPC_PARAN]=CONFIG;
    messages[OPC_REVAL]=CONFIG;
    messages[OPC_ARSON]=ACCESSORY;
    messages[OPC_ARSOF]=ACCESSORY;
    messages[OPC_EXTC3]=GENERAL;
    messages[OPC_RDCC4]=DCC;
    messages[OPC_WCVS]=DCC;
    messages[OPC_ACON1]=ACCESSORY;
    messages[OPC_ACOF1]=ACCESSORY;
    messages[OPC_REQEV]=CONFIG;
    messages[OPC_ARON1]=ACCESSORY;
    messages[OPC_AROF1]=ACCESSORY;
    messages[OPC_NEVAL]=CONFIG;
    messages[OPC_PNN]=GENERAL;
    messages[OPC_ASON1]=ACCESSORY;
    messages[OPC_ASOF1]=ACCESSORY;
    messages[OPC_ARSON1]=ACCESSORY;
    messages[OPC_ARSOF1]=ACCESSORY;
    messages[OPC_EXTC4]=GENERAL;
    messages[OPC_RDCC5]=DCC;
    messages[OPC_WCVOA]=DCC;
    messages[OPC_FCLK]=ACCESSORY;
    messages[OPC_ACON2]=ACCESSORY;
    messages[OPC_ACOF2]=ACCESSORY;
    messages[OPC_EVLRN]=CONFIG;
    messages[OPC_EVANS]=CONFIG;
    messages[OPC_ARON2]=ACCESSORY;
    messages[OPC_AROF2]=ACCESSORY;
    messages[OPC_ASON2]=ACCESSORY;
    messages[OPC_ASOF2]=ACCESSORY;
    messages[OPC_ARSON2]=ACCESSORY;
    messages[OPC_ARSOF2]=ACCESSORY;
    messages[OPC_EXTC5]=GENERAL;
    messages[OPC_RDCC6]=DCC;
    messages[OPC_PLOC]=DCC;
    messages[OPC_NAME]=CONFIG;
    messages[OPC_STAT]=DCC;
    messages[OPC_PARAMS]=CONFIG;
    messages[OPC_ACON3]=ACCESSORY;
    messages[OPC_ACOF3]=ACCESSORY;
    messages[OPC_ENRSP]=CONFIG;
    messages[OPC_ARON3]=ACCESSORY;
    messages[OPC_AROF3]=ACCESSORY;
    messages[OPC_EVLRNI]=CONFIG;
    messages[OPC_ACDAT]=ACCESSORY;
    messages[OPC_ARDAT]=ACCESSORY;
    messages[OPC_ASON3]=ACCESSORY;
    messages[OPC_ASOF3]=ACCESSORY;
    messages[OPC_DDES]=ACCESSORY;
    messages[OPC_DDRS]=ACCESSORY;
    messages[OPC_ARSON3]=ACCESSORY;
    messages[OPC_ARSOF3]=ACCESSORY;
    messages[OPC_EXTC6]=ACCESSORY;
}

