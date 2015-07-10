#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#define _msgSize 15 //1 byte for the message size.1 byte for RTR, 4 bytes for header. 8 bytes to max message
#define CIRCULARBUFFER_MSGS 5 //messages
#define CIRCULARBUFFER_SIZE CIRCULARBUFFER_MSGS*_msgSize

#include <Arduino.h>

class CircularBuffer
{
    public:
        CircularBuffer();
        virtual ~CircularBuffer();
    public:
        bool put(byte *buffer);
        bool get(byte *buffer);
        int getPos(){return pos;};
    private:
        byte _buf[CIRCULARBUFFER_SIZE];
        int tail;
        int op;
        int pos;
        int entries;
        void incPos();
};

#endif // CIRCULARBUFFER_H
