#include "MergMemoryManagement.h"

MergMemoryManagement::MergMemoryManagement()
{
    //ctor
    clear();
}

void MergMemoryManagement::clear(){

    int i=0;

    for (i=0;i<(MAX_NUM_EVENTS*4);i++){
        events[i]=0;
    }

    for (i=0;i<(MAX_AVAIL_VARS);i++){
        vars[i]=0;
    }

    for (i=0;i<(MAX_NUM_EVENTS_VAR*2);i++){
        eventVars[i]=0;
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

byte MergMemoryManagement::getVar(int index){
    if (index>NUM_VARS_MEMPOS||index>numVars){
        return 0;
    }
    return vars[index];
}

byte MergMemoryManagement::getEventVar(int eventIdx,int index){
    //[TODO]

    if (index>NUM_VARS_MEMPOS||index>numVars){
        return 0;
    }
    return vars[index];
}

void MergMemoryManagement::read(){

    clear();
    if (EEPROM.read(0)!=0xaa){
        return;
    }
    int pos=0;
    byte n=0;
    //read can id
    can_ID=EEPROM.read(CAN_ID_MEMPOS);
    //node number
    nn[0]=EEPROM.read(NN_MEMPOS);
    nn[0]=EEPROM.read(NN_MEMPOS+1);
    //device number
    dd[0]=EEPROM.read(DD_MEMPOS);
    dd[0]=EEPROM.read(DD_MEMPOS+1);
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
     //[TODO]:correct
     byte i=0;
    if (numEventVars>0){
        n=0;
        pos=EVENTS_VARS_MEMPOS;
        while (n<(numEventVars)){
            i=EEPROM.read(pos);
            pos++;
            eventVars[i]=EEPROM.read(pos);
            pos++;
            n++;
        }
    }

}

void MergMemoryManagement::write(){


}
