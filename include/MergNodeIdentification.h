#ifndef MERGNODEIDENTIFICATION_H
#define MERGNODEIDENTIFICATION_H

#define NAME_SIZE 8

class MergNodeIdentification
{
    public:
        MergNodeIdentification();
        virtual ~MergNodeIdentification();
        void setCanID(byte val){canID=val;};
        void setNodeNumber(unsigned int val){nodeNumber=val;};
        void setNodeName(char[NAME_SIZE] nodeName);
        byte getCanID(){return canID;};
        unsigned int getNodeNumber(){return nodeNumber;};
        const char[NAME_SIZE] getNodeName();
        void setDeviceNumber(unsigned int val){deviceNumber=val;};
        unsigned int getDeviceNumber(){return deviceNumber;};
    protected:
    private:
        byte canID;
        unsigned int nodeNumber;
        char[NAME_SIZE] nodeName;
        unsigned int deviceNumber;

};

#endif // MERGNODEIDENTIFICATION_H
