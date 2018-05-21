#include "MergMemoryManagement.h"

/**\brief
* Constructor: Default constructor. Does nothing.
*/
MergMemoryManagement::MergMemoryManagement()
{
    //ctor

}
/**\brief
* Constructor: clear array and reads the EEPROM
*/
MergMemoryManagement::MergMemoryManagement(byte num_node_vars, byte num_events, byte num_events_vars, byte max_device_numbers){
    MAX_AVAIL_VARS = num_node_vars;
    MAX_NUM_EVENTS = num_events;
    MAX_VAR_PER_EVENT = num_events_vars;
    //MAX_EVENTS_VAR_BUFFER= MAX_NUM_EVENTS*MAX_VAR_PER_EVENT;
    MAX_NUM_DEVICE_NUMBERS = max_device_numbers;
    DN_MEMPOS = VARS_MEMPOS + MAX_AVAIL_VARS;
    EVENTS_MEMPOS = DN_MEMPOS + NNDD_SIZE * MAX_NUM_DEVICE_NUMBERS;

    return_eventVars = new byte[MAX_VAR_PER_EVENT];
    clear();
    read();
}
/**\brief
* Destructor: clear the allocated buffers.
*/
MergMemoryManagement::~MergMemoryManagement(){
    delete [] return_eventVars;
}

/**\brief
* Clear the EEPROM memory and the internal events.
* Should be called just in the first setup of the node and never inside a loop. May damage the EEPROM.
*/
void MergMemoryManagement::setUpNewMemory(){
    //Serial.println(F("Clear memory called"));
    clear();
    write();
}

/**\brief
* Clear the internal buffers
*/
void MergMemoryManagement::clear(){


    for (int i=0;i<(MAX_VAR_PER_EVENT);i++){
        return_eventVars[i]=0;
    }

    can_ID=0;
    nn[0]=EMPTY_BYTE;
    nn[1]=EMPTY_BYTE;
    numEvents=0;
    flags=0;

}


/**\brief
* Return the event pointed by the index
* If index out of bounds return empty event '0000'
* @return 4 bytes
*/

byte * MergMemoryManagement::getEvent(uint8_t index){
    event[0]=EMPTY_BYTE;
    event[1]=EMPTY_BYTE;
    event[2]=EMPTY_BYTE;
    event[3]=EMPTY_BYTE;
    lasterror = 0;

    //Serial.println(F("MEM: Getting event"));

    if (index > (MAX_NUM_EVENTS) || index > numEvents){
        //Serial.println(F("MEM: Getting event failed"));
        lasterror = 1;
        return event;
    }
    unsigned int i = resolveEventPos(index);
    //Serial.print(F("ev "));Serial.print(index);Serial.print(F(" mem pos:"));Serial.println(i);

    event[0]=EEPROM.read(i);
    event[1]=EEPROM.read(i+1);
    event[2]=EEPROM.read(i+2);
    event[3]=EEPROM.read(i+3);
    return event;
}

/**\brief
* Put a new event in the memory and return the index
* @return event index starting with 0.
*/
uint8_t MergMemoryManagement::setEvent(byte *event){
    //Serial.println(F("MEM: Setting event"));
    if (numEvents >= MAX_NUM_EVENTS){
        return (MAX_NUM_EVENTS + 1) ;
    }

    //check if the event exists
    uint8_t evidx;
    evidx = getEventIndex(event[0],event[1],event[2],event[3]);
    //Serial.println(F("MEM: Getting event idx"));
    if (evidx > MAX_NUM_EVENTS){
        //event does not exist
        //get the next index
        //Serial.println(F("MEM: Saving new event"));
        evidx = numEvents;
        //save the event
        return setEvent(event,evidx);
    }
    //Serial.print(F("MEM: event idx:"));
    //Serial.println(evidx);
   return evidx;
}
/**\brief
* Put a new event in the memory and return its index.
* @param event 4 bytes event
* @param index event index starting in 0.
*/
uint8_t MergMemoryManagement::setEvent(byte *event,uint8_t index){

    if (index >= MAX_NUM_EVENTS){
        return (MAX_NUM_EVENTS + 1) ;
    }
    int j;

    numEvents++;
    //number of events
    EEPROM.write(NUM_EVENTS_MEMPOS, numEvents);
     //write event

     j = resolveEventPos(index);
     for (int i = 0;i < EVENT_SIZE;i++){
        EEPROM.write(j,event[i]);
        j++;
     }
     return index;
}

