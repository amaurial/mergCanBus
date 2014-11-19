#ifndef MERGMEMORYMANAGEMENT_H
#define MERGMEMORYMANAGEMENT_H

#include <Arduino.h>
#include <EEPROM.h>

/** \brief Structure that contains the event variables.
 *
 * \param
 * \param
 * \return
 *
 */

struct merg_event_vars{
    unsigned int index;/**< index of this variable */
    unsigned int event_index;/**< index of the event this variable belongs */
    unsigned int mem_index;/**< position in the eeprom */
    byte value;/**< value of the variable */
};

#define MAX_AVAIL_VARS 20               /** Number of node variables. Each has 1 byte                           */
#define MAX_NUM_EVENTS 20               /** Number of supported events. Each event has 4 bytes                  */
#define MAX_NUM_EVENTS_VAR_PER_EVENT 8  /** Total amount of events variables per event. Each var has 2 bytes. first is the index, second is the value        */
#define MAX_EVENTS_VAR_BUFFER 376       /** Total amount of events variables that can be stored. Each var has 2 bytes. first is the index, second is the value        */
#define MAX_NUM_DEVICE_NUMBERS 12       /** The total number of device number if it is a producer node. Each input from the producer can be a device number. So this is the same as the total number of inputs*/
#define EVENT_SIZE 4                    /** Event size.                                                        */
#define EVENT_VARS_SIZE 2               /** Size for events vars. Each var has 2 bytes. first is the index, second is the value        */
#define NNDD_SIZE 2                     /** Size of a node number or device number */
#define MAX_NUM_EV_VARS (unsigned int) MAX_EVENTS_VAR_BUFFER/MAX_NUM_EVENTS_VAR_PER_EVENT
#define EMPTY_BYTE 0xff

#define FAILED_INDEX 255                /** Value returned when vars or events vars not found                             */

#define MERG_MEMPOS 0                                                  /** Position in memory. has the value 0xaa to mark it is from this mod       */
#define CAN_ID_MEMPOS MERG_MEMPOS+1                                    /**Position in memory.can id has 1 byte                                    */
#define NN_MEMPOS CAN_ID_MEMPOS+1                                      /**Position in memory.Node mumber has 2 bytes                               */
#define FLAGS_MEMPOS NN_MEMPOS+NNDD_SIZE                           /**Position in memory.flags                          */
#define DN_MEMPOS FLAGS_MEMPOS+1                                     /**Position in memory.Device number has 2 bytes                            */
#define NUM_VARS_MEMPOS DN_MEMPOS+NNDD_SIZE                          /**Position in memory.amount of variables for this module- 1 byte          */
//#define NUM_VARS_MEMPOS DN_MEMPOS+1+MAX_NUM_DEVICE_NUMBERS*NNDD_SIZE   /**Position in memory.amount of variables for this module- 1 byte          */
#define NUM_EVENTS_MEMPOS NUM_VARS_MEMPOS+1                            /**Position in memory.amount of learned events                             */
#define NUM_EVENTS_VARS_MEMPOS NUM_EVENTS_MEMPOS+1                     /**Position in memory.amount of learned events variables                   */
#define VARS_MEMPOS NUM_EVENTS_VARS_MEMPOS+1                           /**Position in memory.start of variables written for this module           */
#define EVENTS_MEMPOS VARS_MEMPOS+MAX_AVAIL_VARS                     /**Position in memory.                                                     */
#define EVENTS_VARS_MEMPOS EVENTS_MEMPOS+MAX_NUM_EVENTS*EVENT_SIZE   /**Position in memory.                                                    */

/**
* Class that handles the EEPROM and maintains the data in a RAM memory.
*/

class MergMemoryManagement
{
    public:
        MergMemoryManagement();
        void read();
        void write();
        /** \brief Check if there is some learned event.*/
        bool hasEvents(){return (numEvents>0?true:false);};

        /** \brief Check if there is some learned event variables.*/
        //bool hasEventVars(){return (numEventVars>0?true:false);};

