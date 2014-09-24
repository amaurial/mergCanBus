#include "MessageParser.h"



MergCanBus::MessageParser()
{
    //ctor
    loadMessageType();
    skipReservedMessage();
}

MergCanBus::~MessageParser()
{
    //dtor
}



bool MergCanBus::skipMessage(message_type msg){
    switch (msg){
        case message_type.ACCESSORY:
            setBitMessage (message_type.ACCESSORY,true)
            break;
        case message_type.DCC:
            setBitMessage (message_type.DCC,true)
            break;
        case message_type.GENERAL:
            setBitMessage (message_type.GENERAL,true)
            break;
        case message_type.CONFIG:
            setBitMessage (message_type.CONFIG,true)
            break;
        case message_type.RESERVED:
            setBitMessage (message_type.RESERVED,true)
            break;
        default:
            return true;
    }


}

void MergCanBus::getParamSpecific(){
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

