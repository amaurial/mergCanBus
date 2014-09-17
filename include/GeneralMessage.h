#ifndef GENERALMESSAGE_H
#define GENERALMESSAGE_H

#include <Message.h>

#define EXTRA_OPC_SIZE 5

class GeneralMessage : public Message
{
    public:
        GeneralMessage();
        virtual ~GeneralMessage();
    protected:
    private:
    byte extra_opc[EXTRA_OPC_SIZE];
    byte manufacturer_id;//see PNN message
    byte module_id;//see PNN message
    byte flags;//see PNN message
};

#endif // GENERALMESSAGE_H
