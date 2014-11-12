#ifndef MERGNODEIDENTIFICATION_H
#define MERGNODEIDENTIFICATION_H

#include <Arduino.h>

#define NAME_SIZE 8

/**
* Class that saves the node parameters.
* Name, Manufactorer,Module, Can id, Node number, device number, Code Version, Supported events and variables.
* It is set from user information and from EEPROM: can id, node number, device number.
*/

class MergNodeIdentification
{
    public:
        MergNodeIdentification();
        virtual ~MergNodeIdentification();
        void setCanID(byte val){canID=val;};
        void setNodeNumber(unsigned int val){nodeNumber=val;};
        void setNodeName(char const *nodeName,int csize);
        byte getCanID(){return canID;};
        unsigned int getNodeNumber(){return nodeNumber;};
        const char* getNodeName();

        void setDeviceNumber(unsigned int val){deviceNumber=val;};
        unsigned int getDeviceNumber(){return deviceNumber;};

        void setManufacturerId(byte val){manufacturerId=val;};
        byte getManufacturerId(){return manufacturerId;};

        void setModuleId(byte val){moduleId=val;};
        byte getModuleId(){return moduleId;};

        void setFlags(byte val){flags=val;};
        byte getFlags(){return flags;};

        void setMinCodeVersion(byte val){minCodeVersion=val;};
        byte getMinCodeVersion(){return minCodeVersion;};

        void setMaxCodeVersion(byte val){maxCodeVersion=val;};
        byte getMaxCodeVersion(){return maxCodeVersion;};

        void setSuportedEvents(byte val){suportedEvents=val;};
        byte getSuportedEvents(){return suportedEvents;};

        void setSuportedEventsVariables(byte val){suportedEventsVariables=val;};
        byte getSuportedEventsVariables(){return suportedEventsVariables;};

        void setSuportedNodeVariables(byte val){suportedNodeVariables=val;};
        byte getSuportedNodeVariables(){return suportedNodeVariables;};

        byte getParameter(byte inx);
        byte getNumberOfParameters(){return 8;};
    protected:
    private:
        byte canID;
        unsigned int nodeNumber;
        char nodeName[NAME_SIZE];
        unsigned int deviceNumber;
        byte manufacturerId;
        byte moduleId;
        byte flags;
        byte minCodeVersion;
        byte maxCodeVersion;
        byte suportedEvents;
        byte suportedEventsVariables;
        byte suportedNodeVariables;


};

#endif // MERGNODEIDENTIFICATION_H
