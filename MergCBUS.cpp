#include "MergCBUS.h"

/** \brief
* Constructor
* Create the internal buffers.
* Initiate the memory management and transport object.
*
*/


MergCBUS::MergCBUS()
{
    //ctor
    messageFilter=0;
    bufferIndex=0;
    Can=MCP_CAN();
    memory=MergMemoryManagement();
    //canMessage=CANMessage();
    nodeId=MergNodeIdentification();
    message=Message();
    skipMessage(RESERVED);
    softwareEnum=false;
    DEBUG=false;
    greenLed=255;
    yellowLed=255;
    ledGreenState=HIGH;
    ledYellowState=LOW;
    userHandler=0;
    resetFunc=0;
    //candata=canMessage.getData();
    //message.setCanMessage(&canMessage);
}

/** \brief
* Start the memory reading the EEPROM values
*
*/
void MergCBUS::initMemory(){
    memory.read();
    loadMemory();
}

/** \brief
* Load the important EPROM memory data to RAM memory.
*/

void MergCBUS::loadMemory(){
    nodeId.setNodeNumber(memory.getNodeNumber());
    nodeId.setDeviceNumber(memory.getDeviceNumber());
    nodeId.setCanID(memory.getCanId());
    nodeId.setFlags(memory.getNodeFlag());
    if (nodeId.isSlimMode()){
        node_mode=MTYP_SLIM;
    }else{
        node_mode=MTYP_FLIM;
    }
}

/** \brief
* Destructor
* Not used.
*/

MergCBUS::~MergCBUS()
{
    //dtor
}
/** \brief
* Set the port number for SPI communication.
* Set the CBUS rate and initiate the transport layer.
* @param port is the the SPI port number.
* @param rate is the can bus rate. The values defined in the can transport layer are
* CAN_5KBPS    1
* CAN_10KBPS   2
* CAN_20KBPS   3
* CAN_31K25BPS 4
* CAN_40KBPS   5
* CAN_50KBPS   6
* CAN_80KBPS   7
* CAN_100KBPS  8
* CAN_125KBPS  9
* CAN_200KBPS  10
* CAN_250KBPS  11
* CAN_500KBPS  12
* CAN_1000KBPS 13
* @param retries is the number of retries to configure the can bus
* @param retryIntervalMilliseconds is the delay in milliseconds between each retry.
*/

bool MergCBUS::initCanBus(unsigned int port,unsigned int rate,unsigned int retries,unsigned int retryIntervalMilliseconds){

    unsigned int r=0;
    Can.set_cs(port);

    do {
        if (CAN_OK==Can.begin(rate)){

            if (DEBUG){
                Serial.println("Can rate set");
            }

            return true;
        }
        r++;
        delay(retryIntervalMilliseconds);
    }while (r<retries);

    if (DEBUG){
        Serial.println("Failed to set Can rate");
    }

   return false;
}

/** \brief
* Set or unset the bit in the message bit.
* @param pos specifies the bit position @see messageFilter
* @param val if true set bit to 1, else set bit to 0
* @see skipMessage
* @see processMessage
* Internal usage.
*/
void MergCBUS::setBitMessage(byte pos,bool val){
    if (val){
        bitSet(messageFilter,pos);
    }
    else{
        bitClear(messageFilter,pos);
    }
}

