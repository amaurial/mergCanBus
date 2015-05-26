#include "CircularBuffer.h"

CircularBuffer::CircularBuffer()
{
    tail=0;
    op=0;
    pos=0;
    entries=0;
}

CircularBuffer::~CircularBuffer()
{
    //dtor
}

bool CircularBuffer::put(byte* buffer){
    if (op==0){
        op=1;//writing
    }
    else{
        return false;
    }
    if (tail>=CIRCULARBUFFER_SIZE){
        tail=0;
    }

    for (int i=0;i<_msgSize;i++){
        _buf[tail]=buffer[i];
        tail++;
        entries++;
    }
    if (entries>CIRCULARBUFFER_SIZE){
        incPos();
        entries=CIRCULARBUFFER_SIZE;
    }
    op=0;
    return true;
}

bool CircularBuffer::get(byte* buffer){
    if (op==0){
        op=1;//reading
    }
    else{
        return false;
    }

    if (entries<1){
        return false;
    }

    if (pos>=CIRCULARBUFFER_SIZE){
        pos=0;
    }
//    if ((pos+1)>=tail){
//        return false;//nothing to read.
//    }
    for (int i=0;i<_msgSize;i++){
        buffer[i]=_buf[pos];
        pos++;
        entries--;
    }
    op=0;
    return true;
}
void CircularBuffer::incPos(){
    pos=pos+_msgSize;
    if (pos>=CIRCULARBUFFER_SIZE){
        pos=0;
    }
}