/**\brief
* Return the internal index of an event
* @param nn Node number (16 bit integer)
* @param ev Event number (16 bit integer)
* @return event index starting on 0.
*/
uint8_t MergMemoryManagement::getEventIndex(unsigned int nn,unsigned int ev){

    return getEventIndex (highByte(nn),lowByte(nn),highByte(ev),lowByte(ev));

}

/**\brief
* Return the internal index of an event using 4 bytes as parameters.
* The first 2 bytes are supposed to be the node number and the last 2 the event number
* @return event index starting on 0.
*/
uint8_t MergMemoryManagement::getEventIndex(byte ev1,byte ev2,byte ev3,byte ev4){
   unsigned int n = EVENTS_MEMPOS;
    for (uint8_t i = 0; i < numEvents; i++){
        if (ev1 != 0 || ev2 != 0){
            if (ev1 == EEPROM.read(n) &&
                ev2 == EEPROM.read(n+1) &&
                ev3 == EEPROM.read(n+2) &&
                ev4 == EEPROM.read(n+3)){
                return i;
            }
        }
        //check for short events
        else{
            if (ev3 == EEPROM.read(n+2) &&
                ev4 == EEPROM.read(n+3)){
                return i;
            }
        }
        n = incEventPos(n);
    }
    return MAX_NUM_EVENTS + 1;
}

/**\brief
* Check if the given event is present in the memory.
* @return Return true if the event is present in memory, else return false.
*/
bool MergMemoryManagement::hasEvent(byte ev1,byte ev2,byte ev3,byte ev4){
    if (numEvents < 1){
        return false;
    }
    if (getEventIndex(ev1,ev2,ev3,ev4) > MAX_NUM_EVENTS){
        return false;
    }
    return true;
}

/**\brief
* Return the node variable pointed by the index
* @return FAILED_INDEX if index is out of bounds
*/
byte MergMemoryManagement::getVar(uint8_t index){
    if (index>=MAX_AVAIL_VARS){
        return FAILED_INDEX;
    }
    return EEPROM.read(VARS_MEMPOS+index);
}

/**\brief
* Return the node internal variable pointed by the index
* @return FAILED_INDEX if index is out of bounds
*/
byte MergMemoryManagement::getInternalVar(uint8_t index){
    if (index >= INTERNAL_VARS){
        return FAILED_INDEX;
    }
    return EEPROM.read(NODE_INTERNAL_VAR + index);
}

/**\brief
* Return the event variable for a specific event
* @param eventIdx is the event index starting on 0.
* @param index indicates the variable position startin on 0
* @return FAILED_INDEX if index out of bounds
*/
byte MergMemoryManagement::getEventVar(uint8_t eventIdx,uint8_t index){
    byte val;
    lasterror = 0;
    if (eventIdx > numEvents){
        lasterror = FAILED_INDEX;
        return FAILED_INDEX;
    }

    if (index > MAX_VAR_PER_EVENT){
        lasterror = FAILED_INDEX;
        return FAILED_INDEX;
    }

    //position in the array
    int i = resolveEvVarArrayPos(eventIdx,index);
    val = EEPROM.read(i);
    /*
    Serial.print(F("ev "));
    Serial.print (eventIdx);
    Serial.print (" var:"));
    Serial.print (index);
    Serial.print (" val:"));
    Serial.println(val);
    */
    return val;
}