/** \brief
* Method that deals with the majority of messages and behavior. Auto enummeration, query requests and config messages.
* If a custom function is set it calls it for every non automatic message.
* @see setUserHandlerFunction
* If the green and yellow leds are set ,it also control the standard their behaviour based on node state.
*/
unsigned int MergCBUS::run(){

    controlLeds();

    if (state_mode==SELF_ENUMERATION){
        unsigned long tdelay=millis()-timeDelay;
        if (DEBUG){
            Serial.println("Processing self ennumeration.");
        }
        if (tdelay>SELF_ENUM_TIME){
            if (DEBUG){
                Serial.println("Finishing self ennumeration.");
            }
            finishSelfEnumeration();
        }
    }

    if (readCanBus()==false){
        //nothing to do
        return NO_MESSAGE;
    }

    if (message.getRTR()){
        //if we are a device with can id
        //we need to answer this message
        if (DEBUG){
                Serial.print("RTR message received.");
            }
        if (nodeId.getNodeNumber()!=0){
            //create the response message with no data
            if (DEBUG){
                Serial.print("RTR message received. Sending can id: ");
                Serial.println(nodeId.getCanID(),HEX);
            }
            //canMessage.clear();
            Can.sendMsgBuf(nodeId.getCanID(),0,0,mergCanData);
            return OK;
        }
    }

    //message for self enumeration
    if (message.getOpc()==OPC_ENUM){

        if (message.getNodeNumber()==nodeId.getNodeNumber()){
            if (DEBUG){
                    Serial.println("Starting message based self ennumeration.");
                }
            doSelfEnnumeration(true);
        }
        return OK;
    }

    //do self enumeration
    //collect the canid from messages with 0 size
    //the state can be a message or manually

    if (state_mode==SELF_ENUMERATION){
        Serial.print("other msg size:");
        Serial.println(message.getCanMessageSize());
        if (message.getCanMessageSize()==0){

            //if (message.getCanId()!=nodeId.getCanID()){
                if (DEBUG){
                    Serial.println("Self ennumeration: saving others can id.");
                }

                if (bufferIndex<SELF_ENUM_BUFFER_SIZE){
                    buffer[bufferIndex]=message.getCanId();
                    bufferIndex++;
                }
            //}
        }
        return OK;
    }


    //treat each message individually to interpret the code

    if (DEBUG){
        Serial.print("Message type:");
        Serial.print(message.getType());
        Serial.print ("\t OPC:");
        Serial.print(message.getOpc(),HEX);
        Serial.print("\t STATE:");
        Serial.println(state_mode);
    }

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
            return handleConfigMessages();
        break;
        default:
            return UNKNOWN_MSG_TYPE;
    }

    return OK;

}

/** \brief
* Read the can bus and load the data in the message object.
* @return true if a message in the can bus.
*/
bool MergCBUS::readCanBus(){
    byte len=0;
    bool resp;
    byte bufIdxdata=115;
    byte bufIdxhead=110;
    resp=readCanBus(&buffer[bufIdxdata],&buffer[bufIdxhead],&len);
    if (resp){
        message.clear();
        message.setCanMessageSize(len);
        message.setDataBuffer(&buffer[bufIdxdata]);
        if (Can.isRTMMessage()==0){
            //Serial.println("readCanBus - unsetRTM");
            message.unsetRTR();
        }
        else{
            //Serial.println("readCanBus - setRTM");
            message.setRTR();
        }
        message.setHeaderBuffer(&buffer[bufIdxhead]);
    }
    return resp;
}


/** \brief
* Read the can bus and return the buffer.
* @return number of bytes read;
*/
bool MergCBUS::readCanBus(byte *data,byte *header,byte *length){
    byte resp;
    if(CAN_MSGAVAIL == Can.checkReceive()) // check if data coming
    {
        resp=Can.readMsgBuf(length,data);
        if (resp==CAN_OK){
            Can.getCanHeader(header);
            return true;
        }
        return false;
    }
    return false;
}

/** \brief
* Put node in setup mode and send the Request Node Number RQNN
* It is the function that starts the changing from slim to flim
*/
void MergCBUS::doSetup(){

    state_mode=SETUP;
    prepareMessage(OPC_RQNN);
    if (DEBUG){
        Serial.println("Doing setup");
        printSentMessage();
    }
    sendCanMessage();
}

/** \brief
* Node when going out of service. Send NNREL.
*/
void MergCBUS::doOutOfService(){
    prepareMessage(OPC_NNREL);
    sendCanMessage();
}
/** \brief
* Initiate the auto enumeration procedure
* Start the timers and send a RTR message.
* @param softEnum True if the self ennumeration started by a software tool by receiving a ENUM message.
*/
void MergCBUS::doSelfEnnumeration(bool softEnum){
    bufferIndex=0;
    softwareEnum=softEnum;
    state_mode=SELF_ENUMERATION;
    //Can.sendRTMMessage(nodeId.getCanID());
    Can.setPriority(PRIO_LOW,PRIO_MIN_LOWEST);
    Can.sendRTMMessage(nodeId.getCanID());
    timeDelay=millis();
}

