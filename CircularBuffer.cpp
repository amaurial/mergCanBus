#include "CircularBuffer.h"
/** \brief
 * Constructor
 * Basically set the initial values of the internal variables.
 */

CircularBuffer::CircularBuffer()
{
    tail=0;
    op=0;
    pos=0;
    entries=0;
}
/** \brief
 * Destructor
 */
/**<  */
CircularBuffer::~CircularBuffer()
{
    //dtor
}
/** \brief
 *  Add a message of the _msgSize size to the buffer.
 * \param buffer
 * \return False in case the buffer is being writing else return True.
 *
 */

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
/** \brief Put the next message to the buffer.
 *
 * \param buffer. Buffer that will receive the message.
 * \return Return False if the buffer is being reading or if the buffer is empty. Else returns True.
 *
 */

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
/** \brief Advance the head position.
 *
 */

void CircularBuffer::incPos(){
    pos=pos+_msgSize;
    if (pos>=CIRCULARBUFFER_SIZE){
        pos=0;
    }
}
