#include "MergMemoryManagement.h"

MergMemoryManagement::MergMemoryManagement()
{
    //ctor
    clear();
}
/*
// clear the arrays
*/
void MergMemoryManagement::clear(){

    int i=0;

    for (i=0;i<(MAX_NUM_EVENTS*4);i++){
        events[i]=0;
    }

    for (i=0;i<(MAX_AVAIL_VARS);i++){
        vars[i]=0;
    }

    for (i=0;i<(MAX_NUM_EVENTS_VAR*2);i++){
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
    //read events
    if (numEvents>0){
        n=0;
        pos=EVENTS_MEMPOS;
        while (n<(numEvents*4)){
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
    EEPROM.write(NUM_VARS_MEMPOS,numVars);
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
    if (numEvents>0){
        n=0;
        pos=EVENTS_MEMPOS;
        while (n<(numEvents*4)){
            EEPROM.write(pos,events[n]);
            n++;
            pos++;
        }
    }

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

void MergMemoryManagement::eraseAllEvents(){
    //to avoid to many eprom writings we just set the numberOfEvents to zero and the eventsVar to zero and clear the arrays

    //erase the events var
    //number of events
    byte val=0;
     EEPROM.write(NUM_EVENTS_MEMPOS,val);
    //number of events vars
     EEPROM.write(NUM_EVENTS_VARS_MEMPOS,val);

    for (int i=0;i<(MAX_NUM_EVENTS*4);i++){
        events[i]=0;
    }
    for (int i=0;i<(MAX_NUM_EVENTS_VAR*2);i++){
        eventVars[i].event_index=0;
        eventVars[i].index=0;
        eventVars[i].mem_index=0;
        eventVars[i].value=0;
        return_eventVars[i]=0;
    }
}

//[TODO]
void MergMemoryManagement::eraseEvent(int eventIdx);{
    //to avoid to many eprom writings we just set the numberOfEvents to zero and the eventsVar to zero and clear the arrays

    //erase the events var
    //number of events
    byte val=0;
     EEPROM.write(NUM_EVENTS_MEMPOS,val);
    //number of events vars
     EEPROM.write(NUM_EVENTS_VARS_MEMPOS,val);

    for (int i=0;i<(MAX_NUM_EVENTS*4);i++){
        events[i]=0;
    }
    for (int i=0;i<(MAX_NUM_EVENTS_VAR*2);i++){
        eventVars[i].event_index=0;
        eventVars[i].index=0;
        eventVars[i].mem_index=0;
        eventVars[i].value=0;
        return_eventVars[i]=0;
    }


}


