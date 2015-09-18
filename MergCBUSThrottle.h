#ifndef MERGCBUSTHROTTLE_H

#define MERGCBUSTHROTTLE_H


class MergCBUSThrottle
{
    public:
        MergCBUSThrottle(MergCBUS *cbus,Message *message);
        virtual ~MergCBUSThrottle();
        bool getSession(uint16_t loco);
        bool releaseSession(uint8_t session);
        bool consist(uint16_t loco1,uint16_t loco2);
        void setKeepAliveInterval(uint16_t interval_milli);
        uint16_t getKeepAliveInterval();
        void setFOn(uint8_t f);
        void setFOff(uint8_t f);
        void setSpeed(uint8_t v);
        uint8_t getSpeed();
        void setDirection(bool d_forward);
        bool getDirection();
        bool setSpeedDirection(uint8_t v,bool d_forward);
        bool stealLoco();
        bool shareLoco();

    protected:
    private:
        MergCBUS *cbus;
        Message *message;
        uint16_t keepalive_interval;
        bool direction;
};

#endif // MERGCBUSTHROTTLE_H
