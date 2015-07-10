#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

<<<<<<< HEAD
#define _msgSize 15 //1 byte for the message size.1 byte for RTR, 4 bytes for header. 8 bytes to max message
#define CIRCULARBUFFER_MSGS 5 //messages
#define CIRCULARBUFFER_SIZE CIRCULARBUFFER_MSGS*_msgSize
=======
#define _msgSize 15 /**<Size of the message stored in the buffer. 1 byte for the message size.1 byte for RTR, 4 bytes for header. 8 bytes to max message */
#define CIRCULARBUFFER_MSGS 10 /**<Number of messages of size _msgSize to be stored.*/
#define CIRCULARBUFFER_SIZE CIRCULARBUFFER_MSGS*_msgSize/**< The buffer size.*/
>>>>>>> 0877562a2d6a00eab20f04c26374e0761b5f3671

#include <Arduino.h>
  /**
   * CircularBuffer class is responsible for keeping the cbus messages in a circular buffer way.
   */


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
