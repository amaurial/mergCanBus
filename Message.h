#ifndef BYTE_TYPE
#define BYTE_TYPE
//typedef char byte;
#endif // BYTE_TYPE

#ifndef MESSAGE_H
#define MESSAGE_H

//#define bitRead(val,pos){return 1;};
//#define bitSet(val,pos){return;};
//#define bitClear(val,pos){return;};


#include "CANMessage.h"
#include "opcodes.h"


#define CANDATA_SIZE 8      /** Message size*/
#define HEADER_SIZE 4
#define MSGSIZE 256         /** Amount of possible can messages.*/

/**
* Type of MergCBUS messages. See the developer documentation for more details.
*/
enum message_type {
                    RESERVED=0, /**< Reserved messages*/
                    DCC=1,      /**< DCC messages */
                    CONFIG=2,   /**< Config messages*/
                    ACCESSORY=3,/**< Accessory messages*/
                    GENERAL=4   /**< General messages*/
                    };

/**
* Position on the filter for messages for the most common fields.
*/
enum message_config_pos {
                        NODE_NUMBER=0,      /**< Node number*/
                        DEVICE_NUMBER=1,    /**< Device Number*/
                        EVENT_NUMBER=2,     /**< Event number */
                        SESSION=3,          /**< Session */
                        DECODER=4,          /**< Decoder */
                        CV=5                /**< Decoder CV*/
                        };

/**
* The Message class holds the detailed information about a can message.
* If works as a wrapper class over the can bus message.
* It extracts the information from the message based on the semantics of the message OPC.
* Still in fase of modification and cleanning.
*/

class Message
{
    public:
        Message();
        Message(unsigned int canId,
                 unsigned int opc,
                 unsigned int nodeNumber,
                 unsigned int eventNumber,
                 byte data [CANDATA_SIZE] ,
                 unsigned int priority);
        virtual ~Message();
        unsigned int setCanMessage(CANMessage *canMessage);
        CANMessage *getCanMessage(){return _canMessage;}
        byte getByte(byte pos);


        byte *getDataBuffer(){return data;};
         void setDataBuffer(byte val[CANDATA_SIZE]);

        byte *getHeaderBuffer(){return header;};
        void setHeaderBuffer(byte val[HEADER_SIZE] );

        byte getCanId();
        void setCanId(byte val) { canId = val; }

        byte getOpc();
        void setOpc(byte val) {opc = val; }

        message_type getType() { return _type; }
        void setType(message_type val) { _type = val; }

        unsigned int getEventNumber();
        void setEventNumber(unsigned int val) { _eventNumber = val; }

        unsigned int getNodeNumber();
        void setNodeNumber(unsigned int val) { _nodeNumber = val; }

        unsigned int getDeviceNumber();
        void setDeviceNumber(unsigned int val) { _deviceNumber = val; }



        byte getPriority() { return _priority;}
        void setPriority(byte val) { _priority = val; }

        byte getMessageSize();
        unsigned int getNumBytes() { return _numBytes; }
        void setNumBytes(unsigned int val) { _numBytes = val; }

        void setRTR() {_RTR=true;};
        void unsetRTR(){_RTR=false;};
        bool getRTR(){return _RTR;};

        void setSession(byte val) {_session=val;}
        byte getSession();

        unsigned int getDecoder();
        void setDecoder(unsigned int val) { _decoder = val; }

        unsigned int getCV();
        void setCV(unsigned int val) { _cv = val; }

        unsigned int getCVMode();
        unsigned int getCVValue();
        byte getConsist();
        byte getSpeedDir();
        byte getEngineFlag();
        byte getAvailableEventsLeft();
        byte getStoredEvents();
        byte getFunctionNumber();
        byte functionValue();
        byte getStatus();
        byte getParaIndex();
        byte getParameter();
        byte getNodeVariableIndex();
        byte getNodeVariable();
        byte getEventIndex();
        byte getEventVarIndex();
        byte getEventVar();
        void clear();
        void setDebug(bool val){debug=val;};
        byte getCanMessageSize();
        void setCanMessageSize(byte val) {canMsgSize=val;};

    protected:
    private:
        byte canId;                             /** The cand id*/
        byte opc;                               /** The opc*/
        byte msgSize;                           /** The message size. It is the total number of bytes of message in the merg can message*/
        byte canMsgSize;                        /** The amount of byte read from the can frame*/
        message_type _type;                     /** Holds the message type @see message_type*/
        unsigned int _eventNumber;              /** Event number 2 bytes*/
        unsigned int _nodeNumber;               /** Node Number 2 bytes*/
        unsigned int _deviceNumber;             /** Device Number 2 bytes*/
        unsigned int _decoder;                  /** Decoder address*/
        unsigned int _cv;                       /** Decoder variable*/
        byte _session;                          /** Loc session*/
        byte data[CANDATA_SIZE] ;              /** Internal buffer for can message.*/
        byte header[HEADER_SIZE] ;              /** Internal buffer for can message.*/
        byte _priority;                         /** Message priority*/
        unsigned int _numBytes;                 /** Message size*/
        bool _RTR;                              /** If the message is a RTR message*/
        CANMessage* _canMessage;                /** The transport can message*/
        message_type messages[MSGSIZE];         /** Holds the message type for each opc. Make an index of message types. Opc is the array index*/

        unsigned int message_params[MSGSIZE];   /** Use each bit to hold a true false information about the message. Used to make the search faster.*/
        bool hasThisData(byte opc, message_config_pos pos);/** Check if a field is present in a message.*/
        void loadMessageConfig();               /** Load standard message config.*/
        void loadMessageType();                 /** Load standard message types config.*/

        bool debug;

};

#endif // MESSAGE_H
