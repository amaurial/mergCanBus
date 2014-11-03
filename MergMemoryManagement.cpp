#include "MergMemoryManagement.h"

MergMemoryManagement::MergMemoryManagement()
{
    //ctor
    clear();
    read();
}
/*
// clear the arrays
*/
void MergMemoryManagement::clear(){

    int i=0;

    for (i=0;i<(MAX_NUM_EVENTS*EVENT_SIZE);i++){
        events[i]=0;
    }

    for (i=0;i<(MAX_AVAIL_VARS);i++){
        vars[i]=0;
    }

    for (i=0;i<(MAX_NUM_EVENTS_VAR*EVENT_VARS_SIZE);i++){
        eventVars[i].event_index=0;
        eventVars[i].index=0;
        eventVars[i].mem_index=0;
        eventVars[i].value=0;
        return_eventVars[i]=0;
    }

    can_ID=0;
    nn[0]=0;
    nn[1]=0;
    dd[0]=0;
    dd[1]=0;
    numVars=0;
    numEvents=0;
    numEventVars=0;

}

/*
// return the event pointed by the index
// if index out of bounds return empty event '0000'
*/

byte * MergMemoryManagement::getEvent(int index){
    event[0]=0;
    event[1]=0;
    event[2]=0;
    event[3]=0;

    if (index>(NUM_EVENTS_MEMPOS)||index>numEvents){
        return event;
    }

    event[0]=events[index];
    event[1]=events[index+1];
    event[2]=events[index+2];
    event[3]=events[index+3];

    return event;

}

/*
//put a new event in the memory and return the index
*/
unsigned int MergMemoryManagement::setEvent(byte *event){
    if (numEvents>=MAX_NUM_EVENTS){
        return (MAX_NUM_EVENTS+1) ;
    }

    //check if the event exists
    //TODO
    unsigned int evidx,resp;
    evidx=getEventIndex(event[0],event[1],event[2],event[3]);

    if (evidx<0){
        //event does not exist
        //get the next index
        evidx=numEvents+1;
        return setEvent(event,evidx);
    }
   return evidx;

}
/*
//put a new event in the memory
*/
unsigned int MergMemoryManagement::setEvent(byte *event,unsigned int index){

    if (index>=MAX_NUM_EVENTS){
        return (MAX_NUM_EVENTS+1) ;
    }

    for (int i=0;i<EVENT_SIZE;i++){
            events[index+i]=event[i];
    }
    numEvents++;
    //number of events
     EEPROM.write(NUM_EVENTS_MEMPOS,numEvents);
     //write event
     for (int i=0;i<EVENT_SIZE;i++){
        EEPROM.write(EVENTS_MEMPOS+index+i,event[i]);
     }
     return index;
}

unsigned int MergMemoryManagement::getEventIndex(byte ev1,byte ev2,byte ev3,byte ev4){
    int n=0;
    for (int i=0;i<numEvents;i++){
        if (ev1==events[n] &&
            ev2==events[n+1] &&
            ev3==events[n+2] &&
            ev4==events[n+3] &&){
            return i;
        }
        n=n+4;
    }

    return -1;
}

/*
// return the node variable pointed by the index
// return 0 if index out of bounds
*/
byte MergMemoryManagement::getVar(int index){
    if (index>NUM_VARS_MEMPOS||index>numVars){
        return 0;
    }
    return vars[index];
}

/*
// return the event variable for a specific event
// return 0 if index out of bounds
*/
byte MergMemoryManagement::getEventVar(int eventIdx,int index){
    //[TODO]
    if (eventIdx>(NUM_EVENTS_MEMPOS)||eventIdx>numEvents){
        return 0;
    }

    if (index>NUM_VARS_MEMPOS||index>numVars){
        return 0;
    }

    for (int i=0;i<MAX_NUM_EVENTS_VAR;i++){
        if (eventVars[i].event_index==eventIdx && eventVars[i].index==index){
            return eventVars[i].value;
        }
    }

    return 0;
}

/*
// get all variables of an specific event
// len indicates how many variables
*/
byte* MergMemoryManagement::getEventVars(int eventIdx,int &len){
    //populate the array to return
    len=0;
    for (int i=0;i<MAX_NUM_EVENTS_VAR;i++){
        if (eventVars[i].event_index==eventIdx ){
            return_eventVars[eventVars[i].index]=eventVars[i].value;
            len++;
        }
    }
    return return_eventVars;
}

