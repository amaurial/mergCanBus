#include "MergMemoryManagement.h"

/**\brief
* Constructor: clear array and reads the EEPROM
*/
MergMemoryManagement::MergMemoryManagement()
{
    //ctor
    clear();
    nodeVariablesSize=MAX_AVAIL_VARS;
    eventVarsPerEvent=MAX_NUM_EVENTS_VAR_PER_EVENT;
    amountSuportedEvents=MAX_NUM_EVENTS;
    //read();
}
/**\brief
* Clear the EEPROM memory and the internal events.
* Should be called just in the first setup of the node and never inside a loop. May damage the EEPROM.
*/
void MergMemoryManagement::setUpNewMemory(){
    clear();
    write();
    numEvents=MAX_NUM_EVENTS;
    writeEvents();
    writeEventsVars();
    numEvents=0;
}

/**\brief
* Clear the internal buffers
*/
void MergMemoryManagement::clear(){

    int i=0;

    for (i=0;i<(MAX_NUM_EVENTS*EVENT_SIZE);i++){
        events[i]=EMPTY_BYTE;
    }

    for (i=0;i<(MAX_AVAIL_VARS);i++){
        vars[i]=EMPTY_BYTE;
    }

    for (i=0;i<(MAX_EVENTS_VAR_BUFFER);i++){
        eventVars[i]=EMPTY_BYTE;
    }

    for (i=0;i<(MAX_NUM_EVENTS_VAR_PER_EVENT);i++){
        return_eventVars[i]=0;
    }

    can_ID=0;
    nn[0]=EMPTY_BYTE;
    nn[1]=EMPTY_BYTE;
    dn[0]=EMPTY_BYTE;
    dn[1]=EMPTY_BYTE;
    numVars=0;
    numEvents=0;
    numEventVars=0;
    flags=0;

}

/**\brief
* Return the event pointed by the index
* If index out of bounds return empty event '0000'
* @return 4 bytes
*/

byte * MergMemoryManagement::getEvent(unsigned int index){
    event[0]=EMPTY_BYTE;
    event[1]=EMPTY_BYTE;
    event[2]=EMPTY_BYTE;
    event[3]=EMPTY_BYTE;

    Serial.println("MEM: Getting event");

    if (index>(amountSuportedEvents)||index>numEvents){
        return event;
    }

    event[0]=events[index];
    event[1]=events[index+1];
    event[2]=events[index+2];
    event[3]=events[index+3];

    return event;

}

