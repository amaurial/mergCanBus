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
    for (int i=0;i<DATA_SIZE;i++){_data[i]=data[i];}
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
    setType(message_type.RESERVED);
    setEventNumber(0);
    setNodeNumber(0);
    setData("00000000");
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
    setRTR(_canMessage->getRTR());
    if (getRTR()){
        return 0;
    }

    setCanId(_canMessage->getCanId());
    setOpc(_canMessage->getOpc());
    setType(messages[getOpc()]);
    setNumBytes(_canMessage->getDataSize());
    return 0;
}

unsigned int Message::getNodeNumber(){
    //node number is always at the position 1 and 2

    char* data=_canMessage->getData();
    unsigned int r=0;
    if (hasThisData(_opc,message_config_pos.NODE_NUMBER)){


            r=(unsigned int)word(data[1],data[2]);
    }

    return r;

}

byte Message::getSession(){
    //session is always at the position 1
    char* data=_canMessage->getData();
    byte r=0;
    if (hasThisData(_opc,message_config_pos.SESSION)){
            r=data[1];
    }

    return r;

}

unsigned int Message::getEventNumber(){
    //node number is always at the position 3 and 4
    char* data=_canMessage->getData();
    unsigned int r=0;
    if (hasThisData(_opc,message_config_pos.EVENT_NUMBER)){
            r=(unsigned int)word(data[3],data[4]);
    }

    return r;
}

unsigned int Message::getDeviceNumber(){
    //node number is always at the position 3 and 4 with exception from the messages
    //OPC_DDRS OPC_DDES OPC_RQDDS
    char* data=_canMessage->getData();
    unsigned int r=0;
    if (hasThisData(_opc,message_config_pos.DEVICE_NUMBER)){
            if ((opc==OPC_DDRS) || (opc==OPC_DDES) || (opc==OPC_RQDDS)){
                r=(unsigned int)word(data[1],data[2]);
            }
            else {
                r=(unsigned int)word(data[3],data[4]);
            }
    }

    return r;
}

unsigned int Message::getDecoder(){
    //node number is always at the position 2 and 3
    char* data=_canMessage->getData();
    unsigned int r=0;
    if (hasThisData(_opc,message_config_pos.DECODER)){
            r=(unsigned int)word(data[2],data[3]);
    }

    return r;
}

unsigned int Message::getCV(){
    //node number is always at the position 2 and 3
    char* data=_canMessage->getData();
    unsigned int r=0;
    if (hasThisData(_opc,message_config_pos.CV)){
            r=(unsigned int)word(data[2],data[3]);
    }

    return r;
}

byte Message::getConsist(){
    char* data=_canMessage->getData();
    byte r=0;
    if (_opc==OPC_PCON || _opc==OPC_KCON){
        return data[2];
    }
    return r;
}

byte Message::getSpeedDir(){
    char* data=_canMessage->getData();
    byte r=0;
    if (_opc==OPC_DSPD){
        return data[2];
    }
    else if (_opc==OPC_PLOC){
        return data[4];
    }
    return r;

}

byte Message::functionNumber(){
    char* data=_canMessage->getData();
    byte r=0;
    if (_opc==OPC_DFNON||_opc==OPC_DFNOF||_opc==OPC_DFUN){
        return data[2];
    }
    return r;
}
byte Message::functionValue(){
    char* data=_canMessage->getData();
    byte r=0;
    if (_opc==OPC_DFUN){
        return data[3];
    }
    return r;
}

byte Message::getFunctionNumber(){
    char* data=_canMessage->getData();
    byte r=0;
    if (_opc==OPC_DFNON||_opc==OPC_DFNOF){
        return data[2];
    }
    return r;
}

