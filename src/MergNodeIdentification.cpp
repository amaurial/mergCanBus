#include "MergNodeIdentification.h"

MergNodeIdentification::MergNodeIdentification()
{
    for (i=0;i<NAME_SIZE;i++){
        this._nodeName[i]='0';
    }
}

MergNodeIdentification::~MergNodeIdentification()
{
    //dtor
}
void MergNodeIdentification::setNodeName(char[NAME_SIZE] nodeName)
{
    int i=0;
    for (i=0;i<NAME_SIZE;i++){
        this._nodeName[i]=nodeName[i];
    }
}
void const char[NAME_SIZE] MergNodeIdentification::getNodeName()
{
    return this._nodeName;
}

