#include "MergNodeIdentification.h"

/** \brief Constructor
 *
 * Set the internal variables.
 */


MergNodeIdentification::MergNodeIdentification()
{
    for (int i=0;i<NAME_SIZE;i++){
        nodeName[i]='0';
    }
}

/** \brief Destructor
 *
 */
MergNodeIdentification::~MergNodeIdentification()
{
    //dtor
}

/** \brief Set the node name
 *
 * \param nn Pointer to the node name.
 * \param csize Size of the node name limited by NAME_SIZE.
 *
 */

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
/** \brief Retrieve the node name.
 *
 * \return Pointer to the node name.
 *
 */

const char* MergNodeIdentification::getNodeName()
{
    return nodeName;
}

/** \brief Get one of the defined parameters
 *
 * \param idx Index of the parameter: 0 to 8
 * \return Return either of the parameters:
 * + Manufacturer Id
 * + Max Code Version
 * + Min Code Version
 * + Module Id
 * + Number of Supported Events
 * + Number of Supported Event Variables
 * + Number of Supported Node Variables
 * + Node Flags
 * + 0 by default
 */

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
/** \brief Set this module as a producer.
 *
 * \param val True for producer. False not producer.
 */

void MergNodeIdentification::setProducerNode(bool val){
    if (val){
        bitSet(flags,1);
    }else {
        bitClear(flags,1);
    }
}
/** \brief Indicates if the node is a producer.
 * \return True for producer. False not producer.
 *
 */

bool MergNodeIdentification::isProducerNode(){
    if (bitRead(flags,1)==1){
        return true;
    } else{
        return false;
    }

}
/** \brief Set this module as a consumer.
 *
 * \param val True for consumer. False not consumer.
 */
void MergNodeIdentification::setConsumerNode(bool val){
    if (val){
        bitSet(flags,0);
    }else {
        bitClear(flags,0);
    }
}
/** \brief Indicates if the node is a consumer.
 * \return True for consumer. False not consumer.
 *
 */
bool MergNodeIdentification::isConsumerNode(){
    if (bitRead(flags,0)==0){
        return true;
    } else{
        return false;
    }
}
/** \brief Set this module to SlimMode.
 */
void MergNodeIdentification::setSlimMode(){
    bitClear(flags,2);
}
/** \brief Set this module to FlimMode.
 */
void MergNodeIdentification::setFlimMode(){
    bitSet(flags,2);
}
/** \brief Indicate if the mode is in SlimMode.
 */
bool MergNodeIdentification::isSlimMode(){
    if (bitRead(flags,2)==0){
        return true;
    } else{
        return false;
    }
}
/** \brief Indicate if the mode is in FlimMode.
 */
bool MergNodeIdentification::isFlimMode(){
    if (bitRead(flags,2)==1){
        return true;
    } else{
        return false;
    }
}
/** \brief Indicate if the mode supports boot loading.
 * This current version by default does not support boot loading.
 */
void MergNodeIdentification::setSuportBootLoading(bool val){
    if (val){
        bitSet(flags,3);
    }else {
        bitClear(flags,3);
    }

}
/** \brief Indicate if the mode supports boot loading.
 * This current version by default does not support boot loading.
 */
bool MergNodeIdentification::suportBootLoading(){
   /*
    if (bitRead(flags,3)==1){
        return true;
    } else{
        return false;
    }
    */
    return false;
}

