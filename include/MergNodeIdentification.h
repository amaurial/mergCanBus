#ifndef MERGNODEIDENTIFICATION_H
#define MERGNODEIDENTIFICATION_H

#define NAME_SIZE 8

class MergNodeIdentification
{
    public:
        MergNodeIdentification();
        virtual ~MergNodeIdentification();
        void setCanID(int canID);
        void setNodeNumber(int nodeNumber);
        void setNodeName(char nodeName[NAME_SIZE] );
        void setNumberOfEvents(int numberOfEvents);
        void setNumberOfVarEvent(int numberOfVariables);
        void setNumberOfVariables(int numberOfVarEvents);
        int getCanID();
        int getNodeNumber();
        char* getNodeName();
        int getNumberOfEvents();
        int getNumberOfVarEvent();
        int getNumberOfVariables();

    protected:
    private:
        int _canID;                 //can id for this module
        int _nodeNumber;            //node number
        char _nodeName[NAME_SIZE] ;  //node name
        int _numberOfEvents;        //number of events supported
        int _numberOfVariables;     //number of node variables
        int _numberOfVarEvent;      //number of variables per event

};

#endif // MERGNODEIDENTIFICATION_H
