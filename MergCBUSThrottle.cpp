#include "MergCBUSThrottle.h"

MergCBUSThrottle::MergCBUSThrottle(MergCBUS *cbus)
{
    //ctor
    this->cbus=cbus;
    //this->message=message;
    for (uint8_t i=0;i<NUM_SESSIONS;i++){
        tinfo[i].setSession(255);
        tinfo[i].setLoco(0);
    }
    setKeepAliveInterval(2000);
}

MergCBUSThrottle::~MergCBUSThrottle()
{
    //dtor
}

void MergCBUSThrottle::run(){
    sendKeepAlive();
}

void MergCBUSThrottle::sendKeepAlive(){

    uint32_t t;
    for (uint8_t i=0;i<NUM_SESSIONS;i++){
        t=millis();
        if (tinfo[i].getLoco() != 0){
            if ((t-tinfo[i].getTime())>keepalive_interval){
                tinfo[i].setTime(t);
                cbus->sendKeepAliveSession(tinfo[i].getSession());
            }
        }
    }
}

bool MergCBUSThrottle::getSession(uint16_t loco){
    if (cbus->sendGetSession(loco) == 0){
        return true;
    }
    return false;
}

bool MergCBUSThrottle::releaseSession(uint8_t session){
    if (cbus->sendReleaseSession(session) == 0){
        return true;
    }
    return false;
}

bool MergCBUSThrottle::setSession(uint8_t session,uint16_t loco){

    for (uint8_t i=0;i<NUM_SESSIONS;i++){
        if (tinfo[i].getLoco() == 0 ){
            tinfo[i].setTime(millis());
            tinfo[i].setSession(session);
            tinfo[i].setLoco(loco);
            return true;
        }
    }
    return false;
}

bool MergCBUSThrottle::consist(uint16_t loco1,uint16_t loco2){
}

void MergCBUSThrottle::setKeepAliveInterval(uint16_t interval_milli){
    this->keepalive_interval=interval_milli;
}

uint16_t MergCBUSThrottle::getKeepAliveInterval(){
    return this->keepalive_interval;
}

void MergCBUSThrottle::setFOn(uint8_t f){
}

void MergCBUSThrottle::setFOff(uint8_t f){
}

void MergCBUSThrottle::setSpeed(uint8_t v){
}

uint8_t MergCBUSThrottle::getSpeed(){
}

void MergCBUSThrottle::setDirection(bool d_forward){
}

bool MergCBUSThrottle::getDirection(){
}

bool MergCBUSThrottle::setSpeedDirection(uint8_t v,bool d_forward){
}

bool MergCBUSThrottle::stealLoco(){
}

bool MergCBUSThrottle::shareLoco(){
}