/** \brief
* Finish the auto enumeration. Get the lowest available can id and set the Node to NORMAL mode.
* If a software tool started the ennumeration, it return a NNACK message - in revision.
*/
void MergCBUS::finishSelfEnumeration(){
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
    if (cid>99){

         if (DEBUG){
                Serial.println("Self ennumeration: no can id available.");
            }

        //send and error message
        if (softwareEnum){
            sendERRMessage(CMDERR_INVALID_EVENT);
        }
        return;
    }

    if (DEBUG){
        Serial.print("Self ennumeration: new can id.");
        Serial.println(cid);
    }
    memory.setCanId(cid);
    nodeId.setCanID(cid);
    //TODO: check if it is from software

    if (softwareEnum){
        prepareMessageBuff(OPC_NNACK,
                       highByte(nodeId.getNodeNumber()),
                       lowByte(nodeId.getNodeNumber())  );
        Can.sendMsgBuf(nodeId.getCanID(),0,3,mergCanData);
    }

    return;
}


/** \brief
* Handle all config messages
* Do the hard work of learning and managing the memory
*/
byte MergCBUS::handleConfigMessages(){


    byte ind,val,evidx;
    unsigned int ev,nn,resp;

    //config messages should be directed to node number or device id
    if (message.getNodeNumber()!=nodeId.getNodeNumber()) {
        if (DEBUG){
            Serial.println("handleConfigMessages- NN different from message NN");
        }
        if (state_mode!=SETUP){
                if (DEBUG){
                    Serial.println("handleConfigMessages- not in setup mode. leaving");
                }
                return OK;
        }
    }
    if (DEBUG){
        Serial.println("handleConfigMessages- Processing config message");
        Serial.print("handleConfigMessages- state:");
        Serial.println(state_mode);
        printReceivedMessage();
    }

    nn=nodeId.getNodeNumber();

    switch ((unsigned int)message.getOpc()){
    case OPC_RSTAT:
        //command station
        return OK;
        break;
    case OPC_QNN:
        //response with a OPC_PNN if we have a node ID
        //[<MjPri><MinPri=3><CANID>]<B6><NN Hi><NN Lo><Manuf Id><Module Id><Flags>

        if (nn>0){
            prepareMessage(OPC_PNN);

            if (DEBUG){
                Serial.println("RECEIVED OPC_QNN sending OPC_PNN");
                printSentMessage();
            }
            return sendCanMessage();
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
            prepareMessage(OPC_PARAMS);

            if (DEBUG){
                Serial.println("RECEIVED OPC_RQNP sending OPC_PARAMS");
                printSentMessage();
            }

            return sendCanMessage();
        }
        break;
    case OPC_RQMN:
        //Answer with OPC_NAME
        if (state_mode==SETUP){
            prepareMessage(OPC_NAME);

            if (DEBUG){
                Serial.println("RECEIVED OPC_RQNN sending OPC_NAME");
                printSentMessage();
            }

            return sendCanMessage();
        }else{
            if (DEBUG){
                Serial.println("RECEIVED OPC_RQNN and not in setup mode.");
            }

            sendERRMessage(CMDERR_NOT_SETUP);
        }
        break;

    case OPC_SNN:
        //set the node number
        //answer with OPC_NNACK
        if (state_mode==SETUP){
            if (DEBUG){
                Serial.println("RECEIVED OPC_SNN sending OPC_NNACK");
                //printSentMessage();
            }
            nodeId.setNodeNumber(message.getNodeNumber());
            memory.setNodeNumber(nodeId.getNodeNumber());
            prepareMessage(OPC_NNACK);


            setFlimMode();
            state_mode=NORMAL;
            return sendCanMessage();
        }else{

            if (DEBUG){
                Serial.println("RECEIVED OPC_SNN and not in setup mode.");
            }

            sendERRMessage(CMDERR_NOT_SETUP);
        }
        break;
    case OPC_NNLRN:
        //put the node in the lear mode
        state_mode=LEARN;

        if (DEBUG){
            Serial.println("going to LEARN MODE.");
        }

        break;

    case OPC_NNULN:
        //leaving the learn mode
        state_mode=NORMAL;
        if (DEBUG){
            Serial.println("going to NORMAL MODE.");
        }
        break;

    case OPC_NNCLR:
        //clear all events from the node
        if (state_mode==LEARN){

            if (DEBUG){
                Serial.println("Clear all events.");
            }

            memory.eraseAllEvents();
            return OK;
        }
        break;
    case OPC_NNEVN:
        //read the events available in memory
        prepareMessage(OPC_EVNLF);

        if (DEBUG){
                Serial.println("RECEIVED OPC_NNEVN sending OPC_EVNLF");
                printSentMessage();
            }

        return sendCanMessage();
        break;

    case OPC_NERD:
        //send back all stored events in message OPC_ENRSP
        int i;
        i=(int)memory.getNumEvents();
        if (i>0){
            byte *events=memory.getEvents();
            int pos=0;
            for (int j=0;j<i;j++){
                prepareMessageBuff(OPC_ENRSP,highByte(nn),lowByte(nn),
                                events[pos],
                                events[pos+1],
                                events[pos+2],
                                events[pos+3],
                                (j+1)
                );
                pos=pos+4;


                if (DEBUG){
                    Serial.println("RECEIVED OPC_NERD sending OPC_ENRSP");
                    printSentMessage();
                }

                ind=sendCanMessage();
            }
        }
        break;

    case OPC_RQEVN:
        //request the number of stored events
        prepareMessage(OPC_NUMEV);
        if (DEBUG){
            Serial.println("RECEIVED OPC_RQEVN sending OPC_NUMEV");
            printSentMessage();
        }
        sendCanMessage();
        break;
    case OPC_BOOT:
        //boot mode. not supported
        return OK;
        break;
    case OPC_ENUM:
        //has to be handled in the automatic procedure
        if (message.getNodeNumber()==nodeId.getNodeNumber()){
            if (DEBUG){
                Serial.println("Doing self ennumeration");
            }

            doSelfEnnumeration(true);
        }

        break;
    case OPC_NVRD:
        //answer with NVANS
        ind=message.getNodeVariableIndex();

        prepareMessageBuff(OPC_NVANS,highByte(nn),lowByte(nn),ind,memory.getVar(ind-1));//the CBUS index start with 1

        if (DEBUG){
            Serial.println("RECEIVED OPC_NVRD sending OPC_NVANS");
            printSentMessage();
        }

        sendCanMessage();
        break;

    case OPC_NENRD:
        //Request read of stored events by event index
        clearMsgToSend();
        ind=message.getEventIndex();
        byte *event;
        event=memory.getEvent(ind-1);//the CBUS index start with 1
        prepareMessageBuff(OPC_ENRSP,highByte(nn),lowByte(nn),event[0],event[1],event[2],event[3],ind);


        if (DEBUG){
            Serial.println("RECEIVED OPC_NENRD sending OPC_ENRSP");
            printSentMessage();
        }

        sendCanMessage();

        break;

    case OPC_RQNPN:
        //Request node parameter. Answer with PARAN
        ind=message.getParaIndex();

        if (ind==0){
            prepareMessageBuff(OPC_PARAN,highByte(nn),lowByte(nn),ind,nodeId.getNumberOfParameters());//the CBUS index start with 1
        }
        else{
            prepareMessageBuff(OPC_PARAN,highByte(nn),lowByte(nn),ind,nodeId.getParameter(ind));
        }

        if (DEBUG){
            Serial.println("RECEIVED OPC_RQNPN sending OPC_PARAN");
            printSentMessage();
        }
        sendCanMessage();
        break;
    case OPC_CANID:
        //force a new can id
        ind=message.getByte(3);
        nodeId.setCanID(ind);
        memory.setCanId(ind);
        prepareMessage(OPC_NNACK);

        if (DEBUG){
            Serial.println("RECEIVED OPC_CANID sending OPC_NNACK");
            printSentMessage();
        }

        sendCanMessage();
        break;
    case OPC_EVULN:
        //Unlearn event
        if (DEBUG){
            Serial.println("Unlearn event");
            //printSentMessage();
        }
        if (state_mode==LEARN){
            ev=message.getEventNumber();
            nn=message.getNodeNumber();
            evidx=memory.getEventIndex(nn,ev);

            if (evidx<0){
                sendERRMessage(CMDERR_INVALID_EVENT);
                break;
            }

            if (memory.eraseEvent(evidx)!=ev){
                //send ack
                prepareMessage(OPC_WRACK);
                sendCanMessage();
            }else{
                //send error
                sendERRMessage(CMDERR_INVALID_EVENT);
            }
        }else{
            sendERRMessage(CMDERR_NOT_LRN);
        }

        break;
    case OPC_NVSET:
        //set a node variable
        ind=message.getNodeVariableIndex()-1;//the CBUS index start with 1
        val=message.getNodeVariable();
        if (DEBUG){
            Serial.println("Learning node variable");
            //printSentMessage();
        }

        if (ind<=nodeId.getSuportedNodeVariables()){
            memory.setVar(ind,val);
            prepareMessage(OPC_WRACK);
            sendCanMessage();
        }else{
            //send error
            sendERRMessage(CMDERR_INV_PARAM_IDX);
        }
        break;

    case OPC_REVAL:
        //Request for read of an event variable
        evidx=message.getEventIndex();
        ind=message.getEventVarIndex();
        val=memory.getEventVar(evidx-1,ind-1);//the CBUS index start with 1
        nn=nodeId.getNodeNumber();

        prepareMessageBuff(OPC_NEVAL,highByte(nn),lowByte(nn),evidx,ind,val);

        if (DEBUG){
            Serial.println("RECEIVED OPC_REVAL sending OPC_NEVAL");
            printSentMessage();
        }

        sendCanMessage();
        break;
    case OPC_REQEV:
        //Read event variable in learn mode
        if (state_mode==LEARN){
            ev=message.getEventNumber();
            evidx=memory.getEventIndex(nn,ev);
            ind=message.getEventVarIndex();
            val=memory.getEventVar(evidx,ind-1);//the CBUS index start with 1

            prepareMessageBuff(OPC_EVANS,highByte(nn),lowByte(nn),highByte(ev),lowByte(ev),ind,val);

        if (DEBUG){
            Serial.println("RECEIVED OPC_REQEV sending OPC_EVANS");
            printSentMessage();
        }

            sendCanMessage();
        }else{
            sendERRMessage(CMDERR_NOT_LRN);
        }

        break;

    case OPC_EVLRN:
        //learn event
        if (state_mode==LEARN){

            //TODO: suport device number mode

            if (DEBUG){
                Serial.println("Learning event.");
                //printSentMessage();
            }

            ev=message.getEventNumber();
            nn=message.getNodeNumber();
            ind=message.getEventVarIndex();
            val=message.getEventVar();

            //save event and get the index
            buffer[0]=highByte(nn);
            buffer[1]=lowByte(nn);
            buffer[2]=highByte(ev);
            buffer[3]=lowByte(ev);
            evidx=memory.setEvent(buffer);

            if (evidx>MAX_NUM_EVENTS){
                //send a message error
                sendERRMessage(CMDERR_TOO_MANY_EVENTS);
                break;
            }

            //save the parameter
            //the CBUS index start with 1
            resp=memory.setEventVar(evidx,ind-1,val);

            if (resp!=message.getEventVarIndex()){
                //send a message error
                sendERRMessage(CMDERR_INV_NV_IDX);
                break;
            }

            //get the device number in case of short event
            if (nn==0){
                nodeId.setDeviceNumber(ev);
            }

            //send a WRACK back
            prepareMessage(OPC_WRACK);
            sendCanMessage();

        }else{
            sendERRMessage(CMDERR_NOT_LRN);
            break;
        }

        break;

    case OPC_EVLRNI:
        //lear event by index. like an update
        if (state_mode==LEARN){

            //TODO: suport device number mode

            if (DEBUG){
                Serial.println("Learning event by index.");
                //printSentMessage();
            }

            ev=message.getEventNumber();
            nn=message.getNodeNumber();
            ind=message.getEventVarIndex();
            val=message.getEventVar();
            evidx=message.getEventIndex();

            //save event and get the index
            buffer[0]=highByte(nn);
            buffer[1]=lowByte(nn);
            buffer[2]=highByte(ev);
            buffer[3]=lowByte(ev);
            resp=memory.setEvent(buffer,evidx-1);

            if ((byte)resp!=(evidx-1)){
                //send a message error
                sendERRMessage(CMDERR_INV_EV_IDX);
                break;
            }

            //save the parameter
            //the CBUS index start with 1
            resp=memory.setEventVar(evidx-1,ind-1,val);

            if ((byte)resp!=(ind-1)){
                //send a message error
                sendERRMessage(CMDERR_INV_NV_IDX);
                break;
            }
            //send a WRACK back
            prepareMessage(OPC_WRACK);
            sendCanMessage();

        }else{
            sendERRMessage(CMDERR_NOT_LRN);
        }

        break;
    }
    return OK;
}

