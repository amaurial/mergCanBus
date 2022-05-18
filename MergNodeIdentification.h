#ifndef MERGNODEIDENTIFICATION_H
#define MERGNODEIDENTIFICATION_H

#include <Arduino.h>

#define NAME_SIZE 7 /**< Maximum node name size.*/

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
        /** \brief  Set the can id for this node.
         * \param val The can id.
         */
        void setCanID(byte val){canID=val;};
        /** \brief  Get the can id for this node.
         * \return The can id.
         */
        byte getCanID(){return canID;};
        /** \brief  Set the node number for this node.
         * \param val The node number.
         */
        void setNodeNumber(unsigned int val){nodeNumber=val;};
        /** \brief  Get the node number for this node.
         * \return The node number.
         */
        unsigned int getNodeNumber(){return nodeNumber;};

        void setNodeName(char const *nodeName,int csize);
        const char* getNodeName();

        //void setDeviceNumber(unsigned int val){deviceNumber=val;};
        //unsigned int getDeviceNumber(){return deviceNumber;};
		
		
        /** \brief  Set the manufacturer ID.
         *
         * \param val The manufacuter id.
         *
         */
        void setManufacturerId(byte val){manufacturerId=val;};
        /** \brief  GSet the manufacturer ID.
         *
         * \return The manufacuter id.
         *
         */
        byte getManufacturerId(){return manufacturerId;};
        /** \brief  Set the module ID.
         *
         * \param val The module id.
         *
         */
        void setModuleId(byte val){moduleId=val;};
        /** \brief  Get the module ID.
         *
         * \return The module id.
         *
         */
        byte getModuleId(){return moduleId;};
        /** \brief  Set the module flags.
         *
         * \param val The module flags.
         *
         */
        void setFlags(byte val){flags=val;};
        /** \brief  Get the module flags.
         *
         * \param val The module flags.
         *
         */
        byte getFlags(){return flags;};
		
		
        /** \brief  Set the min Code Version.
         *
         * \param val The min code version.
         *
         */
        void setMinCodeVersion(byte val){minCodeVersion=val;};
        /** \brief  Get the min Code Version.
         *
         * \return The min code version.
         *
         */
        byte getMinCodeVersion(){return minCodeVersion;};
        /** \brief  Set the max Code Version.
         *
         * \param val The max code version.
         *
         */
        void setMaxCodeVersion(byte val){maxCodeVersion=val;};
        /** \brief  Get the max Code Version.
         *
         * \return The max code version.
         *
         */
        byte getMaxCodeVersion(){return maxCodeVersion;};
        /** \brief  Set the number of supported events.
         *
         * \param val The number of events.
         *
         */
        void setSuportedEvents(byte val){suportedEvents=val;};
        /** \brief  Get the number of supported events.
         *
         * \return The number of events.
         *
         */
        byte getSuportedEvents(){return suportedEvents;};
        /** \brief  Set the number of supported events variables.
         * \param val The number of events variables.
         */
        void setSuportedEventsVariables(byte val){suportedEventsVariables=val;};
        /** \brief  Get the number of supported events variables.
         * \return The number of events variables.
         */
        byte getSuportedEventsVariables(){return suportedEventsVariables;};
        /** \brief  Set the number of supported node variables.
         * \param val The number of events variables.
         */
        void setSuportedNodeVariables(byte val){suportedNodeVariables=val;};
        /** \brief  Get the number of supported node variables.
         * \return The number of events variables.
         */
        byte getSuportedNodeVariables(){return suportedNodeVariables;};
		
		
		void setTransportType(byte val){transportType=val;}; // added by phil
		byte getTransportType(){return transportType;};
		
		void setCpuType(byte val){cpuType=val;}; // added by phil
		byte getCpuType(){return cpuType;};
		
		
		void setCpuManufacturer(byte val){cpuManufacturer=val;};		// added by phil
		byte getCpuManufacturer(){return cpuManufacturer;};
		

        void setBetaRelease(byte val){betaRelease=val;}; // added by phil
		byte getBetaRelease(){return betaRelease;};
		
		
		void setCodeLoad(byte val =0){codeLoad =val;}; // added by phil params 11 to 14 value 0x00 does not support boot loading
		byte getCodeLoad(){return codeLoad;};
		
		void setManufacturerCpuCodeA(byte val){ManufacturerCpuCodeA=val;};		// added by phil actual proccessor param 15
		byte getManufacturerCpuCodeA(){return ManufacturerCpuCodeA;};
		
		void setManufacturerCpuCodeB(byte val){ManufacturerCpuCodeB=val;};		// added by phil actual proccessor param 16
		byte getManufacturerCpuCodeB(){return ManufacturerCpuCodeB;};
		
		void setManufacturerCpuCodeC(byte val){ManufacturerCpuCodeC=val;};		// added by phil actual proccessor param 17
		byte getManufacturerCpuCodeC(){return ManufacturerCpuCodeC;};
		
		void setManufacturerCpuCodeD(byte val){ManufacturerCpuCodeD=val;};		// added by phil actual proccessor param 18
		byte getManufacturerCpuCodeD(){return ManufacturerCpuCodeD;};
		

	   byte getParameter(byte inx);
        /** \brief  Get the number of node parameters.
         * \return Currently 21 changed by phil
         */
        byte getNumberOfParameters(){return 20;}; //  20 added by phil

        void setProducerNode(bool val);
        bool isProducerNode();

        void setConsumerNode(bool val);
        bool isConsumerNode();
		
		void setConsumeEvents(bool val); // added by phil
        bool isConsumeEvents();
		
		void setLearnMode(bool val); // added by phil
        bool isLearnMode();

        void setSlimMode();
        void setFlimMode();
        bool isSlimMode();
        bool isFlimMode();

        void setSuportBootLoading(bool val);
        bool isBootLoading();

    protected:
    private:
        byte canID;
        unsigned int nodeNumber;
        char nodeName[NAME_SIZE];
        //unsigned int deviceNumber;
        byte manufacturerId;
        byte moduleId;
        byte flags;
        byte minCodeVersion;
        byte maxCodeVersion;
        byte suportedEvents;
        byte suportedEventsVariables;
        byte suportedNodeVariables;
		byte transportType;
		byte betaRelease;
		byte cpuManufacturer;
		byte cpuType;
		byte codeLoad;
		byte ManufacturerCpuCodeA;
		byte ManufacturerCpuCodeB;
		byte ManufacturerCpuCodeC;
		byte ManufacturerCpuCodeD;
	


};

#endif // MERGNODEIDENTIFICATION_H
