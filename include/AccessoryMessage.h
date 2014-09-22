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
        byte getMinutes(){return minutes;};
        void setMinutes(byte val){minute=val;};
        byte getHours(){return hours};
        void setHours(byte val){hours=val;}
        byte getWdmon(){return wdmon;};
        void setWdmon(){wdmon=val;};
        byte getDiv(){return div;};
        void setDiv(){div=val;};
        byte getMday(){return mday;};
        void setMday(){mday=val;};
        byte getTemperature(){return temperature;};
        void setTemperature(){temperature=val;};
        int getTotalAddBytes(){return totalAddBytes};
        void setTotalAddBytes(unsigned int val){totalAddBytes=val;};
        void setAdditionalByte(unsigned int byte,unsigned int pos);
        void setDeviceNumber(unsigned int byte,unsigned int pos);
        byte getDeviceNumberByte(unsigned int pos);
        byte getAddtionalByte(unsigned int pos);



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
        byte temperature;
        unsigned int totalAddBytes;
        //

};

#endif // ACCESSORYMESSAGE_H
