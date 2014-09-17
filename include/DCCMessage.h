#ifndef DCCMESSAGE_H
#define DCCMESSAGE_H

#include <Message.h>

#define DECODER_SIZE 2
#define DCCPACK_SIZE 6
#define CV_SIZE 2
#define FUNC_BYTE_SIZE 3
#define CS_REV_SIZE 2



class DCCMessage : public Message
{
    public:
        DCCMessage();
    protected:
    private:
        unsigned int session;
        byte decoderAddress[DECODER_SIZE];//is the MS and LS byte of the DCC address. For short addresses MS is set to 0.
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
        byte dccPackages[DCCPACK_SIZE];//see message RDCC3 RDCC4 RDCC5 RDCC6 in developer guide
        byte CV[CV_SIZE];//CVHigh and CVLow
        byte cvValue;
        byte functionByte[FUNC_BYTE_SIZE];//see PLOC message inde developer guide
        byte cs_revision[CS_REV_SIZE];//see STAT message
        byte cs_buildnumber;//see STAT message
        byte cs_status;//see STAT message
        byte cs_flags;//see STAT message

};

#endif // DCCMESSAGE_H