/**\brief
* Put a new event in the memory and return the index
* @return event index starting with 0.
*/
unsigned int MergMemoryManagement::setEvent(byte *event){
    Serial.println("MEM: Setting event");
    if (numEvents>=amountSuportedEvents){
        return (MAX_NUM_EVENTS+1) ;
    }

    //check if the event exists
    unsigned int evidx;
    evidx=getEventIndex(event[0],event[1],event[2],event[3]);
    Serial.println("MEM: Getting event idx");
    if (evidx>MAX_NUM_EVENTS){
        //event does not exist
        //get the next index
        Serial.println("MEM: Saving new event");
        evidx=numEvents;
        //save the event
        return setEvent(event,evidx);
    }
    Serial.print("MEM: event idx:");
    Serial.println(evidx);
   return evidx;

}
/**\brief
* Put a new event in the memory and return its index.
* @param event 4 bytes event
* @param index event index starting in 0.
*/
unsigned int MergMemoryManagement::setEvent(byte *event,unsigned int index){

    if (index>=amountSuportedEvents){
        return (MAX_NUM_EVENTS+1) ;
    }
    int j;
    j=index*EVENT_SIZE;

    for (int i=0;i<EVENT_SIZE;i++){
            events[i+j]=event[i];
    }
    numEvents++;
    //number of events
     EEPROM.write(NUM_EVENTS_MEMPOS,numEvents);
     //write event

     j=resolveEvVarArrayPos(index,0)-EVENT_SIZE;
     for (int i=0;i<EVENT_SIZE;i++){
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
unsigned int MergMemoryManagement::getEventIndex(unsigned int nn,unsigned int ev){

    return getEventIndex (highByte(nn),lowByte(nn),highByte(ev),lowByte(ev));

}

/**\brief
* Return the internal index of an event using 4 bytes as parameters.
* The first 2 bytes are supposed to be the node number and the last 2 the event number
* @return event index starting on 0.
*/
unsigned int MergMemoryManagement::getEventIndex(byte ev1,byte ev2,byte ev3,byte ev4){
    int n=0;
    for (int i=0;i<numEvents;i++){
        if (ev1==events[n] &&
            ev2==events[n+1] &&
            ev3==events[n+2] &&
            ev4==events[n+3]){
            return i;
        }
        n=n+4;
    }

    return MAX_NUM_EVENTS+1;
}

/**\brief
* Return the node variable pointed by the index
* @return FAILED_INDEX if index is out of bounds
*/
byte MergMemoryManagement::getVar(unsigned int index){
    if (index>nodeVariablesSize||index>numVars){
        return FAILED_INDEX;
    }
    return vars[index];
}

/**\brief
* Return the event variable for a specific event
* @return FAILED_INDEX if index out of bounds
*/
byte MergMemoryManagement::getEventVar(unsigned int eventIdx,unsigned int index){
    //[TODO]
    if (eventIdx>numEvents){
        return FAILED_INDEX;
    }

    if (index>eventVarsPerEvent){
        return FAILED_INDEX;
    }

    //position in the array
    int i=resolveEvVarArrayPos(eventIdx,index);
    return eventVars[i];
    //return FAILED_INDEX;
}

/**\brief
* Get all variables of an specific event
* @param eventIdx is the event index starting on 0.
* @param len indicates how many variables
*/
byte* MergMemoryManagement::getEventVars(unsigned int eventIdx,unsigned int *len){
    //populate the array to return
    *len=0;
    int j=resolveEvVarArrayPos(eventIdx,0);
    for (int i=0;i<MAX_NUM_EVENTS_VAR_PER_EVENT;i++){
        return_eventVars[i]=eventVars[j];
        j++;
    }
    *len=MAX_NUM_EVENTS_VAR_PER_EVENT;
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
* 25 bytes for node variables
* 120 bytes for 30 learned events
* 150 bytes for learned event variables. 2 bytes for each variable. 1st is the event number index. the index is the position of the event in the memory range from 1 to 30
*/
void MergMemoryManagement::read(){

    clear();
    byte mergid=EEPROM.read(MERG_MEMPOS);

    if (mergid!=0xaa){
        write();
        return;
    }
    int pos=0;
    int n=0;
    int m=0;
    //read can id

    can_ID=EEPROM.read(CAN_ID_MEMPOS);
    //node number
    nn[0]=EEPROM.read(NN_MEMPOS);
    nn[1]=EEPROM.read(NN_MEMPOS+1);
    //node mode
    flags=EEPROM.read(FLAGS_MEMPOS);
    //device number
    dn[0]=EEPROM.read(DN_MEMPOS);
    dn[1]=EEPROM.read(DN_MEMPOS+1);
    //number of variables
    numVars=EEPROM.read(NUM_VARS_MEMPOS);
    //number of events
    numEvents=EEPROM.read(NUM_EVENTS_MEMPOS);
    //number of events
    numEventVars=EEPROM.read(NUM_EVENTS_VARS_MEMPOS);
    //read the variables
    if (numVars>0){
        pos=VARS_MEMPOS;
        while (n<numVars){
            vars[n]=EEPROM.read(pos);
            n++;
            pos++;
        }
    }
    //read events and events vars

    if (numEvents>0){
        n=0;
        m=0;
        pos=EVENTS_MEMPOS;
        if (numEvents>MAX_NUM_EVENTS){
            numEvents=MAX_NUM_EVENTS;
            EEPROM.write(NUM_EVENTS_MEMPOS,numEvents);
        }

        while (n<numEvents){
            //pos=n*(EVENT_SIZE+MAX_NUM_EVENTS_VAR_PER_EVENT)+EVENTS_MEMPOS;
            //read events
            for (int i=0;i<EVENT_SIZE;i++){
                events[n*EVENT_SIZE+i]=EEPROM.read(pos);
                pos++;
            }
            //read event vars
            for (int i=0;i<MAX_NUM_EVENTS_VAR_PER_EVENT;i++){
                eventVars[m]=EEPROM.read(pos);
                m++;
                pos++;
            }
            n++;
        }
    }
}

/**\brief
* Write all data to memory. it overwrites the all the previous information.
*/
void MergMemoryManagement::write(){

    int pos=0;
    //merg id
    EEPROM.write(MERG_MEMPOS,0xaa);
    //Can id
    EEPROM.write(CAN_ID_MEMPOS,can_ID);
    //node number
    EEPROM.write(NN_MEMPOS,nn[0]);
    EEPROM.write(NN_MEMPOS+1,nn[1]);
    //node mode
    EEPROM.write(FLAGS_MEMPOS,flags);
    //device number
    EEPROM.write(DN_MEMPOS,dn[0]);
    EEPROM.write(DN_MEMPOS+1,dn[1]);
    //number of variables
    //EEPROM.write(NUM_VARS_MEMPOS,numVars);
    //number of events
     EEPROM.write(NUM_EVENTS_MEMPOS,numEvents);
    //number of events vars
     EEPROM.write(NUM_EVENTS_VARS_MEMPOS,numEventVars);
     //write the variables
     byte n=0;
    if (numVars>0){
        pos=VARS_MEMPOS;
        while (n<numVars){
            EEPROM.write(pos,vars[n]);
            n++;
            pos++;
        }
    }
    //write events
    writeEvents();
    //write events vars
    writeEventsVars();

}
/**\brief
* Write the events to the memory.
*/
void MergMemoryManagement::writeEvents(){
     //write events
    int n;
    int pos;
    if (numEvents>0){
        n=0;
        pos=EVENTS_MEMPOS;
        while (n<numEvents){
            for (int i=0;i<EVENT_SIZE;i++){
                EEPROM.write(pos,events[n]);
                n++;
                pos++;
            }
            pos=pos+MAX_NUM_EVENTS_VAR_PER_EVENT;
        }
    }
}
/**\brief
* Write the events variables to the memory.
*/
void MergMemoryManagement::writeEventsVars(){
     //write events vars
    int n;
    int pos;
    if (numEvents>0){
        n=0;//array index
        pos=resolveEvVarArrayPos(n,0);
        while (n<(numEvents)){
            for (int i=0;i<MAX_NUM_EVENTS_VAR_PER_EVENT;i++){
                EEPROM.write(pos,eventVars[n*MAX_NUM_EVENTS_VAR_PER_EVENT+i]);
                pos++;
            }
            n++;
        }
    }
}
/**\brief
* Erase all events and their variables.
*/

void MergMemoryManagement::eraseAllEvents(){
    //to avoid to many eprom writings we just set the numberOfEvents to zero and the eventsVar to zero and clear the arrays

    //erase the events var
    //number of events
     EEPROM.write(NUM_EVENTS_MEMPOS,0);
    //number of events vars
    for (int i=0;i<(MAX_NUM_EVENTS*EVENT_SIZE);i++){
        events[i]=EMPTY_BYTE;
    }
    for (int i=0;i<(MAX_EVENTS_VAR_BUFFER);i++){
        eventVars[i]=EMPTY_BYTE;
    }
    numEventVars=0;
    numEvents=0;

}

/**\brief
* Erase a specific event by the event index.
* Has to reorganize the memory: events and events vars to avoid fragmentation.
* @param eventIdx event index starting on 0.
*/
unsigned int MergMemoryManagement::eraseEvent(unsigned int eventIdx){

    if (eventIdx>numEvents||numEvents<1){
        return (MAX_NUM_EVENTS +1);
    }

    //reorganize the arrays
    //erase event on the array
    int j,i;
    int pos=eventIdx*EVENT_SIZE;

    //move the other events to one position less
    //if it is the last event don't need organize the array
    for (i=eventIdx;i<(numEvents-1);i++){
        for (j=0;j<EVENT_SIZE;j++){
            events[pos]=events[pos+EVENT_SIZE];
            pos++;
        }
    }

    //organize the events_var array
    //each var belonging to an event greater or equal the event index being erased has to have the index decreased

    pos=eventIdx*MAX_NUM_EVENTS_VAR_PER_EVENT;
    i=eventIdx;
    while (i<(numEvents-1)){
        for (j=0;j<MAX_NUM_EVENTS_VAR_PER_EVENT;j++){
            eventVars[pos]=eventVars[pos+MAX_NUM_EVENTS_VAR_PER_EVENT];
            pos++;
        }
        i++;
    }

    //write events to eprom
    numEvents--;
     //write number of events
    EEPROM.write(NUM_EVENTS_MEMPOS,numEvents);
    writeEvents();
    writeEventsVars();
    return eventIdx;
}

/**\brief
* Erase a specific event.
* Has to reorganize the memory: events and events vars to avoid fragmentation.
* @param nn node number (16 bits integer)
* @param ev event number (16 bits integer)
*/
unsigned int MergMemoryManagement::eraseEvent(unsigned int nn,unsigned int ev){

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
void MergMemoryManagement::setVar(unsigned int index,byte val){
    if (index<0 || index>nodeVariablesSize){
        return;
    }
    vars[index]=val;
    EEPROM.write(VARS_MEMPOS+index,val);
    return;
}

/**\brief
* Update the variable of an event
* @param eventIdx Event index starting on 0.
* @param varIdx Variable index starting on 0.
* @param val Value of the variable.
* @return if success return the varIdx. if not return varIdx+1
*/
unsigned int MergMemoryManagement::setEventVar(unsigned int eventIdx,unsigned int varIdx,byte val){


    if (eventIdx<0 || eventIdx>numEvents){
        return (varIdx+1);
    }

    if (varIdx<0 || varIdx>eventVarsPerEvent){
        return (varIdx+1);
    }

    //look the var in the array vars
    eventVars[eventIdx*MAX_NUM_EVENTS_VAR_PER_EVENT+varIdx]=val;
    EEPROM.write(resolveEvVarArrayPos(eventIdx,varIdx),val);

    return varIdx;
}

/**\brief
* Set the can id.
* @param canId The can id
*/
void MergMemoryManagement::setCanId(byte canId){
    Serial.println("MEM: Writing can id");
    can_ID=canId;
    EEPROM.write(CAN_ID_MEMPOS,can_ID);
}

/**\brief
* Set the node number.
* @param val The node number (16 bit integer).
*/
void MergMemoryManagement::setNodeNumber(unsigned int val){
    Serial.println("MEM: Writing NN");
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
void MergMemoryManagement::setDeviceNumber(unsigned int val){
    dn[0]=highByte(val);
    dn[1]=lowByte(val);
    EEPROM.write(DN_MEMPOS,dn[0]);
    EEPROM.write(DN_MEMPOS+1,dn[1]);
    return;
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
* Get the device number.
* @return The node number (16 bit integer).
*/
unsigned int MergMemoryManagement::getDeviceNumber(){
    temp=((unsigned int)word(dn[0],dn[1]));
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

    //read();
    byte a;
    Serial.println("MEMORY DUMP");
    Serial.print("IDENT:");
    a=EEPROM.read(MERG_MEMPOS);
    Serial.print(a,HEX);
    Serial.print("\nCAN_ID:");
    Serial.print(can_ID,HEX);
    Serial.print("\nNN:");
    Serial.print(getNodeNumber());
    Serial.print("\nDN:");
    Serial.print(dn[0],HEX);
    Serial.print(dn[1],HEX);
    Serial.print("\nFlags:");
    Serial.print(flags,HEX);
    Serial.print("\nNUM EVENTS:");
    Serial.print(numEvents,HEX);
    Serial.print("\nNUM EVENTS VARS:");
    Serial.print(numEventVars,HEX);
    Serial.print("\nNUM VARS:");
    Serial.print(numVars,HEX);
    Serial.print("\nNODE VARS:");
    for (int i=0;i<MAX_AVAIL_VARS;i++){
        Serial.print(vars[i],HEX);
        Serial.print(" ");
    }

    Serial.println("\nEVENTS:");
    int n=0;
    int m=0;
    for (int i=0;i<MAX_NUM_EVENTS;i++){
        for (int j=0;j<EVENT_SIZE;j++){
            Serial.print(events[n],HEX);
            n++;
            Serial.print(" ");
        }
        for (int j=0;j<MAX_NUM_EVENTS_VAR_PER_EVENT;j++){
            Serial.print(eventVars[m],HEX);
            m++;
            Serial.print(" ");
        }
        Serial.println();
    }


}
unsigned int MergMemoryManagement::resolveEvVarArrayPos(byte evidx,byte varidx){

    return (EVENTS_MEMPOS+evidx*(MAX_NUM_EVENTS_VAR_PER_EVENT+EVENT_SIZE)+EVENT_SIZE+varidx);
}
