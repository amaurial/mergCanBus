#include "Message.h"

Message::Message()
{
    //ctor

    clear();
}
Message::Message(unsigned int canId,
                 unsigned int opc,
                 unsigned int nodeNumber,
                 unsigned int eventNumber,
                 byte data [DATA_SIZE] ,
                 unsigned int priority)
{
    //ctor
    Message();
    setCanId(canId);
    setOpc(opc);
    setNodeNumber(nodeNumber);
    setEventNumber(eventNumber);
    setPriority(priority);
    int i=0;
    for (int i=0;i<DATA_SIZE;i++){_data[i]=data[i];}
}

Message::~Message()
{
    //dtor
}

void Message::setData(byte val[DATA_SIZE] )
{
    int i=0;
    for (i=0;i<DATA_SIZE;i++){_data[i]=val[i];}

}

void Message::clear(){
    setCanId(0);
    setOpc(0);
    setType(message_type.RESERVED);
    setEventNumber(0);
    setNodeNumber(0);
    setData("00000000");
    setPriority(0);
    setNumBytes(0);
    unsetRTR();
    setSession(0);


}
