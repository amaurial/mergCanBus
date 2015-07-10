#include "Message.h"


/**
* Constructor
* Clear the internal buffers and load the basic configuration.
*/

Message::Message()
{
    //ctor

    clear();
    loadMessageType();
    loadMessageConfig();

}

/**
* Creates a message to be sent. Not being used.
* NOT USED AT THIS VERSION.
* \param canId is the can id of the message.
* \param opc is the opc code.
* \param nodeNumber is the node number.
* \param eventNumber is the event number.
* \param data is the message data itself.
* \param priority is CAN priority.
*/

Message::Message(unsigned int canId,
                 unsigned int opc,
                 unsigned int nodeNumber,
                 unsigned int eventNumber,
                 byte data [CANDATA_SIZE] ,
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
    for (i=0;i<CANDATA_SIZE;i++){data[i]=data[i];}
}

/**
* Destructor
*/
Message::~Message()
{
    //dtor
}

/**
* Get the message size. Extract it from the can frame.
* @return message size.
*/
byte Message::getCanMessageSize(){

    return canMsgSize;
}

/**
* Get the CBUS message size. Extract it from the opc.
* @return message size.
*/
byte Message::getMessageSize(){
    msgSize=data[0]>>5;
    return msgSize;
}

/**
* Get the Can id from the header.
* @return message size.
*/
byte Message::getCanId(){
  canId=0;
  canId=(header[0]<<4)&0xF0;
  canId=(canId)|(header[1]>>5);
  return canId;

}
/**
* first byte contains the opc and the number of bytes in the message
* the first 3 bits are the number of bytes
* the 5 last bits are the OPC
**/
byte Message::getOpc()
{
    return opc;
}
/**
* Set the can buffer.
* @param val A 8 bytes array.
*/
void Message::setDataBuffer(byte val[CANDATA_SIZE] )
{
    int i=0;
    for (i=0;i<CANDATA_SIZE;i++){data[i]=val[i];}
    setOpc(data[0]);
    setType(messages[getOpc()]);
}

/**
* Set the header buffer.
* @param val A 8 bytes array.
*/
void Message::setHeaderBuffer(byte val[HEADER_SIZE] )
{
    int i=0;
    for (i=0;i<HEADER_SIZE;i++){header[i]=val[i];}

}

/**
* Clear the internal structure.
*/
void Message::clear(){
    //setCanId(0);
    //setOpc(0);
    //setType(RESERVED);
    //setEventNumber(0);
    //setNodeNumber(0);
    //setData((byte*)"00000000");
    for (int i=0;i<CANDATA_SIZE;i++){
        data[i]=0;
    }
    for (int i=0;i<HEADER_SIZE;i++){
        header[i]=0;
    }
    setPriority(0);
    setNumBytes(0);
    unsetRTR();
    //setSession(0);
    //setDecoder(0);
    //setDeviceNumber(0);

}


/**
* Used to get the data fields directly
* @param pos Byte position.
*/
byte Message::getByte(byte pos){
    if (pos>=CANDATA_SIZE){
        return 0;
    }
    if (getOpc()==0){
        return 0;
    }
    return data[pos];

}

/**
* Get the node number
* @return a 16 bit integer.
*/
unsigned int Message::getNodeNumber(){
    //node number is always at the position 1 and 2
    //byte* data=_canMessage->getData();
    unsigned int r=0;
    #ifdef DEBUGDEF
        Serial.println("Message::getNodeNumber - Getting NN");
        Serial.print("OPC:");
        Serial.println(data[0],HEX);
    #endif // DEBUGDEF

    if (hasThisData(data[0],NODE_NUMBER)){
            #ifdef DEBUGDEF
                Serial.println("NN OK");
            #endif // DEBUGDEF
            r=data[1];
            r=r<<8;
            r=r|data[2];
            //r=(unsigned int)word(data[1],data[2]);
    }
    return r;
}

