#ifndef MERGNODEIDENTIFICATION_H
#define MERGNODEIDENTIFICATION_H

#define NAME_SIZE 8

class MergNodeIdentification
{
    public:
        MergNodeIdentification();
        virtual ~MergNodeIdentification();
        public void setCanID(int canID);
        public void setNodeNumber(int nodeNumber);
        public void setNodeName(char[NAME_SIZE] nodeName);
        public int getCanID();
        public int getNodeNumber();
        public const char[NAME_SIZE] getNodeName();

    protected:
    private:
        int _canID;
        int _nodeNumber;
        char[NAME_SIZE] _nodeName;
};

#endif // MERGNODEIDENTIFICATION_H
