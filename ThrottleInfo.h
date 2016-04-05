#ifndef THROTTLEINFO_H
#define THROTTLEINFO_H
#include <Arduino.h>
class ThrottleInfo
{
    public:
        ThrottleInfo();
        virtual ~ThrottleInfo();

        void setLoco(uint16_t loco){this->loco=loco;};
        uint16_t getLoco(){return loco;};

        void setSession(uint8_t session){this->session=session;};
        uint8_t getSession(){return session;};

        void setTime(uint32_t rtime){this->lastRefresh=rtime;};
        uint32_t getTime(){return lastRefresh;};

    protected:
    private:
        uint16_t loco;
        uint8_t session;
        uint32_t lastRefresh;
};

#endif // THROTTLEINFO_H