byte Message::getStatus(){
    char* data=_canMessage->getData();
    byte r=0;
    if (_opc==OPC_SSTAT||_opc==OPC_DFNOF){
        return data[2];
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
    for(i=0;i<MSGSIZE,i++)
    {
        message_params[i]=0;
    }
    bitSet(message_params[OPC_KLOC],message_config_pos.SESSION);

    bitSet(message_params[OPC_QLOC],message_config_pos.SESSION);

    bitSet(message_params[OPC_DKEEP],message_config_pos.SESSION);

    //message_params[OPC_EXTC]=message_type.GENERAL;
    bitSet(message_params[OPC_RLOC],message_config_pos.DECODER);

    bitSet(message_params[OPC_SNN],message_config_pos.NODE_NUMBER);

    bitSet(message_params[OPC_STMOD],message_config_pos.SESSION);

    bitSet(message_params[OPC_PCON],message_config_pos.SESSION);

    bitSet(message_params[OPC_KCON],message_config_pos.SESSION);

    bitSet(message_params[OPC_DSPD],message_config_pos.SESSION);

    bitSet(message_params[OPC_DFLG],message_config_pos.SESSION);

    bitSet(message_params[OPC_DFNON],message_config_pos.SESSION);

    bitSet(message_params[OPC_DFNOF],message_config_pos.SESSION);

    bitSet(message_params[OPC_SSTAT],message_config_pos.SESSION);

    bitSet(message_params[OPC_RQNN],message_config_pos.NODE_NUMBER);

    bitSet(message_params[OPC_NNREL],message_config_pos.NODE_NUMBER);

    bitSet(message_params[OPC_NNACK],message_config_pos.NODE_NUMBER);

    bitSet(message_params[OPC_NNLRN],message_config_pos.NODE_NUMBER);

    bitSet(message_params[OPC_NNULN],message_config_pos.NODE_NUMBER);

    bitSet(message_params[OPC_NNCLR],message_config_pos.NODE_NUMBER);

    bitSet(message_params[OPC_NNEVN],message_config_pos.NODE_NUMBER);

    bitSet(message_params[OPC_NERD],message_config_pos.NODE_NUMBER);

    bitSet(message_params[OPC_RQEVN],message_config_pos.NODE_NUMBER);

    bitSet(message_params[OPC_WRACK],message_config_pos.NODE_NUMBER);

    bitSet(message_params[OPC_RQDAT],message_config_pos.NODE_NUMBER);

    bitSet(message_params[OPC_RQDDS],message_config_pos.DEVICE_NUMBER);

    bitSet(message_params[OPC_BOOT],message_config_pos.NODE_NUMBER);

    bitSet(message_params[OPC_ENUM],message_config_pos.NODE_NUMBER);


    //message_params[OPC_EXTC1]=message_type.GENERAL;

    bitSet(message_params[OPC_DFUN],message_config_pos.SESSION);

    bitSet(message_params[OPC_GLOC],message_config_pos.DECODER);

    //message_params[OPC_ERR]=message_type.DCC;

    bitSet(message_params[OPC_CMDERR],message_config_pos.NODE_NUMBER);

    bitSet(message_params[OPC_EVNLF],message_config_pos.NODE_NUMBER);

    bitSet(message_params[OPC_NVRD],message_config_pos.NODE_NUMBER);

    bitSet(message_params[OPC_NENRD],message_config_pos.NODE_NUMBER);

    bitSet(message_params[OPC_RQNPN],message_config_pos.NODE_NUMBER);

    bitSet(message_params[OPC_NUMEV],message_config_pos.NODE_NUMBER);

    bitSet(message_params[OPC_CANID],message_config_pos.NODE_NUMBER);

    //message_params[OPC_EXTC2]=message_type.GENERAL;

    bitSet(message_params[OPC_RDCC3],message_config_pos.SESSION);
    bitSet(message_params[OPC_RDCC3],message_config_pos.CV);

    bitSet(message_params[OPC_WCVO],message_config_pos.SESSION);
    bitSet(message_params[OPC_WCVO],message_config_pos.CV);

    bitSet(message_params[OPC_WCVB],message_config_pos.SESSION);
    bitSet(message_params[OPC_WCVB],message_config_pos.CV);

    bitSet(message_params[OPC_QCVS],message_config_pos.SESSION);
    bitSet(message_params[OPC_QCVS],message_config_pos.CV);

    bitSet(message_params[OPC_PCVS],message_config_pos.SESSION);
    bitSet(message_params[OPC_PCVS],message_config_pos.CV);

    bitSet(message_params[OPC_ACON],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_ACON],message_config_pos.EVENT_NUMBER);

    bitSet(message_params[OPC_ACOF],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_ACOF],message_config_pos.EVENT_NUMBER);

    bitSet(message_params[OPC_AREQ],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_AREQ],message_config_pos.EVENT_NUMBER);

    bitSet(message_params[OPC_AREQ],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_AREQ],message_config_pos.EVENT_NUMBER);

    bitSet(message_params[OPC_ARON],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_ARON],message_config_pos.EVENT_NUMBER);

    bitSet(message_params[OPC_AROF],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_AROF],message_config_pos.EVENT_NUMBER);

    bitSet(message_params[OPC_EVULN],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_EVULN],message_config_pos.EVENT_NUMBER);

    bitSet(message_params[OPC_NVSET],message_config_pos.NODE_NUMBER);

    bitSet(message_params[OPC_NVANS],message_config_pos.NODE_NUMBER);

    bitSet(message_params[OPC_ASON],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_ASON],message_config_pos.DEVICE_NUMBER);

    bitSet(message_params[OPC_ASOF],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_ASOF],message_config_pos.DEVICE_NUMBER);

    bitSet(message_params[OPC_ASRQ],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_ASRQ],message_config_pos.DEVICE_NUMBER);

    bitSet(message_params[OPC_PARAN],message_config_pos.NODE_NUMBER);

    bitSet(message_params[OPC_REVAL],message_config_pos.NODE_NUMBER);

    bitSet(message_params[OPC_ARSON],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_ARSON],message_config_pos.DEVICE_NUMBER);

    bitSet(message_params[OPC_ARSOF],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_ARSOF],message_config_pos.DEVICE_NUMBER);

    //message_params[OPC_EXTC3]=message_type.GENERAL;
    //message_params[OPC_RDCC4]=message_type.DCC;

    bitSet(message_params[OPC_WCVS],message_config_pos.SESSION);
    bitSet(message_params[OPC_WCVS],message_config_pos.CV);

    bitSet(message_params[OPC_ACON1],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_ACON1],message_config_pos.EVENT_NUMBER);

    bitSet(message_params[OPC_ACOF1],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_ACOF1],message_config_pos.EVENT_NUMBER);

    bitSet(message_params[OPC_REQEV],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_REQEV],message_config_pos.EVENT_NUMBER);

    bitSet(message_params[OPC_ARON1],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_ARON1],message_config_pos.EVENT_NUMBER);

    bitSet(message_params[OPC_AROF1],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_AROF1],message_config_pos.EVENT_NUMBER);

    bitSet(message_params[OPC_NEVAL],message_config_pos.NODE_NUMBER);

    bitSet(message_params[OPC_PNN],message_config_pos.NODE_NUMBER);

    bitSet(message_params[OPC_ASON1],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_ASON1],message_config_pos.DEVICE_NUMBER);

    bitSet(message_params[OPC_ASOF1],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_ASOF1],message_config_pos.DEVICE_NUMBER);

    bitSet(message_params[OPC_ARSON1],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_ARSON1],message_config_pos.DEVICE_NUMBER);

    bitSet(message_params[OPC_ARSOF1],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_ARSOF1],message_config_pos.DEVICE_NUMBER);

    //message_params[OPC_EXTC4]=message_type.GENERAL;
    //message_params[OPC_RDCC5]=message_type.DCC;

    bitSet(message_params[OPC_WCVOA],message_config_pos.DECODER);
    bitSet(message_params[OPC_WCVOA],message_config_pos.CV);

    //message_params[OPC_FCLK]=message_type.ACCESSORY;

    bitSet(message_params[OPC_ACON2],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_ACON2],message_config_pos.EVENT_NUMBER);

    bitSet(message_params[OPC_ACOF2],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_ACOF2],message_config_pos.EVENT_NUMBER);

    bitSet(message_params[OPC_EVLRN],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_EVLRN],message_config_pos.EVENT_NUMBER);

    bitSet(message_params[OPC_EVANS],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_EVANS],message_config_pos.EVENT_NUMBER);

    bitSet(message_params[OPC_ARON2],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_ARON2],message_config_pos.EVENT_NUMBER);

    bitSet(message_params[OPC_AROF2],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_AROF2],message_config_pos.EVENT_NUMBER);

    bitSet(message_params[OPC_ASON2],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_ASON2],message_config_pos.DEVICE_NUMBER);

    bitSet(message_params[OPC_ASOF2],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_ASOF2],message_config_pos.DEVICE_NUMBER);

    bitSet(message_params[OPC_ARSON2],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_ARSON2],message_config_pos.DEVICE_NUMBER);

    bitSet(message_params[OPC_ARSOF2],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_ARSOF2],message_config_pos.DEVICE_NUMBER);

    //message_params[OPC_EXTC5]=message_type.GENERAL;
    //message_params[OPC_RDCC6]=message_type.DCC;

    bitSet(message_params[OPC_PLOC],message_config_pos.SESSION;
    bitSet(message_params[OPC_PLOC],message_config_pos.DECODER);

    //message_params[OPC_NAME]=message_type.CONFIG;

    bitSet(message_params[OPC_STAT],message_config_pos.NODE_NUMBER);

    //message_params[OPC_PARAMS]=message_type.CONFIG;

    bitSet(message_params[OPC_ACON3],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_ACON3],message_config_pos.EVENT_NUMBER);

    bitSet(message_params[OPC_ACOF3],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_ACOF3],message_config_pos.EVENT_NUMBER);

    //message_params[OPC_ENRSP]=message_type.CONFIG;

    bitSet(message_params[OPC_ARON3],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_ARON3],message_config_pos.EVENT_NUMBER);

    bitSet(message_params[OPC_AROF3],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_AROF3],message_config_pos.EVENT_NUMBER);

    bitSet(message_params[OPC_EVLRNI],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_EVLRNI],message_config_pos.EVENT_NUMBER);

    bitSet(message_params[OPC_ACDAT],message_config_pos.NODE_NUMBER);

    bitSet(message_params[OPC_ARDAT],message_config_pos.NODE_NUMBER);

    bitSet(message_params[OPC_ASON3],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_ASON3],message_config_pos.DEVICE_NUMBER);

    bitSet(message_params[OPC_ASOF3],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_ASOF3],message_config_pos.DEVICE_NUMBER);

    bitSet(message_params[OPC_DDES],message_config_pos.DEVICE_NUMBER);

    bitSet(message_params[OPC_DDRS],message_config_pos.DEVICE_NUMBER);

    bitSet(message_params[OPC_ARSON3],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_ARSON3],message_config_pos.DEVICE_NUMBER);

    bitSet(message_params[OPC_ARSOF3],message_config_pos.NODE_NUMBER);
    bitSet(message_params[OPC_ARSOF3],message_config_pos.DEVICE_NUMBER);

    //message_params[OPC_EXTC6]=message_type.ACCESSORY;



}


