#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#define _msgSize 8
class CircularBuffer
{
    public:
        CircularBuffer(int value);
        virtual ~CircularBuffer();
    public:
        void setSize(int value){bufSize=value*_msgSize;};
        void put(byte *buffer);
        void get(byte *buffer);
        int getPos();
    private:
        byte* _buf;
        int bufSize;
        int tail;
        int head;
        int pos;
};

#endif // CIRCULARBUFFER_H
