#ifndef MERGMEMORYMANAGEMENT_H
#define MERGMEMORYMANAGEMENT_H

#include <Arduino.h>
#include <EEPROM.h>

#define EVENT_SIZE 4                    /** Event size.                                                        */
#define EVENT_VARS_SIZE 2               /** Size for events vars. Each var has 2 bytes. first is the index, second is the value        */
#define NNDD_SIZE 2                     /** Size of a node number or device number */
#define FLAG_SIZE 1
#define EMPTY_BYTE 0x00
#define INTERNAL_VARS 16

#define FAILED_INDEX 255                /** Value returned when vars or events vars not found                             */

#define MERG_MEMPOS               0                                               /** Position in memory. has the value 0xaa to mark it is from this mod       */
#define MERG_MEMORY_MODEL         MERG_MEMPOS + 1                                 /**< Mark the current memory model for this node*/
#define NODE_INTERNAL_VAR         MERG_MEMORY_MODEL + 1		  /** Provides space for the node to save internal states*/
#define CAN_ID_MEMPOS             NODE_INTERNAL_VAR + INTERNAL_VARS                           /**Position in memory.can id has 1 byte                                    */
#define NN_MEMPOS                 CAN_ID_MEMPOS + 1                               /**Position in memory.Node mumber has 2 bytes                               */
#define FLAGS_MEMPOS              NN_MEMPOS + NNDD_SIZE                           /**Position in memory.flags                          */
#define NUM_VARS_MEMPOS           FLAGS_MEMPOS + FLAG_SIZE                        /**Position in memory.amount of variables for this module- 1 byte          */
#define NUM_EVENTS_MEMPOS         NUM_VARS_MEMPOS + 1                             /**Position in memory.amount of learned events                             */
#define NUM_EVENTS_VARS_MEMPOS    NUM_EVENTS_MEMPOS + 1                           /**Position in memory.amount of learned events variables                   */
#define VARS_MEMPOS               NUM_EVENTS_VARS_MEMPOS + 1                      /**Position in memory.start of variables written for this module           */
//device numbers
//events with its variables
//#define EVENTS_MEMPOS VARS_MEMPOS+MAX_AVAIL_VARS                     /**Position in memory.                                                     */


/**
* Class that handles the EEPROM and maintains the data in a RAM memory.
*/

class MergMemoryManagement
{
    public:
        MergMemoryManagement();
        MergMemoryManagement(byte num_node_vars,byte num_events,byte num_events_vars,byte max_device_numbers);
        virtual ~MergMemoryManagement();
        /** \brief Check if there is some learned event.
        *\return True if the number of events is bigger than 0, else false.
        */
        bool hasEvents(){return (numEvents>0?true:false);};
        /** \brief Check if there event vars saved.
        *\return True if the number of events vars is bigger than 0, else false.
        */
        bool hasEventVars(uint8_t eventIdx);

        uint8_t setEvent(byte *event);//return the index
        uint8_t setEvent(byte *event,uint8_t eventIdx);
        byte* getEvent(uint8_t index);

        void setVar(uint8_t index,byte val);
        byte getVar(uint8_t index);

	    void setInternalVar(uint8_t index,byte val);
        byte getInternalVar(uint8_t index);

        uint8_t setEventVar(unsigned int eventIdx,uint8_t varIdx,uint8_t val);
        byte getEventVar(uint8_t eventIdx,uint8_t index);
        byte *getEventVars(uint8_t eventIdx,uint8_t *len);

        /** \brief Return the number of set node variables.*/
        byte getNumVars(){return MAX_AVAIL_VARS;};
        /** \brief Return the number of learned events.*/
        byte getNumEvents(){return numEvents;};
        /** \brief Return the number of learned events variables.*/
        byte getNumEventVars(){return MAX_VAR_PER_EVENT;};

        void eraseAllEvents();
        void read();

        uint8_t  eraseEvent(uint8_t eventIdx);
        //unsigned int  eraseEvent(byte event[EVENT_SIZE]);
        uint8_t  eraseEvent(unsigned int nn,unsigned int ev);

        void setCanId(byte canId);
        /**\brief Get the can id.
        * \return The can id
        */
        byte getCanId(){return can_ID;};

        unsigned int getNodeNumber();
        void setNodeNumber(unsigned int val);                       //2 bytes representation

        void setDeviceNumber(unsigned int val,byte port);           //2 bytes representation for dn
        unsigned int getDeviceNumber(byte port);                             //2 bytes representation
        /** \brief Return the maximum number of supported device numbers.*/
        byte getNumDeviceNumber(){return MAX_NUM_DEVICE_NUMBERS;};                                  //2 bytes representation

        uint8_t getEventIndex(byte ev1,byte ev2,byte ev3,byte ev4);
        uint8_t getEventIndex(unsigned int nn,unsigned int ev);
        bool hasEvent(byte ev1,byte ev2,byte ev3,byte ev4);

        byte getNodeFlag();
        void setNodeFlag(byte val);

        void setUpNewMemory();
        void dumpMemory();
        byte getMaxNumEvents(){return MAX_NUM_EVENTS;};
        byte getLastError(){return lasterror;};
    protected:

    private:
        byte *return_eventVars;                  //used to return all the variables of an event
        byte can_ID;
        byte nn[NNDD_SIZE];
        byte numEvents;
        byte numDeviceNumbers;
        byte MAX_AVAIL_VARS;               /** Number of node variables. Each has 1 byte                           */
        byte MAX_NUM_EVENTS;               /** Number of supported events. Each event has 4 bytes                  */
        byte MAX_VAR_PER_EVENT;            /** Total amount of events variables per event. */
        //int MAX_EVENTS_VAR_BUFFER;= MAX_NUM_EVENTS*MAX_VAR_PER_EVENT;       /** Total amount of events variables that can be stored. Each var has 2 bytes. first is the index, second is the value        */
        byte MAX_NUM_DEVICE_NUMBERS;       /** The total number of device number if it is a producer node. Each input from the producer can be a device number. So this is the same as the total number of inputs*/
        int EVENTS_MEMPOS;                     /**Position in memory.   */
        byte DN_MEMPOS;                     /** Position of device numbers in memory */

        //specify the memory model. it is like a module id identification. if the memory model is not equal to the eeprom it creates one. basic it reorganizes the
        //the events and events vars
        byte event[EVENT_SIZE];                                     //used to return this instead of a pointer to events array
        byte flags;
        void clear();
        unsigned int temp;                                          //used to avoid new memory allocation
        byte nodeVariablesSize;     //max supported number of node variables
        byte eventVarsPerEvent;     //number of supported vars per event
        byte amountSuportedEvents;  //number of supported events
        unsigned int resolveEvVarArrayPos(byte evidx,byte varidx);
        unsigned int incEventPos(unsigned int val);
        unsigned int resolveEventPos(byte evidx);
        void copyEvent(uint8_t fromIndex,uint8_t toIndex);
        byte lasterror; //records the last error
        void write();
};

#endif // MERGMEMORYMANAGEMENT_H
