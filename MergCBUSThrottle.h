#ifndef MERGCBUSTHROTTLE_H

#define MERGCBUSTHROTTLE_H

#include "MergCBUS.h"
#include "Message.h"
#include "ThrottleInfo.h"

#define NUM_SESSIONS 4

class MergCBUSThrottle
{
    public:
        MergCBUSThrottle(MergCBUS *cbus);
        virtual ~MergCBUSThrottle();

        bool getSession(uint16_t loco);
        bool setSession(uint8_t session,uint16_t loco);
        bool releaseSession(uint8_t session);

        bool consist(uint16_t loco1,uint16_t loco2);
        void setKeepAliveInterval(uint16_t interval_milli);
        uint16_t getKeepAliveInterval();
        void setFOn(uint8_t session, uint8_t f);
        void setFOff(uint8_t session, uint8_t f);
        uint8_t getSpeed();
        void setDirection(bool d_forward);
        bool getDirection();
        bool setSpeedDirection(uint8_t session, uint8_t v,bool d_forward);
        void setSpeedMode(uint8_t session);
        bool stealLoco();
        bool shareLoco();
        void run();

    protected:
    private:
        MergCBUS *cbus;
        Message *message;
        uint16_t keepalive_interval;
        bool direction;
        ThrottleInfo tinfo[NUM_SESSIONS];
        void sendKeepAlive();

};

#endif // MERGCBUSTHROTTLE_H
