#include "Message.h"


/**
* Constructor
* Clear the internal buffers and load the basic configuration.
*/

Message::Message()
{
    clear();
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
                 uint8_t opc,
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
    uint8_t i=0;
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
    uint8_t i=0;
    for (i=0;i<CANDATA_SIZE;i++){data[i]=val[i];}
    setOpc(data[0]);
    setType(findType(getOpc()));
}

/**
* Set the header buffer.
* @param val A 8 bytes array.
*/
void Message::setHeaderBuffer(byte val[HEADER_SIZE] )
{
    uint8_t i=0;
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
    //setData((byte*)"00000000"));
    for (uint8_t i=0;i<CANDATA_SIZE;i++){
        data[i]=0;
    }
    for (uint8_t i=0;i<HEADER_SIZE;i++){
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
        Serial.println(F("Message::getNodeNumber - Getting NN"));
        Serial.print(F("OPC:"));
        Serial.println(data[0],HEX);
    #endif // DEBUGDEF

    if (hasThisData(data[0],NODE_NUMBER)){
            #ifdef DEBUGDEF
                Serial.println(F("NN OK"));
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

    //Serial.print(F("OPC:"));Serial.print(opc,HEX);
    //Serial.print(F("\tPara index byte: "));

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

    switch (pos){
    case (NODE_NUMBER):
        if (opc==OPC_SNN || opc==OPC_RQNN || opc==OPC_NNREL || opc==OPC_NNACK || opc==OPC_NNLRN ||
            opc==OPC_NNULN || opc==OPC_NNCLR || opc==OPC_NNEVN || opc==OPC_NERD || opc==OPC_RQEVN ||
            opc==OPC_WRACK || opc==OPC_RQDAT || opc==OPC_BOOT || opc==OPC_ENUM || opc==OPC_CMDERR ||
            opc==OPC_EVNLF || opc==OPC_NVRD || opc==OPC_NENRD || opc==OPC_RQNPN || opc==OPC_NUMEV ||
            opc==OPC_CANID || opc==OPC_ACON || opc==OPC_ACOF || opc==OPC_AREQ || opc==OPC_AREQ ||
            opc==OPC_ARON || opc==OPC_AROF || opc==OPC_EVULN || opc==OPC_NVSET || opc==OPC_NVANS ||
            opc==OPC_ASON || opc==OPC_ASOF || opc==OPC_ASRQ || opc==OPC_PARAN || opc==OPC_REVAL ||
            opc==OPC_ARSON || opc==OPC_ARSOF || opc==OPC_ACON1 || opc==OPC_ACOF1 || opc==OPC_REQEV ||
            opc==OPC_ARON1 || opc==OPC_AROF1 || opc==OPC_NEVAL || opc==OPC_PNN || opc==OPC_ASON1 ||
            opc==OPC_ASOF1 || opc==OPC_ARSON1 || opc==OPC_ARSOF1 || opc==OPC_ACON2 || opc==OPC_ACOF2 ||
            opc==OPC_EVLRN || opc==OPC_EVANS || opc==OPC_ARON2 || opc==OPC_AROF2 || opc==OPC_ASON2 ||
            opc==OPC_ASOF2 || opc==OPC_ARSON2 || opc==OPC_ARSOF2 || opc==OPC_STAT || opc==OPC_ACON3 ||
            opc==OPC_ACOF3 || opc==OPC_ARON3 || opc==OPC_AROF3 || opc==OPC_EVLRNI || opc==OPC_ACDAT ||
            opc==OPC_ARDAT || opc==OPC_ASON3 || opc==OPC_ASOF3 || opc==OPC_ARSON3 || opc==OPC_ARSOF3)
        {
            return true;
        }
        else{
            return false;
        }
        break;
    case (EVENT_NUMBER):
        if (opc==OPC_ACON || opc==OPC_ACOF || opc==OPC_AREQ || opc==OPC_AREQ || opc==OPC_ARON || opc==OPC_AROF ||
            opc==OPC_EVULN || opc==OPC_ACON1 || opc==OPC_ACOF1 || opc==OPC_REQEV || opc==OPC_ARON1 ||
            opc==OPC_AROF1 || opc==OPC_ACON2 || opc==OPC_ACOF2 || opc==OPC_EVLRN || opc==OPC_EVANS ||
            opc==OPC_ARON2 || opc==OPC_AROF2 || opc==OPC_ACON3 || opc==OPC_ACOF3 || opc==OPC_ARON3 ||
            opc==OPC_AROF3 || opc==OPC_EVLRNI){
            return true;
        }
        else{
            return false;
        }
        break;
    case (DEVICE_NUMBER):
        if (opc==OPC_RQDDS || opc==OPC_ASON || opc==OPC_ASOF || opc==OPC_ASRQ || opc==OPC_ARSON ||
            opc==OPC_ARSOF || opc==OPC_ASON1 || opc==OPC_ASOF1 || opc==OPC_ARSON1 || opc==OPC_ARSOF1 ||
            opc==OPC_ASON2 || opc==OPC_ASOF2 || opc==OPC_ARSON2 || opc==OPC_ARSOF2 || opc==OPC_ASON3 ||
            opc==OPC_ASOF3 || opc==OPC_DDES || opc==OPC_DDRS || opc==OPC_ARSON3 || opc==OPC_ARSOF3){
            return true;
        }
        else{
            return false;
        }
        break;
    case (SESSION):
        if (opc==OPC_KLOC || opc==OPC_QLOC || opc==OPC_DKEEP || opc==OPC_STMOD || opc==OPC_PCON || opc==OPC_KCON ||
            opc==OPC_DSPD || opc==OPC_DFLG || opc==OPC_DFNON || opc==OPC_DFNOF || opc==OPC_SSTAT || opc==OPC_DFUN ||
            opc==OPC_RDCC3 || opc==OPC_WCVO || opc==OPC_WCVB || opc==OPC_QCVS || opc==OPC_PCVS || opc==OPC_WCVS || opc==OPC_PLOC){
            return true;
        }
        else{
            return false;
        }
        break;
    case (DECODER):
        if (opc==OPC_RLOC || opc==OPC_GLOC || opc==OPC_WCVOA || opc==OPC_PLOC){
            return true;
        }
        else{
            return false;
        }
        break;
    case (CV):
        if (opc==OPC_RDCC3 || opc==OPC_WCVO || opc==OPC_WCVB || opc==OPC_QCVS || opc==OPC_PCVS || opc==OPC_WCVS || opc==OPC_WCVOA){
            return true;
        }
        else{
            return false;
        }
        break;
    }
    return false;
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

message_type Message::findType(byte opc){

    if (opc==OPC_ACK || opc==OPC_NAK || opc==OPC_HLT ||
        opc==OPC_BON || opc==OPC_ARST || opc==OPC_EXTC ||
        opc==OPC_RST || opc==OPC_EXTC1 || opc==OPC_EXTC2 ||
        opc==OPC_EXTC3 || opc==OPC_PNN || opc==OPC_EXTC4 ||
        opc==OPC_EXTC5)
    {
        return GENERAL;
    }
    if (opc==OPC_TOF || opc==OPC_TON || opc==OPC_ESTOP ||
        opc==OPC_RTOF || opc==OPC_RTON || opc==OPC_RESTP ||
        opc==OPC_KLOC || opc==OPC_QLOC || opc==OPC_DKEEP ||
        opc==OPC_RLOC || opc==OPC_STMOD || opc==OPC_PCON ||
        opc==OPC_KCON || opc==OPC_DSPD || opc==OPC_DFLG ||
        opc==OPC_DFNON || opc==OPC_DFNOF || opc==OPC_SSTAT ||
        opc==OPC_DFUN || opc==OPC_GLOC || opc==OPC_ERR ||
        opc==OPC_RDCC3 || opc==OPC_WCVO || opc==OPC_WCVB ||
        opc==OPC_QCVS || opc==OPC_PCVS || opc==OPC_RDCC4 ||
        opc==OPC_WCVS || opc==OPC_RDCC5 || opc==OPC_WCVOA ||
        opc==OPC_RDCC6 || opc==OPC_PLOC || opc==OPC_STAT)
    {
        return DCC;
    }

    if (opc==OPC_RQDAT || opc==OPC_RQDDS || opc==OPC_ACON ||
        opc==OPC_ACOF || opc==OPC_AREQ || opc==OPC_ARON ||
        opc==OPC_AROF || opc==OPC_ASON || opc==OPC_ASOF ||
        opc==OPC_ASRQ || opc==OPC_ARSON || opc==OPC_ARSOF ||
        opc==OPC_ACON1 || opc==OPC_ACOF1 || opc==OPC_ARON1 ||
        opc==OPC_AROF1 || opc==OPC_ASON1 || opc==OPC_ASOF1 ||
        opc==OPC_ARSON1 || opc==OPC_ARSOF1 || opc==OPC_FCLK ||
        opc==OPC_ACON2 || opc==OPC_ACOF2 || opc==OPC_ARON2 ||
        opc==OPC_AROF2 || opc==OPC_ASON2 || opc==OPC_ASOF2 ||
        opc==OPC_ARSON2 || opc==OPC_ARSOF2 || opc==OPC_ACON3 ||
        opc==OPC_ACOF3 || opc==OPC_ARON3 || opc==OPC_AROF3 ||
        opc==OPC_ACDAT || opc==OPC_ARDAT || opc==OPC_ASON3 ||
        opc==OPC_ASOF3 || opc==OPC_DDES || opc==OPC_DDRS ||
        opc==OPC_ARSON3 || opc==OPC_ARSOF3 || opc==OPC_EXTC6)
    {
        return ACCESSORY;
    }
    if (opc==OPC_RSTAT || opc==OPC_QNN || opc==OPC_RQNP ||
        opc==OPC_RQMN || opc==OPC_SNN || opc==OPC_RQNN ||
        opc==OPC_NNREL || opc==OPC_NNACK || opc==OPC_NNLRN ||
        opc==OPC_NNULN || opc==OPC_NNCLR || opc==OPC_NNEVN ||
        opc==OPC_NERD || opc==OPC_RQEVN || opc==OPC_WRACK ||
        opc==OPC_BOOT || opc==OPC_ENUM || opc==OPC_CMDERR ||
        opc==OPC_EVNLF || opc==OPC_NVRD || opc==OPC_NENRD ||
        opc==OPC_RQNPN || opc==OPC_NUMEV || opc==OPC_CANID ||
        opc==OPC_EVULN || opc==OPC_NVSET || opc==OPC_NVANS ||
        opc==OPC_PARAN || opc==OPC_REVAL || opc==OPC_REQEV ||
        opc==OPC_NEVAL || opc==OPC_EVLRN || opc==OPC_EVANS ||
        opc==OPC_NAME || opc==OPC_PARAMS || opc==OPC_ENRSP ||
        opc==OPC_EVLRNI)
    {
        return CONFIG;
    }

    return RESERVED;

}
