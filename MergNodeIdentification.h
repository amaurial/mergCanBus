#ifndef MERGNODEIDENTIFICATION_H
#define MERGNODEIDENTIFICATION_H

#include <Arduino.h>

#define NAME_SIZE 8


class MergNodeIdentification
{
    public:
        MergNodeIdentification();
        virtual ~MergNodeIdentification();
        void setCanID(byte val){canID=val;};
        void setNodeNumber(unsigned int val){nodeNumber=val;};
        void setNodeName(char* nodeName);
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