/*
// read all data from eprom memory
Memory organization
512 bytes is the max value for simple arduino
byte=value;explanation
1=0xaa ; indicates that it is a merg arduino memory storage
2=can_id;store the can_id
3=hh Node number;high byte from node number
4=ll Node number;low byte from node number
5=hh device number;high byte from node number
6=ll device number;low byte from node number
7=amount of node variables
8=amount of learned events
9=amount of learned event variables
10-35=25 bytes for node variables
36-156=120 bytes for 30 learned events
157-307=150 bytes for learned event variables. 2 bytes for each variable. 1st is the event number index. the index is the position of the event in the memory range from 1 to 30
*/
void MergMemoryManagement::read(){

    clear();
    byte mergid=EEPROM.read(0);

    if (mergid!=0xaa){
        return;
    }
    int pos=0;
    byte n=0;
    //read can id
    can_ID=EEPROM.read(CAN_ID_MEMPOS);
    //node number
    nn[0]=EEPROM.read(NN_MEMPOS);
    nn[1]=EEPROM.read(NN_MEMPOS+1);
    //device number
    dd[0]=EEPROM.read(DN_MEMPOS);
    dd[1]=EEPROM.read(DN_MEMPOS+1);
    //number of variables
    //numVars=EEPROM.read(NUM_VARS_MEMPOS);
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
    //read events
    if (numEvents>0){
        n=0;
        pos=EVENTS_MEMPOS;
        while (n<(numEvents*EVENT_SIZE)){
            events[n]=EEPROM.read(pos);
            n++;
            pos++;
        }
    }
     //read events vars
     //index the value in the array for fast search
     byte i=0;
     int positions[MAX_NUM_EVENTS];
     for (int j=0;j<MAX_NUM_EVENTS;j++){
        positions[j]=0;
     }

    if (numEventVars>0){
        n=0;//array index
        pos=EVENTS_VARS_MEMPOS;//memory index
        while (n<(numEventVars)){
            i=EEPROM.read(pos);
            eventVars[n].mem_index=pos;
            pos++;
            //eventVars[i]=EEPROM.read(pos);
            eventVars[n].event_index=i;
            eventVars[n].value=EEPROM.read(pos);
            eventVars[n].index=positions[i];
            positions[i]=positions[i]+1;
            pos++;
            n++;
        }
    }
}

/*
// write all data to memory. it overwrites the hole information
*/
void MergMemoryManagement::write(){

    int pos=0;
    //merg id
    EEPROM.write(0,0xaa);
    //Can id
    EEPROM.write(CAN_ID_MEMPOS,can_ID);
    //node number
    EEPROM.write(NN_MEMPOS,nn[0]);
    EEPROM.write(NN_MEMPOS+1,nn[1]);
    //device number
    EEPROM.write(DN_MEMPOS,dd[0]);
    EEPROM.write(DN_MEMPOS+1,dd[1]);
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
    if (numEventVars>0){
        n=0;//array index
        pos=EVENTS_VARS_MEMPOS;//memory index
        while (n<(numEventVars)){
            EEPROM.write(pos,eventVars[n].event_index);
            pos++;
            EEPROM.write(pos,eventVars[n].value);
            pos++;
            n++;
        }
    }


}
/*
/*Write the events to the memory
*/
void MergMemoryManagement::writeEvents(){
     //write events
    int n;
    int pos;
    if (numEvents>0){
        n=0;
        pos=EVENTS_MEMPOS;
        while (n<(numEvents*EVENT_SIZE)){
            EEPROM.write(pos,events[n]);
            n++;
            pos++;
        }
    }
}


void MergMemoryManagement::eraseAllEvents(){
    //to avoid to many eprom writings we just set the numberOfEvents to zero and the eventsVar to zero and clear the arrays

    //erase the events var
    //number of events
    byte val=0;
     EEPROM.write(NUM_EVENTS_MEMPOS,val);
    //number of events vars
    if (numEventVars>0){
        EEPROM.write(NUM_EVENTS_VARS_MEMPOS,val);
        for (int i=0;i<(MAX_NUM_EVENTS_VAR*EVENT_VARS_SIZE);i++){
            eventVars[i].event_index=0;
            eventVars[i].index=0;
            eventVars[i].mem_index=0;
            eventVars[i].value=0;
            return_eventVars[i]=0;
        }
    }

    for (int i=0;i<(MAX_NUM_EVENTS*EVENT_SIZE);i++){
        events[i]=0;
    }

}

