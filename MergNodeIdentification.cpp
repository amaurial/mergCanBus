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
 * \param idx Index of the parameter: 0 to 20
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
	case 9:  
        return cpuType; 
        break;	
	case 10:  
		return transportType;
		break;
	case 11: 
		return codeLoad;
		break;
	case 12: 
		return codeLoad;
		break;
	case 13: 
		return codeLoad;
		break;	
	case 14: 
		return codeLoad;
		break;	
	case 15: 
		return ManufacturerCpuCodeA;
		break;	
	case 16: 
		return ManufacturerCpuCodeB;
		break;	
	case 17: 
		return ManufacturerCpuCodeC;
		break;	
	case 18: 
		return ManufacturerCpuCodeD;
		break;	
	case 19:  
		return cpuManufacturer;
		break;	
	case 20: 
		return betaRelease;
		break;
	}
		
    return 0;
}
/** \brief Set this module as a producer.
 *
 * \param val True for producer. False not producer.
 */



/*Bit 0: Consumer
Bit 1: Producer
Bit 2: FLiM Mode
Bit 3: The module supports bootloading
Bit 4: The module can consume its own produced events
Bit 5: Module is in learn mode

*/

 /*--------------------------------------------------------------*/

 
 /* Set Flags for Consumer Mode */

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

 /*--------------------------------------------------------------*/
 
 
 /** \brief Indicates if the node is a producer.
 * \return True for producer. False not producer.
 *
 */
 
  /* Set Flags for Producer Mode */


void MergNodeIdentification::setProducerNode(bool val){
    if (val){
        bitSet(flags,1);
    }else {
        bitClear(flags,1);
    }
}


bool MergNodeIdentification::isProducerNode(){
    if (bitRead(flags,1)==1){
        return true;
    } else{
        return false;
    }

}

 /*--------------------------------------------------------------*/
 
  /** \brief Indicates if the node can consume own events
 * \return True for can consume False cannot consume.
 *
 */
  // added by phil

void MergNodeIdentification::setConsumeEvents(bool val){
    if (val){
        bitSet(flags,4);
    }else {
        bitClear(flags,4);
    }
}

bool MergNodeIdentification::isConsumeEvents(){
    if (bitRead(flags,4)==4){
        return true;
    } else{
        return false;
    }

}

 /*--------------------------------------------------------------*/



/** \brief Indicates if the node is in learn mode.
 * \return True if node is in learn mode.
 * added by phil
 */

void MergNodeIdentification::setLearnMode(bool val){
    if (val){
        bitSet(flags,5);
    }else {
        bitClear(flags,5);
    }
}


bool MergNodeIdentification::isLearnMode(){
    if (bitRead(flags,5)==5){
        return true;
    } else{
        return false;
    }

}

 /*--------------------------------------------------------------*/




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
bool MergNodeIdentification::isBootLoading(){
   
    if (bitRead(flags,3)==1){
        return true;
    } else{
        return false;
    }
    
    return false;
	
}