/**\brief
* Get all variables of an specific event
* @param eventIdx is the event index starting on 0.
* @param len indicates how many variables
*/
byte* MergMemoryManagement::getEventVars(uint8_t eventIdx,uint8_t *len){
    //populate the array to return
    *len=0;
    int j = resolveEvVarArrayPos(eventIdx,0);
    for (uint8_t i = 0; i< MAX_VAR_PER_EVENT; i++){
        return_eventVars[i] = EEPROM.read(j);
        j++;
    }
    *len = MAX_VAR_PER_EVENT;
    return return_eventVars;
}

/**\brief
* Read all data from eprom memory and put in the internal arrays
* @return
* Memory organization
* 512 bytes is the max value for simple arduino
* byte=value;explanation
* 0xaa ; indicates that it is a merg arduino memory storage
* can_id;store the can_id
* hh Node number;high byte from node number
* ll Node number;low byte from node number
* flags
* hh device number;high byte from node number
* ll device number;low byte from node number
* amount of node variables
* amount of learned events
* amount of learned event variables
* 20 bytes for node variables
* 120 bytes for 30 learned events
* 150 bytes for learned event variables. 2 bytes for each variable. 1st is the event number index. the index is the position of the event in the memory range from 1 to 30
*/
void MergMemoryManagement::read(){

    clear();
    byte mergid = EEPROM.read(MERG_MEMPOS);

    if (mergid != 0xaa){
        write();
        return;
    }

    //read can id
    can_ID = EEPROM.read(CAN_ID_MEMPOS);
    //node number
    nn[0] = EEPROM.read(NN_MEMPOS);
    nn[1] = EEPROM.read(NN_MEMPOS+1);
    //node mode
    flags = EEPROM.read(FLAGS_MEMPOS);
    //number of node variables
    //numVars=EEPROM.read(NUM_VARS_MEMPOS);
    //number of events
    numEvents = EEPROM.read(NUM_EVENTS_MEMPOS);
    //number of events
    //numEventVars=EEPROM.read(NUM_EVENTS_VARS_MEMPOS);


}

/**\brief
* Write all data to memory. it overwrites the all the previous information.
*/
void MergMemoryManagement::write(){

    //Serial.println(F("write function called"));

    //merg id
    EEPROM.write(MERG_MEMPOS,0xaa);
    //Can id
    EEPROM.write(CAN_ID_MEMPOS,can_ID);
    //node number
    EEPROM.write(NN_MEMPOS,nn[0]);
    EEPROM.write(NN_MEMPOS+1,nn[1]);
    //node mode
    EEPROM.write(FLAGS_MEMPOS,flags);
    //number of variables
    //EEPROM.write(NUM_VARS_MEMPOS,numVars);
    //number of events
     EEPROM.write(NUM_EVENTS_MEMPOS,numEvents);
    //number of events vars
     EEPROM.write(NUM_EVENTS_VARS_MEMPOS,MAX_VAR_PER_EVENT);

}

/**\brief
* Erase all events and their variables.
*/

void MergMemoryManagement::eraseAllEvents(){
    //to avoid to many eprom writings we just set the numberOfEvents to zero and the eventsVar to zero and clear the arrays

    //erase the events var
    //number of events
     EEPROM.write(NUM_EVENTS_MEMPOS,0);
    numEvents = 0;

}

/**\brief
* Erase a specific event by the event index.
* Has to reorganize the memory: events and events vars to avoid fragmentation.
* @param eventIdx event index starting on 0.
*/
uint8_t MergMemoryManagement::eraseEvent(uint8_t eventIdx){

    if (eventIdx > numEvents || numEvents < 1){
        return (MAX_NUM_EVENTS +1);
    }
    numEvents--;
     //write number of events
    EEPROM.write(NUM_EVENTS_MEMPOS,numEvents);
    //copy the events and vars to one position less
    for (int i = eventIdx; i < numEvents; i++){
        copyEvent(i + 1,i);
    }

    return eventIdx;
}
/**\brief
* Make a copy of the event and event vars to another position.
* Has to reorganize the memory: events and events vars to avoid fragmentation.
* @param fromIndex event index starting on 0.
* @param toIndex event index starting on 0.
*/
void MergMemoryManagement::copyEvent(uint8_t fromIndex,uint8_t toIndex){

    unsigned int memposOld,memposNew;


    if (fromIndex==toIndex){return;};

    memposOld=resolveEventPos(fromIndex);
    memposNew=resolveEventPos(toIndex);

    for (int i=0;i<(EVENT_SIZE+MAX_VAR_PER_EVENT);i++){
        EEPROM.write(memposNew,EEPROM.read(memposOld));
        memposNew++;
        memposOld++;
    }

}


