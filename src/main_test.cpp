typedef char byte;
#include "Message.h"
#include "MessageReader.h"
#include "MergNodeIdentification.h"
//#include "opcodes.h"
#include "CANMessage.h"
#include <iostream>

using namespace std;

int main ()
{
    MergNodeIdentification node;
    node.setCanID(10);
    node.setNodeName("NODETEST");
    node.setNumberOfEvents(15);
    node.setNumberOfVarEvent(20);
    node.setNumberOfVariables(10);

    cout <<"NODE:" << node.getNodeName()<<"\t" << node.getCanID() << "\t" <<node.getNumberOfEvents()<<endl;

    Message message;
    CANMessage canMessage;
    MessageReader msgReader;

    msgReader.readMessage(&canMessage);

    cout << "CAN MESSAGE:" << canMessage.getData()<< "\t" << canMessage.get_header()<<endl;









}
