#include "MergNodeIdentification.h"

MergNodeIdentification::MergNodeIdentification()
{
    for (int i=0;i<NAME_SIZE;i++){
        nodeName[i]='0';
    }
}

MergNodeIdentification::~MergNodeIdentification()
{
    //dtor
}
void MergNodeIdentification::setNodeName(char nn[NAME_SIZE])
{
    int i=0;
    for (i=0;i<NAME_SIZE;i++){
        nodeName[i]=nn[i];
    }
}
const char* MergNodeIdentification::getNodeName()
{
    return nodeName;
}

