#include "MergNodeIdentification.h"

MergNodeIdentification::MergNodeIdentification()
{
    int i=0;
    for (i=0;i<NAME_SIZE;i++){
        _nodeName[i]='\0';
    }
}

MergNodeIdentification::~MergNodeIdentification()
{
    //dtor
}
void MergNodeIdentification::setCanID(int canID)
{
    _canID=canID;
}
int MergNodeIdentification::getCanID()
{
    return _canID;
}
void MergNodeIdentification::setNodeNumber(int nodeNumber)
{
    _nodeNumber=nodeNumber;
}

void MergNodeIdentification::setNumberOfEvents(int numberOfEvents)
{
    _numberOfEvents=numberOfEvents;
}
void MergNodeIdentification::setNumberOfVarEvent(int numberOfVariables)
{
    _numberOfVariables=numberOfVariables;
}
void MergNodeIdentification::setNumberOfVariables(int numberOfVarEvents)
{
    _numberOfVarEvent=numberOfVarEvents;
}

int MergNodeIdentification::getNodeNumber()
{
    return _nodeNumber;
}
void MergNodeIdentification::setNodeName(char nodeName[NAME_SIZE] )
{
    int i=0;
    for (i=0;i<NAME_SIZE;i++){
        _nodeName[i]=nodeName[i];
    }
}
char* MergNodeIdentification::getNodeName()
{
    return _nodeName;
}
int MergNodeIdentification::getNumberOfEvents()
{
    return _numberOfEvents;
}
int MergNodeIdentification::getNumberOfVarEvent()
{
    return _numberOfVarEvent;
}
int MergNodeIdentification::getNumberOfVariables()
{
    return _numberOfVariables;
}




