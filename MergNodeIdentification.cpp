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
void MergNodeIdentification::setNodeName(char const *nn,int csize)
{
    int i=0;
    int n=NAME_SIZE;

    if (csize<NAME_SIZE){
        n=csize;
    }
    for (i=0;i<NAME_SIZE;i++){
        nodeName[i]=0;
    }

    for (i=0;i<n;i++){
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

void MergNodeIdentification::setProducerNode(bool val){
    if (val){
        bitSet(flags,1);
    }else {
        bitClear(flags,1)
    }
}
bool MergNodeIdentification::isProducerNode(){
    if (bitRead(flags,1)==1){
        return true;
    } else{
        return false;
    }

}

void MergNodeIdentification::setConsumerNode(bool val){
    if (val){
        bitSet(flags,0);
    }else {
        bitClear(flags,0)
    }
}
bool MergNodeIdentification::isConsumerNode(){
    if (bitRead(flags,0)==0){
        return true;
    } else{
        return false;
    }
}

void MergNodeIdentification::setSlimMode(){
    bitClear(flags,2);
}
void MergNodeIdentification::setFlimMode(){
    bitSet(flags,2);
}
bool MergNodeIdentification::isSlimMode(){
    if (bitRead(flags,2)==0){
        return true;
    } else{
        return false;
    }

}
bool MergNodeIdentification::isFlimMode(){
    if (bitRead(flags,2)==1){
        return true;
    } else{
        return false;
    }
}

void MergNodeIdentification::setSuportBootLoading(bool val){
    if (val){
        bitSet(flags,3);
    }else {
        bitClear(flags,3)
    }

}
bool MergNodeIdentification::suportBootLoading(){
    if (bitRead(flags,3)==1){
        return true;
    } else{
        return false;
    }
}

