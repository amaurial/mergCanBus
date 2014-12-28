#ifndef MESSAGEBUFFER_H
#define MESSAGEBUFFER_H


#define MAX_BUFFER_MSG_SIZE 30  /* Max number of messages in the buffer when reading the CAN**/
#define MSG_SIZE_BUFFER  11 /* Size of the messages in the buffer, 8 bytes for can data + 3 extra bytes for priority and others**/

#include "Message.h"

class MessageBuffer
{
    public:
        MessageBuffer();
        virtual ~MessageBuffer();
        bool isEmpty();
        void getMessage(Message *message);
        void put(byte *canmsg,byte priority,byte msgtype);


    protected:
    private:
        //byte msgreadBuffer[MAX_BUFFER_MSG_SIZE*MSG_SIZE_BUFFER]; /**Buffer containing the read messages from the buffer*/
        byte msgindex;
        byte totalmessages;


};

#endif // MESSAGEBUFFER_H
