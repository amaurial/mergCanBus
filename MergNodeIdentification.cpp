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

byte MergNodeIdentification::getParameter(byte idx){
    switch (idx){

    case 1:
        return manufacturerId;
        break;
    case 2:
        return minCodeVersion;
        break;
    case 3:
        return moduleId;
        break;
    case 4:
        return suportedEvents;
        break;
    case 5:
        return suportedEventsVariables;
        break;
    case 6:
        return suportedNodeVariables;
        break;
    case 7:
        return maxCodeVersion;
        break;
    case 8:
        return flags;
        break;
    }
    return 0;
}
