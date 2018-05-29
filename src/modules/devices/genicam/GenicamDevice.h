/**
 * @file	src/modules/devices/genicam/GenicamDevice.h
 * @date	Jan. 2017
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2017 Ph. Renaud-Goud / Opticalp

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

#ifndef SRC_MODULES_DEVICES_GENICAM_GENICAMDEVICE_H_
#define SRC_MODULES_DEVICES_GENICAM_GENICAMDEVICE_H_

#ifdef HAVE_OPENCV
#ifdef HAVE_GENAPI

#include "core/Module.h"
#include "core/DataAttributeOut.h"

#include "GenDevTransportLayer.h"

#include "GenTLLib.h"
#include "GenICam.h"

#include "Poco/Path.h"

#define BUFFER_COUNT 4

/**
 * GenicamDevice
 * 
 * GenICam Camera device that shall be exchangeable with the CameraFromFiles
 * fake camera.
 *
 * @par hefVision 2.0.0-dev.2 fork:
 * First implementation only valid with some Baumer cameras (e.g. VLG40M)
 *
 * @par hefVision 2.0.0-beta.3 fork:
 * Implementation for any genicam using the GenAPI reference implementation
 *
 * @par hefVision 2.0.0-beta.4 fork:
 * Implementation of the start/stop features using the data sequences
 */
class GenicamDevice: public Module
{
public:
    GenicamDevice(GenTLLib* genTL,
            GenTL::IF_HANDLE TLhInterface,
            GenTL::DEV_HANDLE TLhDevice,
            ModuleFactory* parent, std::string name);
    virtual ~GenicamDevice();

    std::string description()
    {
        return "Standard GenICam camera";
    }

private:
    GenicamDevice();

    /**
     * Main logic
     */
    void process(int startCond);

	/**
	 * - retrieve the TL device port
	 * - load the node map
	 */
	void connectNodeMap();

    /**
     * Load the config file selected by the parent factory
     */
    void loadConf(std::string filePath);

    /**
     * Retrieve the GenTL datastreams
     *
     * - query the number of available datastreams,
     * - open the first datastream
     * - retrieve the handle _genTLhDataStream on this opened DS
     */
    void retrieveDataStream();

    /**
     * Start acquisition
     *
     * Wind the camera to be able to get triggers
	 *
	 * @return true if the acquisition was effectively started
     */
    bool startAcq();

    /**
     * Stop acquisition
     *
     * Stop the camera (do not get triggers any more)
     */
    void stopAcq();

    /**
     * Allocate and announce the buffers
     *
     * 2 rolling buffers will be used
     */
    void allocBuffers();

     /**
      * Revoke and free the buffers
      */
    void revokeBuffers();

    /**
     * Stop the acquisition
     *
     * kill the acq done event
     */
    void cancel();

    /**
     * Reset the input and output data sequence (if any)
     */
    void reset();

    enum params
    {
        paramExposure,
        paramPixelFormat, ///< mono8 or mono12
        paramTrigAct, ///< raising, falling edge
        paramCnt ///< number of parameters in the parameter set
    };

    std::string getStrParameterValue(size_t paramIndex);
    void setStrParameterValue(size_t paramIndex, std::string value);

    Poco::Int64 getIntParameterValue(size_t paramIndex);
    void setIntParameterValue(size_t paramIndex, Poco::Int64 value);

    double getFloatParameterValue(size_t paramIndex);
    void setFloatParameterValue(size_t paramIndex, double value);

    /// Indexes of the input ports
    enum inPorts
    {
        trigPort,
        inPortCnt
    };

    /// Indexes of the output ports
    enum outPorts
    {
        imgOutPort,
        acqReadyOutPort,
        outPortCnt
    };

    /// image width
    int imgWidth;
    /// image height
    int imgHeight;

    /// pixel format storing variable
    bool bPixFormatMono8;

    /**
     * Get the pixel format
     *
     * if the module is not loaded, the function only reads the local storage
     */
    Poco::Int64 getPixelFormat();

    Poco::FastMutex acqControlMutex; ///< mutex locked during startAcq or stopAcq operations
    bool acquiring; ///< flag indicating if the camera is ready to be trigged

    GenTLLib* mGenTL;

    GenTL::IF_HANDLE hInterface; ///< handle on the GenTL interface module
    GenTL::DEV_HANDLE hDevice; ///< handle on the GenTL device module
    GenTL::DS_HANDLE hDataStream; ///< handle on the GenTL datastream module
    GenTL::BUFFER_HANDLE hBuffer[BUFFER_COUNT]; ///< handle on the GenTL buffer
    GenTL::EVENT_HANDLE hEvent; ///< handle on the GenTL new buffer event handle
    GenTL::PORT_HANDLE hRemoteDevPort; ///< handle on the GenTL remote device port handle

    char* pImgBuffer[BUFFER_COUNT]; ///< data buffers
    size_t imgBufSize; ///< data buffer size
	size_t lastUsed; ///< last used buffer

   /// get various about the buffers in the DS
    void genTLDSBufferInfo();

	/// requeue dequeued buffers
	void requeueBuffers();

    void dispGCDataType(GenTL::INFO_DATATYPE dataType);
 
	GenApi::CNodeMapRef nodeMap; ///< GenAPI node map to access device properties

	GenDevTransportLayer transportObj; ///< GenICam device transport layer object

    /**
     * Parse the config file as a yaml file.
     *
     * a custom very simple yaml parser is used.
	 *
	 * Some genAPI properties are set here
     */
    void simpleYamlParse(std::string filePath);

	/**
     * Check if the given line corresponds to a property name definition
     *
	 * @parameter line string to parse. filled with the remaining characters after the call.  
     * @return property name
     */
    std::string newProperty(std::string& line);

	/**
	 * To set a property value without knowing its type
	 * or knowning that it has string type (or enum)
	 * 
	 * Can be called while parsing the yaml config file
	 *
	 */
	void setGenicamProperty(GenApi::CNodePtr node, std::string value);

	/**
	 * Set a intfIFloat property value
	 */
	void setGenicamFloatProperty(GenApi::CNodePtr node, double value);

	/** 
	 * Set a property value, knowing its type
	 *
	 * Its type is among: 
	 *  - intfIInteger
	 *  - intfIBoolean (0 or 1)
	 *  - intfICommand (set to 1)
	 */
	void setGenicamIntProperty(GenApi::CNodePtr node, Poco::Int64 value);

	/**
	 * Get a property value, knowing its type
	 *
	 * Either intfIString or intfIEnumeration
	 */
	std::string getGenicamStrProperty(GenApi::CNodePtr node);

	/**
	 * Get a intfIFloat property value
	 */
	double getGenicamFloatProperty(GenApi::CNodePtr node);

	/** 
	 * Get a property value, knowing its type
	 *
	 * Its type is among: 
	 *  - intfIInteger
	 *  - intfIBoolean (0 or 1)
	 *  - intfICommand (set to 1)
	 *  - intfIRegister
	 */
	Poco::Int64 getGenicamIntProperty(GenApi::CNodePtr node);

	std::vector<GenApi::CNodePtr> genParamList; ///< Store the genAPI nodes as defined by the conf file

	Poco::Path confFile;

    size_t seqIndex; ///< input data attribute control
    DataAttributeOut* pOutAttr; ///< output attribute local storage
};

#endif /* HAVE_GENAPI */
#endif /* HAVE_OPENCV */
#endif /* SRC_MODULES_DEVICES_GENICAM_GENICAMDEVICE_H_ */