/**
* Get the loc session for DCC messages
* @return a byte.
*/
byte Message::getSession(){
    //session is always at the position 1
    //byte* data=_canMessage->getData();
    byte r=0;
    if (hasThisData(opc,SESSION)){
            r=data[1];
    }

    return r;

}
/**
* Get the event number
* @return a 16 bit integer.
*/
unsigned int Message::getEventNumber(){
    //node number is always at the position 3 and 4
    //byte* data=_canMessage->getData();
    unsigned int r=0;
    if (hasThisData(opc,EVENT_NUMBER)){
            r=data[3];
            r=r<<8;
            r=r|data[4];
            //r=(unsigned int)word(data[3],data[4]);
    }

    return r;
}
/**
* Get the device number
* @return a 16 bit integer.
*/
unsigned int Message::getDeviceNumber(){
    //node number is always at the position 3 and 4 with exception from the messages
    //OPC_DDRS OPC_DDES OPC_RQDDS
    //byte* data=_canMessage->getData();
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
/**
* Get the decoder id for DCC messages
* @return a 2 byte integer.
*/
unsigned int Message::getDecoder(){
    //node number is always at the position 2 and 3
    //byte* data=_canMessage->getData();
    unsigned int r=0;
    if (hasThisData(opc,DECODER)){
            //r=(unsigned int)word(data[2],data[3]);
            r=data[2];
            r=r<<8;
            r=r|data[3];
    }

    return r;
}

/**
* Get the decoder id for DCC messages
* @return a 2 byte integer.
*/
unsigned int Message::getCV(){
    //cv number is always at the position 2 and 3
    //byte* data=_canMessage->getData();
    unsigned int r=0;
    if (hasThisData(opc,CV)){
            //r=(unsigned int)word(data[2],data[3]);
            r=data[2];
            r=r<<8;
            r=r|data[3];
    }

    return r;
}


/**
* Get the cv value for DCC messages
* @return a 1 byte integer.
*/
unsigned int Message::getCVValue(){

    //byte* data=_canMessage->getData();
    byte r=0;
    if (opc==OPC_WCVO || opc==OPC_WCVB || opc==OPC_QCVS || opc==OPC_PCVS){
        return data[4];
    }
    else if (opc==OPC_WCVS){
        return data[5];
    }
    return r;
}


/**
* Get the cv mode value for DCC messages
* @return a 1 byte integer.
*/
unsigned int Message::getCVMode(){
    //node number is always at the position 2 and 3
    //byte* data=_canMessage->getData();
    byte r=0;
    if (opc==OPC_WCVS){
        return data[4];
    }
    return r;
}

/**
* Get the consist value for DCC messages
* @return a 1 byte integer.
*/

byte Message::getConsist(){
    //byte* data=_canMessage->getData();
    byte r=0;
    if (opc==OPC_PCON || opc==OPC_KCON){
        return data[2];
    }
    return r;
}

/**
* Get the speed direction for DCC messages
* @return a 1 byte integer.
*/
byte Message::getSpeedDir(){
    //byte* data=_canMessage->getData();
    byte r=0;
    if (opc==OPC_DSPD){
        return data[2];
    }
    else if (opc==OPC_PLOC){
        return data[4];
    }
    return r;

}
/**
* Get the engine flags for DCC messages
* @return a 1 byte integer.
*/
byte Message::getEngineFlag(){
    //byte* data=_canMessage->getData();
    byte r=0;
    if (opc==OPC_DFLG ){
        return data[2];
    }
    else if (opc==OPC_GLOC){
        return data[3];
    }
    return r;

}

/**
* Get the space left to store events
* @return a 1 byte integer.
*/
byte Message::getAvailableEventsLeft(){
    //byte* data=_canMessage->getData();
    byte r=0;
    if (opc==OPC_EVNLF ){
        return data[3];
    }
    return r;

}
/**
* Get the amount of stored events
* @return a 1 byte integer.
*/
byte Message::getStoredEvents(){
    //byte* data=_canMessage->getData();
    byte r=0;
    if (opc==OPC_NUMEV ){
        return data[3];
    }
    return r;

}

/**
* Get the function number Fn for DCC messages
* @return a 1 byte integer.
*/
byte Message::getFunctionNumber(){
    //byte* data=_canMessage->getData();
    byte r=0;
    if (opc==OPC_DFNON||opc==OPC_DFNOF||opc==OPC_DFUN){
        return data[2];
    }
    return r;
}
/**
* Get the function value for DCC messages
* @return a 1 byte integer.
*/
byte Message::functionValue(){
    //byte* data=_canMessage->getData();
    byte r=0;
    if (opc==OPC_DFUN){
        return data[3];
    }
    return r;
}

/**
* Get the status field
* @return a 1 byte integer.
*/
byte Message::getStatus(){
    //byte* data=_canMessage->getData();
    byte r=0;
    if (opc==OPC_SSTAT||opc==OPC_DFNOF){
        return data[2];
    }
    return r;
}
/**
* Get the parameter index Para# field
* @return a 1 byte integer.
*/
byte Message::getParaIndex(){
    //byte* data=_canMessage->getData();

    //Serial.print("OPC:");Serial.print(opc,HEX);
    //Serial.print("\tPara index byte: ");

    byte r=0;
    if (opc==OPC_RQNPN){
        //Serial.println(data[3],HEX);
        return data[3];
    }
    //Serial.println(0,HEX);
    return r;
}

/**
* Get the parameter Para field
* @return a 1 byte integer.
*/
byte Message::getParameter(){
    //byte* data=_canMessage->getData();
    byte r=0;
    if (opc==OPC_NVSET||opc==OPC_NVANS){
        return data[4];
    }
    return r;
}
/**
* Get the node variable index NV# field
* @return a 1 byte integer.
*/
byte Message::getNodeVariableIndex(){
    //byte* data=_canMessage->getData();
    byte r=0;
    if (opc==OPC_NVRD || opc==OPC_NVSET || opc==OPC_NVANS){
        return data[3];
    }
    return r;
}
/**
* Get the node variable NV field
* @return a 1 byte integer.
*/
byte Message::getNodeVariable(){
    //byte* data=_canMessage->getData();
    byte r=0;
    if (opc==OPC_NVSET||opc==OPC_NVANS){
        return data[4];
    }
    return r;
}
/**
* Get the event index EN# field
* @return a 1 byte integer.
*/
byte Message::getEventIndex(){
    //byte* data=_canMessage->getData();
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

/**
* Get the event variable index EV# field
* @return a 1 byte integer.
*/
byte Message::getEventVarIndex(){
    //byte* data=_canMessage->getData();
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
/**
* Get the event variable EV field
* @return a 1 byte integer.
*/
byte Message::getEventVar(){
    //byte* data=_canMessage->getData();
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

/**
* Check the filter if a specific information is expected in the message.
* @param opc The message id opc
* @param pos Which field to look
*/

bool Message::hasThisData(byte opc, message_config_pos pos){
    byte topc=getOpc();
    if (!((topc>=0) && (topc<MSGSIZE))){
        return false;
    }

    if (bitRead(message_params[topc],pos)==1){
        return true;
    }
    else {
        return false;
    }

}

/**
* Check the if it is an ON message. Major event in CBUS.
* @return True if is and On event, false if not
*/
bool Message::isAccOn(){

    if (opc==OPC_ACON ||
        opc==OPC_ACON1 ||
        opc==OPC_ACON2 ||
        opc==OPC_ACON3 ||
        opc==OPC_ASON ||
        opc==OPC_ASON1 ||
        opc==OPC_ASON2 ||
        opc==OPC_ASON3){
        return true;
    }
        return false;
}
/**
* Check the if it is an OFF message. Major event in CBUS.
* @return True if is and OFF event, false if not
*/
bool Message::isAccOff(){

    if (opc==OPC_ACOF ||
        opc==OPC_ACOF1 ||
        opc==OPC_ACOF2 ||
        opc==OPC_ACOF3||
        opc==OPC_ASOF ||
        opc==OPC_ASOF1 ||
        opc==OPC_ASOF2 ||
        opc==OPC_ASOF3){
        return true;
    }
        return false;
}

/**
* Check the if it is an ON message. Major event in CBUS.
* @return True if is and On event, false if not
*/
bool Message::isShortEvent(){

    if (opc==OPC_ASON ||
        opc==OPC_ASON1 ||
        opc==OPC_ASON2 ||
        opc==OPC_ASON3 ||
        opc==OPC_ASOF  ||
        opc==OPC_ASOF1 ||
        opc==OPC_ASOF2 ||
        opc==OPC_ASOF3){
        return true;
    }
        return false;
}
/**
* Check the if it is an OFF message. Major event in CBUS.
* @return True if is and OFF event, false if not
*/
bool Message::isLongEvent(){

    if (opc==OPC_ACON ||
        opc==OPC_ACON1 ||
        opc==OPC_ACON2 ||
        opc==OPC_ACON3 ||
        opc==OPC_ACOF ||
        opc==OPC_ACOF1 ||
        opc==OPC_ACOF2 ||
        opc==OPC_ACOF3){
        return true;
    }
        return false;
}


/**
* Return how many bytes of extra data has the ON event.
* @return The number of extra bytes depending on the message type. ACON,ACOF=0 ; ACON1,ACOF1=1; ACON2,ACOF2=2; ACON3,ACOF1=3
*/
byte Message::accExtraData(){

    if (opc==OPC_ACON || opc==OPC_ACOF){
        return 0;
    }
    if (opc==OPC_ACON1 || opc==OPC_ACOF1){
        return 1;
    }
    if (opc==OPC_ACON2 || opc==OPC_ACOF2){
        return 2;
    }
    if (opc==OPC_ACON3 || opc==OPC_ACOF3){
        return 3;
    }

    return 0;
}

/**
* Get the extra data byte on an ON or OFF event.
* @return Return the extra byte. The index is between 1 and 3
*/
byte Message::getAccExtraData(byte idx){

    if (isAccOff()||isAccOn()){

        if (idx==0 || idx>accExtraData()){
            return 0;
        }
        else{
            return data[4+idx];
        }
    }
    return 0;
}
/** \brief Create a ON event message
 *
 * \param nodeNumber Node number of the message
 * \param longEvent Indication of Long or Short event. True means long event.
 * \param eventNumber Event number
 * \param numDataBytes Number of extra bytes in the message. It defines the type of On event should be created:
 * 0 means +ACON or ASON
 * 1 means +ACON1 or ASON1
 * 2 means +ACON2 or ASON2
 * 3 means +ACON3 or ASON3
 * \param msgdata The extra data to be added according to the Number of extra bytes. There is no check if the msgdata is set or not.
 * So be sure to set it if the number of extra bytes > 1.
 * \return
 *
 */

void Message::createOnEvent(unsigned int nodeNumber,bool longEvent,unsigned int eventNumber,byte numDataBytes,byte* msgdata){


    switch (numDataBytes){
    case (0):
        if (longEvent){
            data[0]=OPC_ACON;
        }else {
            data[0]=OPC_ASON;
        }
        break;
    case (1):
        if (msgdata!=0){
            data[5]=msgdata[0];
        }
        if (longEvent){
            data[0]=OPC_ACON1;
        }else {
            data[0]=OPC_ASON1;
        }
        break;
    case(2):
        if (msgdata!=0){
            data[5]=msgdata[0];
            data[6]=msgdata[1];
        }
        if (longEvent){
            data[0]=OPC_ACON2;
        }else {
            data[0]=OPC_ASON2;
        }
        break;
    case(3):
        if (msgdata!=0){
            data[5]=msgdata[0];
            data[6]=msgdata[1];
            data[7]=msgdata[2];
        }
        if (longEvent){
            data[0]=OPC_ACON3;
        }else {
            data[0]=OPC_ASON3;
        }
        break;
    default:
        return;
        break;
    }
    data[1]=highByte(nodeNumber);
    data[2]=lowByte(nodeNumber);
    data[3]=highByte(eventNumber);
    data[4]=lowByte(eventNumber);

}
/** \brief Create a OFF event message
 *
 * \param nodeNumber Node number of the message
 * \param longEvent Indication of Long or Short event. True means long event.
 * \param eventNumber Event number
 * \param numDataBytes Number of extra bytes in the message. It defines the type of On event should be created:
 * 0 means +ACOF or ASOF
 * 1 means +ACOF1 or ASOF1
 * 2 means +ACOF2 or ASOF2
 * 3 means +ACOF3 or ASOF3
 * \param msgdata The extra data to be added according to the Number of extra bytes. There is no check if the msgdata is set or not.
 * So be sure to set it if the number of extra bytes > 1.
 * \return
 *
 */
void Message::createOffEvent(unsigned int nodeNumber,bool longEvent,unsigned int eventNumber,byte numDataBytes,byte* msgdata){
    switch (numDataBytes){
    case (0):
        if (longEvent){
            data[0]=OPC_ACOF;
        }else {
            data[0]=OPC_ASOF;
        }
        break;
    case (1):
        if (msgdata!=0){
            data[5]=msgdata[0];
        }

        if (longEvent){
            data[0]=OPC_ACOF1;
        }else {
            data[0]=OPC_ASOF1;
        }
        break;
    case(2):
        if (msgdata!=0){
            data[5]=msgdata[0];
            data[6]=msgdata[1];
        }

        if (longEvent){
            data[0]=OPC_ACOF2;
        }else {
            data[0]=OPC_ASOF2;
        }
        break;
    case(3):
        if (msgdata!=0){
            data[5]=msgdata[0];
            data[6]=msgdata[1];
            data[7]=msgdata[2];
        }

        if (longEvent){
            data[0]=OPC_ACOF3;
        }else {
            data[0]=OPC_ASOF3;
        }
        break;
    default:
        return;
        break;
    }
    data[1]=highByte(nodeNumber);
    data[2]=lowByte(nodeNumber);
    data[3]=highByte(eventNumber);
    data[4]=lowByte(eventNumber);
}

/**
* Load the most commom fields for each message.
* Used to make the search for fields faster.
*/
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

/**
* Index the message type to the OPC.
*/

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
    messages[OPC_SNN]=CONFIG;
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
    messages[OPC_RST]=GENERAL;
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


