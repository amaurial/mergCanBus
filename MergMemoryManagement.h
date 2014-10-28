#ifndef MERGMEMORYMANAGEMENT_H
#define MERGMEMORYMANAGEMENT_H

#include <Arduino.h>
#include <EEPROM.h>

struct merg_event_vars{
    unsigned int index;
    unsigned int event_index;
    unsigned int mem_index;
    byte value;
};

#define MAX_AVAIL_VARS 25           //each has 1 byte
#define MAX_NUM_EVENTS 30           //each event has 4 bytes
#define MAX_NUM_EVENTS_VAR 75       //each var has 2 bytes. first is the index, second is the value
#define EVENT_SIZE 4
#define EVENT_VARS_SIZE 2


#define MERG_MEMPOS 0                                   //has the value 0xaa to mark it is from this mod
#define CAN_ID_MEMPOS 1                                 //can id has 1 byte
#define NN_MEMPOS 2                                      //Node mumber has 2 bytes
#define DN_MEMPOS 4                                      //Device number has 2 bytes
#define NUM_VARS_MEMPOS 6                               //amount of variables for this module- 1 byte
#define NUM_EVENTS_MEMPOS 7                             //amount of learned events
#define NUM_EVENTS_VARS_MEMPOS 8                        //amount of learned events variables
#define VARS_MEMPOS 9                                   //start of variables written for this module
#define EVENTS_MEMPOS VARS_MEMPOS+1+MAX_AVAIL_VARS
#define EVENTS_VARS_MEMPOS EVENTS_MEMPOS+1+MAX_NUM_EVENTS*EVENT_SIZE


class MergMemoryManagement
{
    public:
        MergMemoryManagement();
        void read();
        void write();

        bool hasEvents(){numEvents>0?true:false;};
        bool hasEventVars(){numEventVars>0?true:false;};
        bool hasEventVars(int eventIdx);

        byte* getEvents(){return events;};
        byte* getEvent(int index);

        byte* getVars(){return vars;};
        byte getVar(int index);

        byte* getEventVars();
        byte getEventVar(int eventIdx,int index);
        byte *getEventVars(int eventIdx,int &len);

        byte getNumVars(){return numVars;};
        byte getNumEvents(){return numEvents;};
        byte getNumEventVars(){return numEventVars;};

        void eraseAllEvents();
        void eraseEvent(int eventIdx);
        void eraseEvent(byte event[EVENT_SIZE]);
        int eraseEvent(unsigned int event);


        void setVar(int index,byte val);
        void setCanId(byte canId);
        void setNodeNumber(unsigned int val);                       //2 bytes representation
        void setDeviceNumber(unsigned int val);                     //2 bytes representation

        byte getCanId(){return can_ID;};
        unsigned int getNodeNumber();
        unsigned int getDeviceNumber();

        void setEvent(byte *event);
        void setEvent(byte *event,int eventIdx);

        void newEventVar(int eventIdx,byte val);
        void setEventVar(int eventIdx,int index, byte value);

        int getEventIndex(unsigned int event);


    protected:

    private:
        byte events[MAX_NUM_EVENTS*EVENT_SIZE];
        byte vars[MAX_AVAIL_VARS];
        merg_event_vars eventVars[MAX_NUM_EVENTS_VAR];
        byte return_eventVars[MAX_NUM_EVENTS_VAR];                  //used to return all the variables of an event
        byte can_ID;
        byte nn[2];
        byte dd[2];
        byte numVars;
        byte numEvents;
        byte numEventVars;
        byte event[EVENT_SIZE];                                     //used to return this instead of a pointer to events array

        void clear();
        void writeEvents();


};

#endif // MERGMEMORYMANAGEMENT_H
