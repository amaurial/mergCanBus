#ifndef DCCMESSAGE_H
#define DCCMESSAGE_H

#include <Message.h>


class DCCMessage : public Message
{
    public:
        DCCMessage();
    protected:
    private:
        unsigned int session;
        byte decoderAddress[2];//is the MS and LS byte of the DCC address. For short addresses MS is set to 0.
        byte engineParameter;
        //0 or 1
        unsigned int speedMode;
        //0=128steps 1=14 steps 2=28 steps with interleave 3=28 steps
        unsigned int speedSteps;
        unsigned int serviceMode;
        unsigned int sndCtrlMode;
        unsigned int consist;
        byte int speedDirValue;
        unsigned int lightsOnOff;
        unsigned int relativeDirection;
        //active =0 , consisted =1, consist master=2, inactive=3
        unsigned int engineState;
        unsigned int funcNumber;
        unsigned int serviceStatus;
        byte serviceWriteMode;//see message WCVS on developer guide
        //1 is F0(FL) to F4
        //2 is F5 to F8
        //3 is F9 to F12
        //4 is F13 to F19
        //5 is F20 to F28
        unsigned int functionRange;
        byte int engineFlags;//see message GLOC in developer guide
        unsigned int repetionPackages;//see message RDCC3 in developer guide
        byte dccPackages[6];//see message RDCC3 RDCC4 RDCC5 RDCC6 in developer guide
        byte CV[2];//CVHigh and CVLow
        byte cvValue;
        byte functionByte[3];//see PLOC message inde developer guide
        byte cs_revision[2];
        byte cs_buildnumber;
        byte cs_status;
        byte





};

#endif // DCCMESSAGE_H