/**\brief
* Erase a specific event.
* Has to reorganize the memory: events and events vars to avoid fragmentation.
* @param nn node number (16 bits integer)
* @param ev event number (16 bits integer)
*/
uint8_t MergMemoryManagement::eraseEvent(unsigned int nn,unsigned int ev){

    //find the event in the memory and erase
    int index=getEventIndex(nn,ev);
    if (index>=0){
        return eraseEvent(index);
    }
    return (MAX_NUM_EVENTS+1);
}


/**\brief
* Set the value of a node variable.
* @param index variable index
* @param val variable value
*/
void MergMemoryManagement::setVar(uint8_t index,byte val){
    if (index>MAX_AVAIL_VARS){
        return;
    }

    //Serial.println(F("write var to eeprom"));
    EEPROM.write(VARS_MEMPOS+index,val);
    return;
}

/**\brief
* Set the value of a node internal variable.
* @param index variable index
* @param val variable value
*/
void MergMemoryManagement::setInternalVar(uint8_t index,byte val){
    if (index>INTERNAL_VARS){
        return;
    }
    EEPROM.write(NODE_INTERNAL_VAR + index,val);
    return;
}


/**\brief
* Update the variable of an event
* @param eventIdx Event index starting on 0.
* @param varIdx Variable index starting on 0.
* @param val Value of the variable.
* @return if success return the varIdx. if not return varIdx+1
*/
uint8_t MergMemoryManagement::setEventVar(unsigned int eventIdx,uint8_t varIdx,uint8_t val){

    if (eventIdx > numEvents){
        return (varIdx + 1);
    }

    if (varIdx > MAX_VAR_PER_EVENT){
        return (varIdx + 1);
    }
    //look the var in the array vars
    //eventVars[eventIdx*MAX_VAR_PER_EVENT+varIdx]=val;
    EEPROM.write(resolveEvVarArrayPos(eventIdx,varIdx),val);

    return varIdx;
}

/**\brief
* Set the can id.
* @param canId The can id
*/
void MergMemoryManagement::setCanId(byte canId){
    //Serial.println(F("MEM: Writing can id"));
    can_ID=canId;
    EEPROM.write(CAN_ID_MEMPOS,can_ID);
}

/**\brief
* Set the node number.
* @param val The node number (16 bit integer).
*/
void MergMemoryManagement::setNodeNumber(unsigned int val){
    //Serial.println(F("MEM: Writing NN"));
    nn[0]=highByte(val);
    nn[1]=lowByte(val);
    EEPROM.write(NN_MEMPOS,nn[0]);
    EEPROM.write(NN_MEMPOS+1,nn[1]);
    return;
}

/**\brief
* Set the device number.
* @param val The node number (16 bit integer).
*/
void MergMemoryManagement::setDeviceNumber(unsigned int val,byte port){

    if (port>MAX_NUM_DEVICE_NUMBERS){
        return;
    }
    EEPROM.write(DN_MEMPOS+port*NNDD_SIZE,highByte(val));
    EEPROM.write(DN_MEMPOS+port*NNDD_SIZE+1,lowByte(val));
    return;
}

/**\brief
* Get the device number.
* \param port The device port.
* \return The node number (16 bit integer).
*/
unsigned int MergMemoryManagement::getDeviceNumber(byte port){
    if (port >MAX_NUM_DEVICE_NUMBERS){
        return 0;
    }
    temp=((unsigned int)word(EEPROM.read(DN_MEMPOS+port*NNDD_SIZE),EEPROM.read(DN_MEMPOS+port*NNDD_SIZE+1)));
    return temp;
}