        bool hasEventVars(unsigned int eventIdx);

        /** \brief Return the array of events.*/
        byte* getEvents(){return events;};

        byte* getEvent(unsigned int index);
        /** \brief Return the array of node variables.*/
        byte* getVars(){return vars;};

        byte getVar(unsigned int index);

        //byte* getEventVars();
        byte getEventVar(unsigned int eventIdx,unsigned int index);
        byte *getEventVars(unsigned int eventIdx,unsigned int &len);
        /** \brief Return the number of set node variables.*/
        byte getNumVars(){return numVars;};
        /** \brief Return the number of learned events.*/
        byte getNumEvents(){return numEvents;};
        /** \brief Return the number of learned events variables.*/
        byte getNumEventVars(){return numEventVars;};

        void eraseAllEvents();

        unsigned int  eraseEvent(unsigned int eventIdx);
        //unsigned int  eraseEvent(byte event[EVENT_SIZE]);
        unsigned int  eraseEvent(unsigned int nn,unsigned int ev);


        void setVar(unsigned int index,byte val);
        void setCanId(byte canId);
        void setNodeNumber(unsigned int val);                       //2 bytes representation
        void setDeviceNumber(unsigned int val);                     //2 bytes representation

        //TODO:implement support for multiple device numbers
        void setDeviceNumber(unsigned int val,byte port);           //2 bytes representation for dn
        void getDeviceNumber(byte port);                             //2 bytes representation
        byte getNumDeviceNumber();                                  //2 bytes representation


        byte getCanId(){return can_ID;};
        unsigned int getNodeNumber();
        unsigned int getDeviceNumber();

        unsigned int setEvent(byte *event);//return the index
        unsigned int setEvent(byte *event,unsigned int eventIdx);

        unsigned int setEventVar(unsigned int eventIdx,unsigned int varIdx,byte val);

        unsigned int getEventIndex(byte ev1,byte ev2,byte ev3,byte ev4);
        unsigned int getEventIndex(unsigned int nn,unsigned int ev);

        byte getNodeFlag();
        void setNodeFlag(byte val);

        void setUpNewMemory();
        void dumpMemory();

        void setNodeVariablesSize(byte val){nodeVariablesSize=val;};
        void setEventVarsPerEvent(byte val){eventVarsPerEvent=val;};
        void setAmountSuportedEvents(byte val){amountSuportedEvents=val;};

        byte getNodeVariablesSize(){return nodeVariablesSize;};
        byte getEventVarsPerEvent(){return eventVarsPerEvent;};
        byte getAmountSuportedEvents(){return amountSuportedEvents;};


    protected:

    private:
        byte events[MAX_NUM_EVENTS*EVENT_SIZE];
        byte vars[MAX_AVAIL_VARS];
        //merg_event_vars eventVars[MAX_NUM_EV_VARS];
        byte eventVars[MAX_NUM_EVENTS_VAR_PER_EVENT*MAX_NUM_EVENTS];
        byte return_eventVars[MAX_NUM_EVENTS_VAR_PER_EVENT];                  //used to return all the variables of an event
        byte can_ID;
        byte dns[NNDD_SIZE*MAX_NUM_DEVICE_NUMBERS];                 //array of device numbers
        byte nn[NNDD_SIZE];
        byte dn[NNDD_SIZE];
        byte numVars;
        byte numEvents;
        byte numEventVars;
        byte numDeviceNumbers;
        byte event[EVENT_SIZE];                                     //used to return this instead of a pointer to events array
        byte flags;
        void clear();
        void writeEvents();
        void writeEventsVars();
        //void newEventVar(unsigned int eventIdx,unsigned int varIdx,byte val);
        unsigned int temp;                                          //used to avoid new memory allocation
        byte nodeVariablesSize;     //max supported number of node variables
        byte eventVarsPerEvent;     //number of supported vars per event
        byte amountSuportedEvents;  //number of supported events


};

#endif // MERGMEMORYMANAGEMENT_H
