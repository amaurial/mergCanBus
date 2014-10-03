#ifndef MERGMEMORYMANAGEMENT_H
#define MERGMEMORYMANAGEMENT_H

#include <Arduino.h>
#include <EEPROM.h>


#define MAX_AVAIL_VARS 25           //each has 1 byte
#define MAX_NUM_EVENTS 30           //each event has 4 bytes
#define MAX_NUM_EVENTS_VAR 75       //each var has 2 bytes. first is the index, second is the value


#define MERG_MEMPOS 0                                   //has the value 0xaa to mark it is from this mod
#define CAN_ID_MEMPOS 1                                 //can id has 1 byte
#define NN_MEMPOS 2                                      //Node mumber has 2 bytes
#define DN_MEMPOS 4                                      //Device number has 2 bytes
#define NUM_VARS_MEMPOS 6                               //amount of variables for this module- 1 byte
#define NUM_EVENTS_MEMPOS 7                             //amount of learned events
#define NUM_EVENTS_VARS_MEMPOS 8                        //amount of learned events variables
#define VARS_MEMPOS 9                                   //start of variables written for this module
#define EVENTS_MEMPOS VARS_MEMPOS+1+MAX_AVAIL_VARS
#define EVENTS_VARS_MEMPOS EVENTS_MEMPOS+1+MAX_NUM_EVENTS*4


class MergMemoryManagement
{
    public:
        MergMemoryManagement();
        void read();
        void write();

        byte* getEvents(){return events;};
        byte* getEvent(int index);

        byte* getVars(){return vars;};
        byte getVar(int index);

        byte* getEventVars(){return eventVars;};
        byte getEventVar(byte eventIdx,int index);

        byte getNumVars(){return numVars;};
        byte getNumEvents(){return numEvents;};
        byte getNumEventVars(){return numEventVars;};


    protected:

    private:
        byte events[MAX_NUM_EVENTS*4];
        byte vars[MAX_AVAIL_VARS];
        byte eventVars[MAX_NUM_EVENTS_VAR*2];
        byte can_ID;
        byte nn[2];
        byte dd[2];
        byte numVars;
        byte numEvents;
        byte numEventVars;
        byte event[4];                                  //used to return this instead of a pointer to events array

        void clear();
};

#endif // MERGMEMORYMANAGEMENT_H
