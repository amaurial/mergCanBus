#include "MergCBUSThrottle.h"

MergCBUSThrottle::MergCBUSThrottle(MergCBUS *cbus,Message *message)
{
    //ctor
}

MergCBUSThrottle::~MergCBUSThrottle()
{
    //dtor
}

bool MergCBUSThrottle::getSession(uint16_t loco){
}

bool MergCBUSThrottle::releaseSession(uint8_t session){
}

bool MergCBUSThrottle::consist(uint16_t loco1,uint16_t loco2){
}

void MergCBUSThrottle::setKeepAliveInterval(uint16_t interval_milli){
}

uint16_t MergCBUSThrottle::getKeepAliveInterval(){
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