/** \brief
* Deals with accessory functions. No automatic response of events once the user function determines the behaviour.
* The accessory messages has to be threated by the user function
* once it is related to the module function
* has to deal with ACON,SCOF, ARON ,AROF, AREQ, ASON, ASOF
*/
byte MergCBUS::handleACCMessages(){
    if (userHandler!=0){
        userHandler(&message,this);
    }
    return OK;
}

/** \brief
* Handle general messages. No automatic response of events once the user function determines the behaviour..
* Has to handle the EXTC messages
*/
byte MergCBUS::handleGeneralMessages(){

    switch ((unsigned int) message.getOpc()){
    case OPC_ARST:
            //reset arduino
            Reset_AVR();
        break;
    case OPC_RST:
            Reset_AVR();
        break;
    }

    if (userHandler!=0){
        userHandler(&message,this);
    }
    return OK;
}

// TODO (amauriala#1#): Create the DDC handle
/** \brief
* Handle DCC messages. Still to TODO.
*/
byte MergCBUS::handleDCCMessages(){
    return 0;
}

/** \brief
* Sort a simple array.
*/

void MergCBUS::sortArray(byte *a, byte n){

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

/** \brief
* Clear the message buffer
*/
void MergCBUS::clearMsgToSend(){
    for (int i=0;i<CANDATA_SIZE;i++){
        mergCanData[i]=0;
    }
}

/** \brief
* Send the message to can bus. The message is set by @see prepareMessage or @see prepareMessageBuff
* @return OK if no error found, else return error code from the transport layer.
*/
byte MergCBUS::sendCanMessage(){
    byte message_size;
    message_size=getMessageSize(mergCanData[0]);
    Can.setPriority(PRIO_LOW,PRIO_MIN_LOWEST);
    byte r=Can.sendMsgBuf(nodeId.getCanID(),0,message_size,mergCanData);
    if (CAN_OK!=r){
        return r;
    }
    return OK;
}

/** \brief
* Put in debug mode
*/
void MergCBUS::setDebug(bool debug){
    DEBUG=debug;
    message.setDebug(debug);
}

/** \brief
* Get the message size using the opc
*/
byte MergCBUS::getMessageSize(byte opc){
    byte a=opc;
    a=a>>5;
    return (a+1);
}

/**\brief
* Save the parameters to the message buffer.
*/
void MergCBUS::prepareMessageBuff(byte data0,byte data1,byte data2,byte data3,byte data4,byte data5,byte data6,byte data7){
    //clearMsgToSend();
    mergCanData[0]=data0;
    mergCanData[1]=data1;
    mergCanData[2]=data2;
    mergCanData[3]=data3;
    mergCanData[4]=data4;
    mergCanData[5]=data5;
    mergCanData[6]=data6;
    mergCanData[7]=data7;
}

/**\brief
* Prepare the general messages by opc. If the opc is unknow it does nothing.
*/
void MergCBUS::prepareMessage(byte opc){

    //clearMsgToSend();
    switch (opc){
    case OPC_PNN:
        prepareMessageBuff(OPC_PNN,highByte(nodeId.getNodeNumber()),lowByte(nodeId.getNodeNumber()),
                            nodeId.getManufacturerId(),nodeId.getModuleId(),nodeId.getFlags());

        break;
    case OPC_NAME:
        prepareMessageBuff(OPC_NAME,nodeId.getNodeName()[0],nodeId.getNodeName()[1],
                            nodeId.getNodeName()[2],nodeId.getNodeName()[3],
                            nodeId.getNodeName()[4],nodeId.getNodeName()[5],
                            nodeId.getNodeName()[6]);

        break;
    case OPC_PARAMS:
        prepareMessageBuff(OPC_PARAMS,nodeId.getManufacturerId(),
                           nodeId.getMinCodeVersion(),nodeId.getModuleId(),
                           nodeId.getSuportedEvents(),nodeId.getSuportedEventsVariables(),
                           nodeId.getSuportedNodeVariables(),nodeId.getMaxCodeVersion());

        break;
    case OPC_RQNN:
        prepareMessageBuff(OPC_RQNN,highByte(nodeId.getNodeNumber()),lowByte(nodeId.getNodeNumber()));

        break;
    case OPC_NNACK:
        prepareMessageBuff(OPC_NNACK,highByte(nodeId.getNodeNumber()),lowByte(nodeId.getNodeNumber()));

        break;

    case OPC_NNREL:
        prepareMessageBuff(OPC_NNREL,highByte(nodeId.getNodeNumber()),lowByte(nodeId.getNodeNumber()));

        break;
    case OPC_EVNLF:
        prepareMessageBuff(OPC_EVNLF,highByte(nodeId.getNodeNumber()),lowByte(nodeId.getNodeNumber()),(nodeId.getSuportedEvents()-memory.getNumEvents()));

        break;
    case OPC_NUMEV:
        prepareMessageBuff(OPC_NUMEV,highByte(nodeId.getNodeNumber()),lowByte(nodeId.getNodeNumber()),memory.getNumEvents());

        break;
    case OPC_WRACK:
        prepareMessageBuff(OPC_WRACK,highByte(nodeId.getNodeNumber()),lowByte(nodeId.getNodeNumber()));

        break;
    }
}

/**\brief
* Send the error message with the code.
* @param code can be:
* 1 Command Not Supported - see note 1.
* 2 Not In Learn Mode
* 3 Not in Setup Mode - see note 1
* 4 Too Many Events
* 5 Reserved
* 6 Invalid Event variable index
* 7 Invalid Event
* 8 Reserved - see note 2
* 9 Invalid Parameter Index
* 10 Invalid Node Variable Index
* 11 Invalid Event Variable Value
* 12 Invalid Node Variable Value

* Note 1: Accessory modules do not return this error
* Note 2: Currently used by code that processes OPC REVAL 0x9C but this code
* should be updated to use codes 6 & 7.
*/
void MergCBUS::sendERRMessage(byte code){
    prepareMessageBuff(OPC_CMDERR,highByte(nodeId.getNodeNumber()),lowByte(nodeId.getNodeNumber()),code);

    sendCanMessage();

}

/**\brief
* Check it the received event is a learned event. For the messages ACONs,ACOFs,ASONs,ASOFs.
* Should be called after reading the can bus.
*/

bool MergCBUS::hasThisEvent(){
    byte opc=message.getOpc();

    if (opc==OPC_ACON || opc==OPC_ACON1 || opc==OPC_ACON2 || opc==OPC_ACON3 ||
        opc==OPC_ACOF || opc==OPC_ACOF1 || opc==OPC_ACOF2 || opc==OPC_ACOF3 ||
        opc==OPC_ASON || opc==OPC_ASON1 || opc==OPC_ASON2 || opc==OPC_ASON3 ||
        opc==OPC_ASOF || opc==OPC_ASOF1 || opc==OPC_ASOF2 || opc==OPC_ASOF3
        ){
             if (memory.getEventIndex(message.getNodeNumber(),message.getEventNumber())>=0){
                return true;
             }
    }
    return false;
}

/**\brief
* Print the message to be sent to serial. Used for debug.
*/

void MergCBUS::printSentMessage(){

    Serial.print("printSentMessage- message sent: ");
    for (int i=0;i<8;i++){
        Serial.print(mergCanData[i],HEX);
        Serial.print(" ");
    }
    Serial.println("");

}

/**\brief
* Print the received message buffer to serial. Used for debug.
*/
void MergCBUS::printReceivedMessage(){

    if (message.getCanMessageSize()<=0){
        return;
    }
    Serial.print("printReceivedMessage- message received: ");
    for (int i=0;i<8;i++){
        Serial.print(message.getByte(i),HEX);
        Serial.print(" ");
    }
    Serial.println("");

}

/**\brief
* Create a new memory set up. Writes the EEPROM with null values. Equivalent to reset the memory.
* Should be done once before setting a new node.
*/
void MergCBUS::setUpNewMemory(){
    memory.setUpNewMemory();
}

/**\brief
* Set the pins for green and yello leds.
* @param green Pin for the green led.
* @param yellow Pin for the yellow led.
*/
void MergCBUS::setLeds(byte green,byte yellow){
    greenLed=green;
    yellowLed=yellow;
    pinMode(greenLed,OUTPUT);
    pinMode(yellowLed,OUTPUT);
}

/**\brief
* Do the automatic led control based on the node status.
* Called in the run() function. If not using the @see run then has to be called manually
*/
void MergCBUS::controlLeds(){

    if (yellowLed==255 && greenLed==255){
        return;
    }

    if (state_mode==SETUP){
        delay(100);
        if (ledYellowState==HIGH){
            digitalWrite(yellowLed,LOW);
            ledYellowState=LOW;
        }else{
            digitalWrite(yellowLed,HIGH);
            ledYellowState=HIGH;
        }
    }
    else{
        if(node_mode==MTYP_FLIM){
            digitalWrite(greenLed,LOW);
            digitalWrite(yellowLed,HIGH);
        }
        else{
            digitalWrite(greenLed,HIGH);
            digitalWrite(yellowLed,LOW);
        }
    }






}

/**\brief
* Return true if the node is in self enumeration mode
*/
bool MergCBUS::isSelfEnumMode(){

    if (state_mode==SELF_ENUMERATION){return true;}
    return false;
}
/**
* Check the if it is an ON message. Major event in CBUS.
* @return True if is and On event, false if not
*/
bool MergCBUS::isAccOn(){
    return message.isAccOn();
}
/**
* Check the if it is an OFF message. Major event in CBUS.
* @return True if is and OFF event, false if not
*/
bool MergCBUS::isAccOff(){
    return message.isAccOff();
}
/**
* Return how many bytes of extra data has the ON event.
* @return The number of extra bytes depending on the message type. ACON,ACOF=0 ; ACON1,ACOF1=1; ACON2,ACOF2=2; ACON3,ACOF1=3
*/
byte MergCBUS::accExtraData(){
    return message.accExtraData();

}
/**
* Get the extra data byte on an ON or OFF event.
* @return Return the extra byte. The index is between 1 and 3
*/
byte MergCBUS::getAccExtraData(byte idx){
    return message.getAccExtraData(idx);
}

void MergCBUS::setSlimMode(){
    node_mode=MTYP_SLIM;
    nodeId.setSlimMode();
    memory.setNodeFlag(nodeId.getFlags());
}

void MergCBUS::setFlimMode(){
    node_mode=MTYP_FLIM;
    nodeId.setFlimMode();
    memory.setNodeFlag(nodeId.getFlags());
}

void MergCBUS::saveNodeFlags(){
    memory.setNodeFlag(nodeId.getFlags());
}