unsigned int bitSet(unsigned int &val,unsigned int pos){
    return *val;
}

void Message::loadMessageType()
{
    //clear all types
    int i=0;
    for(i=0;i<MSGSIZE,i++)
    {
        messages[i]=message_type.RESERVED;
    }
    messages[OPC_ACK]=message_type.GENERAL;
    messages[OPC_NAK]=message_type.GENERAL;
    messages[OPC_HLT]=message_type.GENERAL;
    messages[OPC_BON]=message_type.GENERAL;
    messages[OPC_TOF]=message_type.DCC;
    messages[OPC_TON]=message_type.DCC;
    messages[OPC_ESTOP]=message_type.DCC;
    messages[OPC_ARST]=message_type.GENERAL;
    messages[OPC_RTOF]=message_type.DCC;
    messages[OPC_RTON]=message_type.DCC;
    messages[OPC_RESTP]=message_type.DCC;
    messages[OPC_RSTAT]=message_type.CONFIG;
    messages[OPC_QNN]=message_type.CONFIG;
    messages[OPC_RQNP]=message_type.CONFIG;
    messages[OPC_RQMN]=message_type.CONFIG;
    messages[OPC_KLOC]=message_type.DCC;
    messages[OPC_QLOC]=message_type.DCC;
    messages[OPC_DKEEP]=message_type.DCC;
    messages[OPC_EXTC]=message_type.GENERAL;
    messages[OPC_RLOC]=message_type.DCC;
    messages[OPC_QCON]=message_type.RESERVED;
    messages[OPC_SNN]=message_type.GENERAL;
    messages[OPC_STMOD]=message_type.DCC;
    messages[OPC_PCON]=message_type.DCC;
    messages[OPC_KCON]=message_type.DCC;
    messages[OPC_DSPD]=message_type.DCC;
    messages[OPC_DFLG]=message_type.DCC;
    messages[OPC_DFNON]=message_type.DCC;
    messages[OPC_DFNOF]=message_type.DCC;
    messages[OPC_SSTAT]=message_type.DCC;
    messages[OPC_RQNN]=message_type.CONFIG;
    messages[OPC_NNREL]=message_type.CONFIG;
    messages[OPC_NNACK]=message_type.CONFIG;
    messages[OPC_NNLRN]=message_type.CONFIG;
    messages[OPC_NNULN]=message_type.CONFIG;
    messages[OPC_NNCLR]=message_type.CONFIG;
    messages[OPC_NNEVN]=message_type.CONFIG;
    messages[OPC_NERD]=message_type.CONFIG;
    messages[OPC_RQEVN]=message_type.CONFIG;
    messages[OPC_WRACK]=message_type.CONFIG;
    messages[OPC_RQDAT]=message_type.ACCESSORY;
    messages[OPC_RQDDS]=message_type.ACCESSORY;
    messages[OPC_BOOT]=message_type.CONFIG;
    messages[OPC_ENUM]=message_type.CONFIG;
    messages[OPC_EXTC1]=message_type.GENERAL;
    messages[OPC_DFUN]=message_type.DCC;
    messages[OPC_GLOC]=message_type.DCC;
    messages[OPC_ERR]=message_type.DCC;
    messages[OPC_CMDERR]=message_type.CONFIG;
    messages[OPC_EVNLF]=message_type.CONFIG;
    messages[OPC_NVRD]=message_type.CONFIG;
    messages[OPC_NENRD]=message_type.CONFIG;
    messages[OPC_RQNPN]=message_type.CONFIG;
    messages[OPC_NUMEV]=message_type.CONFIG;
    messages[OPC_CANID]=message_type.CONFIG;
    messages[OPC_EXTC2]=message_type.GENERAL;
    messages[OPC_RDCC3]=message_type.DCC;
    messages[OPC_WCVO]=message_type.DCC;
    messages[OPC_WCVB]=message_type.DCC;
    messages[OPC_QCVS]=message_type.DCC;
    messages[OPC_PCVS]=message_type.DCC;
    messages[OPC_ACON]=message_type.ACCESSORY;
    messages[OPC_ACOF]=message_type.ACCESSORY;
    messages[OPC_AREQ]=message_type.ACCESSORY;
    messages[OPC_ARON]=message_type.ACCESSORY;
    messages[OPC_AROF]=message_type.ACCESSORY;
    messages[OPC_EVULN]=message_type.CONFIG;
    messages[OPC_NVSET]=message_type.CONFIG;
    messages[OPC_NVANS]=message_type.CONFIG;
    messages[OPC_ASON]=message_type.ACCESSORY;
    messages[OPC_ASOF]=message_type.ACCESSORY;
    messages[OPC_ASRQ]=message_type.ACCESSORY;
    messages[OPC_PARAN]=message_type.CONFIG;
    messages[OPC_REVAL]=message_type.CONFIG;
    messages[OPC_ARSON]=message_type.ACCESSORY;
    messages[OPC_ARSOF]=message_type.ACCESSORY;
    messages[OPC_EXTC3]=message_type.GENERAL;
    messages[OPC_RDCC4]=message_type.DCC;
    messages[OPC_WCVS]=message_type.DCC;
    messages[OPC_ACON1]=message_type.ACCESSORY;
    messages[OPC_ACOF1]=message_type.ACCESSORY;
    messages[OPC_REQEV]=message_type.CONFIG;
    messages[OPC_ARON1]=message_type.ACCESSORY;
    messages[OPC_AROF1]=message_type.ACCESSORY;
    messages[OPC_NEVAL]=message_type.CONFIG;
    messages[OPC_PNN]=message_type.GENERAL;
    messages[OPC_ASON1]=message_type.ACCESSORY;
    messages[OPC_ASOF1]=message_type.ACCESSORY;
    messages[OPC_ARSON1]=message_type.ACCESSORY;
    messages[OPC_ARSOF1]=message_type.ACCESSORY;
    messages[OPC_EXTC4]=message_type.GENERAL;
    messages[OPC_RDCC5]=message_type.DCC;
    messages[OPC_WCVOA]=message_type.DCC;
    messages[OPC_FCLK]=message_type.ACCESSORY;
    messages[OPC_ACON2]=message_type.ACCESSORY;
    messages[OPC_ACOF2]=message_type.ACCESSORY;
    messages[OPC_EVLRN]=message_type.CONFIG;
    messages[OPC_EVANS]=message_type.CONFIG;
    messages[OPC_ARON2]=message_type.ACCESSORY;
    messages[OPC_AROF2]=message_type.ACCESSORY;
    messages[OPC_ASON2]=message_type.ACCESSORY;
    messages[OPC_ASOF2]=message_type.ACCESSORY;
    messages[OPC_ARSON2]=message_type.ACCESSORY;
    messages[OPC_ARSOF2]=message_type.ACCESSORY;
    messages[OPC_EXTC5]=message_type.GENERAL;
    messages[OPC_RDCC6]=message_type.DCC;
    messages[OPC_PLOC]=message_type.DCC;
    messages[OPC_NAME]=message_type.CONFIG;
    messages[OPC_STAT]=message_type.DCC;
    messages[OPC_PARAMS]=message_type.CONFIG;
    messages[OPC_ACON3]=message_type.ACCESSORY;
    messages[OPC_ACOF3]=message_type.ACCESSORY;
    messages[OPC_ENRSP]=message_type.CONFIG;
    messages[OPC_ARON3]=message_type.ACCESSORY;
    messages[OPC_AROF3]=message_type.ACCESSORY;
    messages[OPC_EVLRNI]=message_type.CONFIG;
    messages[OPC_ACDAT]=message_type.ACCESSORY;
    messages[OPC_ARDAT]=message_type.ACCESSORY;
    messages[OPC_ASON3]=message_type.ACCESSORY;
    messages[OPC_ASOF3]=message_type.ACCESSORY;
    messages[OPC_DDES]=message_type.ACCESSORY;
    messages[OPC_DDRS]=message_type.ACCESSORY;
    messages[OPC_ARSON3]=message_type.ACCESSORY;
    messages[OPC_ARSOF3]=message_type.ACCESSORY;
    messages[OPC_EXTC6]=message_type.ACCESSORY;
}

