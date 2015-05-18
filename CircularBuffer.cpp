#include "CircularBuffer.h"

CircularBuffer::CircularBuffer(int value)
{
    if (value>0){
        setSize(value);
        buf_=malloc(bufSize);
    }
}

CircularBuffer::~CircularBuffer()
{
    //dtor
}
