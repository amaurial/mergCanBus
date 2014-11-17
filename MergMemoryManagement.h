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

#define MAX_AVAIL_VARS 25           /** Number of node variables. Each has 1 byte                           */
#define MAX_NUM_EVENTS 30           /** Number of supported events. Each event has 4 bytes                  */
#define MAX_NUM_EVENTS_VAR 75       /** Total amount of events variables. Each var has 2 bytes. first is the index, second is the value        */
#define EVENT_SIZE 4                /** Event size.                                                        */
#define EVENT_VARS_SIZE 2           /** Size for events vars. Each var has 2 bytes. first is the index, second is the value        */

#define FAILED_INDEX 255            /** Value returned when vars or events vars not found                             */

#define MERG_MEMPOS 0                                                  /** Position in memory. has the value 0xaa to mark it is from this mod       */
#define CAN_ID_MEMPOS MERG_MEMPOS+1                                    /**Position in memory.can id has 1 byte                                    */
#define NN_MEMPOS CAN_ID_MEMPOS+1                                      /**Position in memory.Node mumber has 2 bytes                               */
#define NN_MODE_MEMPOS NN_MEMPOS+1                                     /**Position in memory.node mode SLIM=0 or FLIM=1                          */
#define DN_MEMPOS NN_MODE_MEMPOS+1                                     /**Position in memory.Device number has 2 bytes                            */
#define NUM_VARS_MEMPOS DN_MEMPOS+1                                    /**Position in memory.amount of variables for this module- 1 byte          */
#define NUM_EVENTS_MEMPOS NUM_VARS_MEMPOS+1                            /**Position in memory.amount of learned events                             */
#define NUM_EVENTS_VARS_MEMPOS NUM_EVENTS_MEMPOS+1                     /**Position in memory.amount of learned events variables                   */
#define VARS_MEMPOS NUM_EVENTS_VARS_MEMPOS+1                           /**Position in memory.start of variables written for this module           */
#define EVENTS_MEMPOS VARS_MEMPOS+1+MAX_AVAIL_VARS                     /**Position in memory.                                                     */
#define EVENTS_VARS_MEMPOS EVENTS_MEMPOS+1+MAX_NUM_EVENTS*EVENT_SIZE   /**Position in memory.                                                    */

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
        bool hasEventVars(){return (numEventVars>0?true:false);};

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

        byte getCanId(){return can_ID;};
        unsigned int getNodeNumber();
        unsigned int getDeviceNumber();

        unsigned int setEvent(byte *event);//return the index
        unsigned int setEvent(byte *event,unsigned int eventIdx);

        unsigned int setEventVar(unsigned int eventIdx,unsigned int varIdx,byte val);

        unsigned int getEventIndex(byte ev1,byte ev2,byte ev3,byte ev4);
        unsigned int getEventIndex(unsigned int nn,unsigned int ev);

        byte getNodeFlag();
        void setNodeFlag(byte mode);

        void setUpNewMemory();
        void dumpMemory();

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
        byte nodeMode;
        void clear();
        void writeEvents();
        void newEventVar(unsigned int eventIdx,unsigned int varIdx,byte val);
        unsigned int temp; //used to avoid new memory allocation


};

#endif // MERGMEMORYMANAGEMENT_H
