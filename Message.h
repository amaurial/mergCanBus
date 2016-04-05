#ifndef BYTE_TYPE
#define BYTE_TYPE
//typedef char byte;
#endif // BYTE_TYPE

#ifndef MESSAGE_H
#define MESSAGE_H

//#define bitRead(val,pos){return 1;};
//#define bitSet(val,pos){return;};
//#define bitClear(val,pos){return;};

#include <Arduino.h>
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
* Each type represents the bit position in an 16 bits integer. It means it can hold 16 fields por each message.
* The standard is to set just the information wanted. It is used to check if there is a specific field in a message.
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
                 uint8_t opc,
                 unsigned int nodeNumber,
                 unsigned int eventNumber,
                 byte data [CANDATA_SIZE] ,
                 unsigned int priority);
        virtual ~Message();

        byte getByte(byte pos);

        /** \brief Get the 8 bytes CAN message buffer. The user can also get specific values using other methods.
         * \return Pointer to the CAN message.
         */

        byte *getDataBuffer(){return data;};
         void setDataBuffer(byte val[CANDATA_SIZE]);
        /** \brief Get the 8 bytes CAN header buffer. The user can also get specific values using other methods.
         * \return Pointer to the CAN header.
         */
        byte *getHeaderBuffer(){return header;};
        void setHeaderBuffer(byte val[HEADER_SIZE] );

        byte getCanId();
        /** \brief Set the can id in the message
         *  NOT USED. At the moment the messages are being assembled outside and loaded by \ref setDataBuffer
         * \param canId is the can ID
         */
        void setCanId(byte val) { canId = val; }

        byte getOpc();
        /** \brief Set the opc in the message
         * \param opc is the opc
         */
        void setOpc(byte val) {opc = val; }
        /** \brief Get the message type.
         * \return The message type \ref message_type
         */
        message_type getType() { return _type; }
        /** \brief Set the message type
         * \param val is the message type @message_type
         */
        void setType(message_type val) { _type = val; }

        unsigned int getEventNumber();
        /** \brief Set event number to be used on message
         * \param val is the event number
         */
        void setEventNumber(unsigned int val) { _eventNumber = val; }

        unsigned int getNodeNumber();
        /** \brief Set the node number to be used on message
         * \param val is the node number
         */
        void setNodeNumber(unsigned int val) { _nodeNumber = val; }

        unsigned int getDeviceNumber();
        /** \brief Set the device number to be used on message
         * \param val is the device number
         */
        void setDeviceNumber(unsigned int val) { _deviceNumber = val; }

        /** \brief Get the message priority in the Can header
         * \return The priority
         */
        byte getPriority() { return _priority;}
        /** \brief Set the message priority in the CAN header
         * \param val is the priority
         */
        void setPriority(byte val) { _priority = val; }

        byte getMessageSize();
        /** \brief Get the number of bytes parameter in the message
         * \return The number of bytes
         */
        uint8_t getNumBytes() { return _numBytes; }
        /** \brief Set the number of bytes parameter in the message
         * \param val is the number of bytes
         */
        void setNumBytes(uint8_t val) { _numBytes = val; }
        /** \brief Set the CAN RTR parameter. Set after reading the CAN header.
         */
        void setRTR() {_RTR=true;};
        /** \brief Unset the CAN RTR parameter
         */
        void unsetRTR(){_RTR=false;};
        /** \brief Get the CAN RTR parameter. Set after reading the CAN header.
         * \return True if the RTR is set else returns False.
         */
        bool getRTR(){return _RTR;};
        /** \brief Set the session value in the message
        *  NOT USED. At the moment the messages are being assembled outside and loaded by \ref setDataBuffer
         * \param val is the session value
         */
        //void setSession(byte val) {_session=val;}
        byte getSession();

        unsigned int getDecoder();
        /** \brief Set the decoder value
        *  NOT USED. At the moment the messages are being assembled outside and loaded by \ref setDataBuffer
         * \param val is the decoder value
         */
        //void setDecoder(unsigned int val) { _decoder = val; }

        unsigned int getCV();
        /** \brief Set the DCC CV value in the message
        *  NOT USED. At the moment the messages are being assembled outside and loaded by \ref setDataBuffer
         * \param val is the DCC CV value
         */
        //void setCV(unsigned int val) { _cv = val; }

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
        /** \brief Force the code to print debug messages to the serial interface if value is True.
         */
        byte getCanMessageSize();
        /** \brief Set the message size
         * \param The message size
         */
        void setCanMessageSize(byte val) {canMsgSize=val;};

        bool isAccOn();
        bool isAccOff();
        bool isLongEvent();
        bool isShortEvent();
        byte accExtraData();
        byte getAccExtraData(byte idx);//idx starts at 1
        void createOnEvent(unsigned int nodeNumber,bool longEvent,unsigned int eventNumber,byte numDataBytes,byte* data);
        void createOffEvent(unsigned int nodeNumber,bool longEvent,unsigned int eventNumber,byte numDataBytes,byte* data);


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
        byte data[CANDATA_SIZE] ;               /** Internal buffer for can message.*/
        byte header[HEADER_SIZE] ;              /** Internal buffer for can message.*/
        byte _priority;                         /** Message priority*/
        unsigned int _numBytes;                 /** Message size*/
        bool _RTR;                              /** If the message is a RTR message*/
        bool hasThisData(byte opc, message_config_pos pos);/** Check if a field is present in a message.*/
        message_type findType(byte opc);
        bool debug;

};

#endif // MESSAGE_H
