#ifndef ACCESSORYMESSAGE_H
#define ACCESSORYMESSAGE_H

#include <Message.h>

#define ADDBYTE_SIZE 5 //Additional data byte
#define DEVNUM_SIZE 2

class AccessoryMessage : public Message
{
    public:
        AccessoryMessage();
        virtual ~AccessoryMessage();
    protected:
    private:
        byte device_number[DEVNUM_SIZE];
        byte additional_byte[ADDBYTE];
        //for fast clock implementation
        byte minutes;
        byte hours;
        byte wdmon;
        byte div;
        byte mday;
        byte temp;
        //

};

#endif // ACCESSORYMESSAGE_H
