#ifndef MESSAGEREADER_H
#define MESSAGEREADER_H

//#include "mcp_can.h"
#include "CANMessage.h"

class MessageReader
{
    public:
        //MessageReader(MCP_CAN *CAN);
        MessageReader();
        virtual ~MessageReader();
        bool readMessage(CANMessage *canMessage); //message is passed by reference to avoid usage of more memory
    protected:
    private:
        CANMessage *_canMessage;
        //The CAN is responsable for the low level interface with CAN bus
        //MCP_CAN *CAN;                       //reference to CAN object
        void createTestData();
        char header[2];
        char data[8];
};

#endif // MESSAGEREADER_H
