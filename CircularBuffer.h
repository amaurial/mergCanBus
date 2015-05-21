#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#define _msgSize 8
#define CIRCULARBUFFER_MSGS 50 //messages
#define CIRCULARBUFFER_SIZE CIRCULARBUFFER_MSGS*_msgSize
class CircularBuffer
{
    public:
        CircularBuffer(int value);
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
};

#endif // CIRCULARBUFFER_H
