#include "MessageParser.h"



MessageParser::MessageParser()
{
    //ctor
    loadMessageType();
    skipReservedMessage();
}

MessageParser::~MessageParser()
{
    //dtor
}
bool MessageParser::parse(Message *message,CANMessage *canMessage)
{
    _canMessage=canMessage;
    _message=message;
    getCanId();
    getRTR();
    //message is a request CAN RTR
    if (_message->getRTR()==1){
        return true;
    }
    getOpc();
    _message->setType(messages[_message->getOpc()]);
    //skip message types
    if (skipMessage(_message->getType())){
        return true;
    }

    getDataSize();
    if (_message->getNumBytes()==0){
        //nothing to do
        //all fields were set
        return true;
    }

    switch (_message->getType()){
        case message_type.RESERVED:
            return true;
            break;
        case message_type.ACCESSORY:
            parseACCMessage();
            break;
        case message_type.DCC:
            break;
        case message_type.CONFIG:
            break;
        case message_type.GENERAL:
            break;
        }
    }

    if (_message->getNumBytes()>0){
        getNodeNumber();
        getEventNumber();
    }
    //TODO:get the parameter from the message according to the message type

    //getPriority();

    return true;
}

int MessageParser::parseACCMessage(){
    //[TODO: not robust enough. can break easily]
    //the message types has to be transparent for the end user
    _accMessage=(AccessoryMessage*)_message;
    byte *data=_canMessage->getData();
    byte opc=_accMessage->getOpc();

    switch (opc){
        case (OPC_FCLK):
            _accMessage->setMinutes(data[1]);
            _accMessage->setHours(data[2]);
            _accMessage->setWdmon(data[3]);
            _accMessage->setDiv(data[4]);
            _accMessage->setMday(data[5]);
            _accMessage->setTemperature(data[6]);
        break;
        default:
            //messages with device number
            if ((opc==OPC_ASON2)|| (opc==OPC_ASOF2)|| (opc==OPC_ARSON2)|| (opc==OPC_ARSOF2)||
                (opc==OPC_ASON3)|| (opc==OPC_ASOF3)|| (opc==OPC_ARSON3)|| (opc==OPC_ARSOF3)||
                (opc==OPC_DDES)|| (opc==OPC_DDRS)){

                    if ((opc==OPC_DDES)|| (opc==OPC_DDRS)){
                        _accMessage->setDeviceNumber(data[1],0);
                        _accMessage->setDeviceNumber(data[2],1);
                    }
                    else {
                        _accMessage->setDeviceNumber(data[3],0);
                        _accMessage->setDeviceNumber(data[4],1);
                    }
            }

            //messages with 2 bytes of data
            if (_accMessage->getNumBytes()==6){
                //all messages have NN NN EN EN d1 d2
                _accMessage->setTotalAddBytes(2);
                _accMessage->setAdditionalByte(data[5],0);
                _accMessage->setAdditionalByte(data[6],1);
            }

            if (_accMessage->getNumBytes()==7){
                 //messages with 5 bytes of data
                if ((opc==OPC_ACDAT)|| (opc==OPC_ARDAT)|| (opc==OPC_ARSON2)|| (opc==OPC_ARSOF2)||
                    (opc==OPC_DDES)|| (opc==OPC_DDRS)){
                    _accMessage->setTotalAddBytes(5);
                    _accMessage->setAdditionalByte(data[3],0);
                    _accMessage->setAdditionalByte(data[4],1);
                    _accMessage->setAdditionalByte(data[5],2);
                    _accMessage->setAdditionalByte(data[6],3);
                    _accMessage->setAdditionalByte(data[7],4);
                }
                 //messages with 3 bytes of data
                else {
                    //all messages have NN NN EN EN d1 d2
                    _accMessage->setTotalAddBytes(3);
                    _accMessage->setAdditionalByte(data[5],0);
                    _accMessage->setAdditionalByte(data[6],1);
                    _accMessage->setAdditionalByte(data[7],2);
                }
            }
        break;
    }
}

bool MessageParser::getCanId()
{
  _message->setCanId( _canMessage->getCanId());
  return true;
}

/***
* first byte contains the opc and the number of bytes in the message
* the first 3 bits are the number of bytes
* the 5 last bits are the OPC
**/
bool MessageParser::getDataSize()
{
    _message->setNumBytes(_canMessage->getDataSize());
    return true;
}
/**
* first byte contains the opc and the number of bytes in the message
* the first 3 bits are the number of bytes
* the 5 last bits are the OPC
**/
bool MessageParser::getOpc()
{
    _message->setOpc(_canMessage->getOpc());
    return true;
}

bool MessageParser::getRTR()
{
    if (_canMessage->getRTR()==1){_message->setRTR();}
    else{_message->unsetRTR();}
}

/**
* bytes 1 and 2 from data buffer can be, depending on the message, the node number
*/
bool MessageParser::getNodeNumber(Message *_msg)
{
    int nn=0;
    byte* data;
    if (_message->getNumBytes()>1){
        data=_message->getData();
        nn=data[1];
        nn=(nn<<8)|data[2];
        _message->setNodeNumber(nn);
    }
    else{_message->setNodeNumber(0x0000);}

}
/**
* bytes 3 and 4 from data buffer can be, depending on the message, the event number
*/
bool MessageParser::getEventNumber()
{
    int en=0;
    byte* data;
    if (_message->getNumBytes()>3){
        data=_message->getData();
        en=data[3];
        en=(en<<8)|data[4];
        _message->setNodeNumber(en);
    }
    else{_message->setEventNumber(0x0000);}

}

bool MessageParser::skipMessage(message_type msg){
    switch (msg){
        case message_type.ACCESSORY:
            return _skipACCESSORY;
            break;
        case message_type.DCC:
            return _skipDCC;
            break;
        case message_type.GENERAL:
            return _skipGENERAL;
            break;
        case message_type.CONFIG:
            return _skipCONFIG;
            break;
        case message_type.RESERVED:
            return _skipRESERVED;
            break;
        default:
            return true;
    }


}

void MessageParser::getParamSpecific(){
    unsigned int opc=0;
    unsigned int msize=0;
    byte *data;
    switch (_message->getType()){
    case DCC:
        opc=_message->getOpc();
        data=_message->getData();

        if (opc>=OPC_KLOC && opc<=OPC_DKEEP){
            //messages with session parameter
            _message->setSession(_data[1]);
        }else if(opc==OPC_RLOC){
            //get the decoder address
            _message->setDecoder(_data[1],data[2]);
        }else if (opc>=OPC_STMOD && opc<=OPC_SSTAT){
            _message->setSession(_data[1]);
            _message->setEngineParameter(data[2]);
        }
        break;
    case GENERAL:
        break;
    case ACCESSORY:
        break;
    case CONFIG:
        break;
    case RESERVED:
        break;

    }


}

void MessageParser::loadMessageType()
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