/**\brief
* Get the node number.
* @return The node number (16 bit integer).
*/

unsigned int MergMemoryManagement::getNodeNumber(){
    temp=((unsigned int)word(nn[0],nn[1]));
    return temp;
}


/**\brief
* Get the node mode retrieved from the flags.
* @return 0 if SLIM, 1 if FLIM
*/
byte MergMemoryManagement::getNodeFlag(){
    return flags;
}
/**\brief
* Set the node mode retrieved from the flags.
* @param mode 0 if SLIM, 1 if FLIM
*/
void MergMemoryManagement::setNodeFlag(byte val){
    flags=val;
    EEPROM.write(FLAGS_MEMPOS,flags);
}
/**\brief
* Print all memory to serial. Used for debug.
*/
void MergMemoryManagement::dumpMemory(){

/**\brief
* Find the event variable position in memory
*/
//#ifdef DEBUGDEF
    read();
    byte a;
    Serial.println(F("MEMORY DUMP"));
    Serial.print(F("IDENT:"));
    a=EEPROM.read(MERG_MEMPOS);
    Serial.print(a,HEX);
    Serial.print(F("\nCAN_ID:"));
    Serial.print(can_ID,HEX);
    Serial.print(F("\nNN:"));
    Serial.print(getNodeNumber());
    Serial.print(F("\nFlags:"));
    Serial.print(flags,HEX);
    Serial.print(F("\nNUM EVENTS:"));
    Serial.print(numEvents,HEX);
    Serial.print(F("\nNUM EVENTS VARS:"));
    Serial.print(MAX_VAR_PER_EVENT,HEX);
    Serial.print(F("\nNUM VARS:"));
    Serial.print(MAX_AVAIL_VARS,HEX);
    Serial.print(F("\nNODE VARS:"));
    for (uint8_t i=0;i<MAX_AVAIL_VARS;i++){
        Serial.print(EEPROM.read(VARS_MEMPOS+i),HEX);
        Serial.print(F(" "));
    }
    Serial.print(F("\nINTERNAL VARS:"));
    for (uint8_t i=0;i<INTERNAL_VARS;i++){
        Serial.print(EEPROM.read(NODE_INTERNAL_VAR+i),HEX);
        Serial.print(F(" "));
    }

    Serial.print(F("\nDEVICE NUMBERS:"));
    for (uint8_t i=0;i<MAX_NUM_DEVICE_NUMBERS;i++){
        Serial.print(EEPROM.read(DN_MEMPOS+i*NNDD_SIZE),HEX);
        Serial.print(EEPROM.read(DN_MEMPOS+i*NNDD_SIZE+1),HEX);
        Serial.print(F(" "));
    }

    Serial.println(F("\nEVENTS:"));
    int n=resolveEventPos(0);

    for (uint8_t i=0;i<MAX_NUM_EVENTS;i++){
        for (uint8_t j=0;j<EVENT_SIZE;j++){
            Serial.print(EEPROM.read(n),HEX);
            n++;
            Serial.print(F(" "));
        }
        Serial.print(F("VARS "));
        for (uint8_t j=0;j<MAX_VAR_PER_EVENT;j++){
            Serial.print(EEPROM.read(n),HEX);
            n++;
            Serial.print(F(" "));
        }
        Serial.println();
    }
//#endif // DEBUGDEF
}
unsigned int MergMemoryManagement::resolveEvVarArrayPos(byte evidx,byte varidx){

    return (resolveEventPos(evidx)+EVENT_SIZE+varidx);
}
/**\brief
* Increment the event position in memory
*/
unsigned int MergMemoryManagement::incEventPos(unsigned int val){
    return (val + MAX_VAR_PER_EVENT + EVENT_SIZE);
}
/**\brief
* Find the event position in memory
*/
unsigned int MergMemoryManagement::resolveEventPos(byte evidx){
    return (EVENTS_MEMPOS + evidx * (MAX_VAR_PER_EVENT + EVENT_SIZE));
}