/*
/* Erase a specific event
/* Has to reorganize the memory: events and events vars to avoid fragmentation
*/
unsigned int MergMemoryManagement::eraseEvent(int eventIdx){

    if (eventIdx>numEvents||numEvents<1){
        return (MAX_NUM_EVENTS +1);
    }
    numEvents--;
    //reorganize the arrays
    //erase event on the array
    int j,i;
    int pos=eventIdx*EVENT_SIZE;
    for (j=pos;j<EVENT_SIZE;j++){
        events[j]=0;
        j++;
    }
    //move the other events to one position less
    //if it is the last event don't need organize the array
    if (eventIdx!=numEvents+1){
        for (i=eventIdx;i<numEvents;i++){
                for (j=0;j<EVENT_SIZE;j++){
                    events[pos]=events[pos+EVENT_SIZE];
                    pos++;
                }
        }
    }
    //clear the last not moved event
    for (j=pos;j<EVENT_SIZE;j++){
        events[pos]=0;
        pos++;
    }

     //write number of events
    EEPROM.write(NUM_EVENTS_MEMPOS,numEvents);

    //write events to eprom
    writeEvents();


    if (numEventVars<1){
        //no events vars to deal with
        return eventIdx;
    }

    //organize the events_var array
    //each var belonging to an event greater or equal the event index being erased has to have the index decreased
    byte val=0;
    bool f=false;
    for (int i=0;i<numEventVars;i++){

        if (eventVars[i].event_index==eventIdx){
            //clear the index on the eprom
            EEPROM.write(eventVars[i].mem_index,val);
            f=true;
        }
        if (eventVars[i].event_index>eventIdx){
            //decrease the other indexes of vars
            eventVars[i].event_index--;
            EEPROM.write(eventVars[i].mem_index,eventVars[i].event_index);
        }
    }

    //write number of events vars
    if (f){
        //there were events vars
        EEPROM.write(NUM_EVENTS_VARS_MEMPOS,numEventVars);
    }

    //write the new indexes to the memory
    for (int i=0;i<numEventVars;i++){
        if (eventVars[i].event_index>=eventIdx){
                EEPROM.write(eventVars[i].mem_index,eventVars[i].event_index);
        }
    }
    return eventIdx;
}


unsigned int MergMemoryManagement::eraseEvent(unsigned int ev){

    //find the event in the memory and erase
    int index=getEventIndex(ev);
    if (index>=0){
        return eraseEvent(index);
    }
    return (MAX_NUM_EVENTS+1);
}

int MergMemoryManagement::getEventIndex(unsigned int ev){

    byte highev,lowev,pos;
    unsigned int evtmp;
    pos=0;
    for (int i=0;i<numEvents;i++){
        highev=events[pos];
        lowev=events[pos+1];
        evtmp=highev;
        evtmp<<8;
        evtmp=evtmp && lowev;

        if (evtmp==ev){
            return i;
        }

        pos=pos+4;
    }
    //not found
    return -1;

}
/*
/* set the value of a node var
*/
void MergMemoryManagement::setVar(int index,byte val){
    if (index<0 || index>MAX_AVAIL_VARS){
        return;
    }
    vars[index]=val;
    EEPROM.write(VARS_MEMPOS+index,val);
    return;
}

/*
/* Update the var of an event
/* if success return the varIdx. if not return varIdx+1
*/
unsigned int MergMemoryManagement::setEventVar(unsigned int eventIdx,unsigned int varIdx,byte val){

    bool exist=false;

    if (eventIdx<0 || eventIdx>numEvents){
        return (varIdx+1);
    }

    if (varIdx<0 || varIdx>numEventVars){
        return (varIdx+1);
    }

    //look the var in the array vars
    for (int i=0;i<numEventVars;i++){
        if (eventVars[i].event_index==eventIdx && eventVars[i].index==varIdx){
            eventVars[i].value=val;
            EEPROM.write(eventVars[i].mem_index,val);
            exist=true;
        }
    }

    if (!exist){
        newEventVar(eventIdx,varIdx,val);
    }

    return varIdx;
}
/*
/* create a new var for an event
*/
void MergMemoryManagement::newEventVar(unsigned int eventIdx,unsigned int varIdx,byte val){

    eventVars[numEventVars].event_index=eventIdx;
    eventVars[numEventVars].index=varIdx;
    eventVars[numEventVars].value=val;
    eventVars[numEventVars].mem_index=numEventVars+EVENTS_VARS_MEMPOS;

    EEPROM.write(eventVars[numEventVars].mem_index,eventIdx);
    EEPROM.write(eventVars[numEventVars].mem_index+1,val);


    numEventVars++;
    EEPROM.write(NUM_EVENTS_VARS_MEMPOS,numEventVars);

}

/*
/* set the can id
*/
void MergMemoryManagement::setCanId(byte canId){
    can_ID=canId;
    EEPROM.write(CAN_ID_MEMPOS,can_ID);
}

/*
/* set the node number
*/
void MergMemoryManagement::setNodeNumber(unsigned int val){
    nn[0]=highByte(val);
    nn[1]=lowByte(val);
    EEPROM.write(NN_MEMPOS,nn[0]);
    EEPROM.write(NN_MEMPOS+1,nn[1]);
    return;
}

/*
/* set the device number
*/
void MergMemoryManagement::setDeviceNumber(unsigned int val){
    dd[0]=highByte(val);
    dd[1]=lowByte(val);
    EEPROM.write(DN_MEMPOS,dd[0]);
    EEPROM.write(DN_MEMPOS+1,dd[1]);
    return;
}

unsigned int MergMemoryManagement::getNodeNumber(){
    return ((unsigned int)word(nn[0],nn[1]));
}
unsigned int MergMemoryManagement::getDeviceNumber(){
    return ((unsigned int)word(dd[0],dd[1]));
}
bool MergMemoryManagement::hasEventVars(int eventIdx){
    for (int i=0;i<numEventVars;i++){
        if (eventVars[i].event_index==eventIdx){
            return true;
        }
    }
    return false;
}

