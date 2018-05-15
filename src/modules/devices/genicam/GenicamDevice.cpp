/**
 * @file	src/modules/devices/genicam/GenicamDevice.cpp
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

// do nothing, if the camera can't be built
#ifdef HAVE_OPENCV
#ifdef HAVE_GENAPI

#include "GenicamDevice.h"

#include "core/ModuleFactoryBranch.h"

#include "Poco/NumberFormatter.h"
#include "Poco/String.h" // toUpper, cat
#include "Poco/RegularExpression.h"
#include "Poco/StringTokenizer.h"

#include "Poco/ByteOrder.h"

using namespace GenApi;

GenicamDevice::GenicamDevice(GenTLLib* genTL,
        GenTL::IF_HANDLE TLhInterface,
        GenTL::DEV_HANDLE TLhDevice, ModuleFactory* parent,
        std::string customName):
    Module(parent, customName),
    mGenTL(genTL),
    hInterface(TLhInterface),
    hDevice(TLhDevice),
    hDataStream(GENTL_INVALID_HANDLE),
    hEvent(GENTL_INVALID_HANDLE),
    hRemoteDevPort(GENTL_INVALID_HANDLE),
    imgBufSize(0),
    bPixFormatMono8(true),
    imgWidth(0), imgHeight(0),
    acquiring(false),
	transportObj(genTL, hRemoteDevPort),
	seqIndex(0),
	pOutAttr(NULL)
{
	for (size_t ind = 0; ind < BUFFER_COUNT; ind++)
	{
		hBuffer[ind] = GENTL_INVALID_HANDLE;
		pImgBuffer[ind] = NULL;
	}

    setInternalName(static_cast<ModuleFactoryBranch*>(static_cast<ModuleFactoryBranch*>(parent)->parent())->getSelector());
    setCustomName(customName);
    setLogger("module." + name());

	connectNodeMap();

	std::string config(reinterpret_cast<ModuleFactoryBranch*>(parent)->getSelector());
	if (config != "None")
		loadConf(config);

    // ports >>> see CameraFromFiles
    setInPortCount(inPortCnt);
    setOutPortCount(outPortCnt);

    addTrigPort("trig", "Launch the image generation", trigPort);

    addOutPort("acqReady", "acquisition ready trigger", DataItem::typeInt32, acqReadyOutPort);
    addOutPort("image", "image from file", DataItem::typeCvMat, imgOutPort);

    retrieveDataStream();

    notifyCreation();
}

GenicamDevice::~GenicamDevice()
{
    if (acquiring)
        stopAcq();

    try
    {
        mGenTL->DSClose(hDataStream);
        poco_information(logger(),"DS closed");
    }
    catch (GenTLException& e) // could be not opened
    {
        poco_warning(logger(),
            std::string("Closing GenTL datastream: ") + e.displayText());
    }

    hDataStream = GENTL_INVALID_HANDLE;
}

void GenicamDevice::connectNodeMap()
{
	 try
     {
         mGenTL->DevGetPort(hDevice,&hRemoteDevPort);

         poco_information(logger(), "Port retrieved via DevGetPort()");
         // mGenTL->genTLPortInfo(_genTLhRemoteDevPort, logger()); // device info. device dependant
		 mGenTL->retrieveNodeMap(hRemoteDevPort, nodeMap, logger());

		 try
		 {
			nodeMap._Connect(&transportObj); 
		 }
		 catch (GenICam::GenericException& e)
		 {
			 throw GenTLException("GenAPI::connect", e.GetDescription());
		 }
     }
     catch (GenTLException& e)
     {
         poco_error(logger(), "Not able to connect the Genicam camera " + name()
                 + " node map: " + e.displayText());
         throw;
     }
}

void GenicamDevice::loadConf(std::string filePath)
{
    simpleYamlParse(filePath);

	// populate parameters using genParamList
	setParameterCount(genParamList.size());

	for (size_t ind = 0; ind < genParamList.size(); ind++)
	{
		bool errored = false;

		switch (genParamList[ind]->GetPrincipalInterfaceType())
		{
		case  intfIValue:
			addParameter(ind, genParamList[ind]->GetName().c_str(), 
				genParamList[ind]->GetDescription().c_str(), 
				ParamItem::typeString);
			poco_information(logger(), std::string("string (IValue) parameter added: ") 
				+ genParamList[ind]->GetName().c_str());
			break;
		case intfIBase:
			poco_information(logger(), "IBase interface");
			errored = true;
			break;
		case intfIInteger:
			addParameter(ind, genParamList[ind]->GetName().c_str(), 
				genParamList[ind]->GetDescription().c_str(), 
				ParamItem::typeInteger);
			poco_information(logger(), std::string("integer parameter added: ") 
				+ genParamList[ind]->GetName().c_str());
			break;
		case intfIBoolean:
			addParameter(ind, genParamList[ind]->GetName().c_str(), 
				genParamList[ind]->GetDescription().c_str(), 
				ParamItem::typeInteger);
			poco_information(logger(), std::string("integer (boolean) parameter added: ") 
				+ genParamList[ind]->GetName().c_str());
			break;
		case intfICommand:
			addParameter(ind, genParamList[ind]->GetName().c_str(), 
				genParamList[ind]->GetDescription().c_str(), 
				ParamItem::typeInteger);
			poco_information(logger(), std::string("integer (command) parameter added: ") 
				+ genParamList[ind]->GetName().c_str());
			break;
		case intfIFloat:
			addParameter(ind, genParamList[ind]->GetName().c_str(), 
				genParamList[ind]->GetDescription().c_str(), 
				ParamItem::typeFloat);
			poco_information(logger(), std::string("float parameter added: ") 
				+ genParamList[ind]->GetName().c_str());
			break;
		case intfIString:
			addParameter(ind, genParamList[ind]->GetName().c_str(), 
				genParamList[ind]->GetDescription().c_str(), 
				ParamItem::typeString);
			poco_information(logger(), std::string("string parameter added: ") 
				+ genParamList[ind]->GetName().c_str());
			break;
		case intfIRegister:
			poco_information(logger(), "IRegister interface");
			errored = true;
			break;
		case intfICategory:
			poco_information(logger(), "ICategory interface");
			errored = true;
			break;
		case intfIEnumeration:
			{
				std::string descr = genParamList[ind]->GetDescription().c_str();
				descr += "\nPossible values: \n";
				// get symbolics

				CEnumerationPtr ptr = genParamList[ind];
				StringList_t symbolics;
				ptr->GetSymbolics(symbolics);

				for (StringList_t::iterator it = symbolics.begin(), ite = symbolics.end();
						it != ite; it++)
				{
					descr += it->c_str();
					descr += " ; ";
				}

				addParameter(ind, genParamList[ind]->GetName().c_str(), 
					descr, 
					ParamItem::typeString);
				poco_information(logger(), std::string("string (enumeration) parameter added: ") 
					+ genParamList[ind]->GetName().c_str());
				break;
			}
		case intfIEnumEntry:
			poco_information(logger(), "IEnumEntry interface");
			errored = true;
			break;
		case intfIPort:
			poco_information(logger(), "IPort interface");
			errored = true;
			break;
		default:
			poco_information(logger(), "unknown interface type");
			errored = true;
		}

		if (errored)
		{
			// remove the entry from the genParamList
			genParamList.erase(genParamList.begin() + ind);
		}
	}

//    // default values === TODO: set all, then apply together. 
//    setIntParameterValue(paramPixelFormat, getIntParameterDefaultValue(paramPixelFormat));
//    setFloatParameterValue(paramExposure, getFloatParameterDefaultValue(paramExposure));
//    setStrParameterValue(paramTrigAct, getStrParameterDefaultValue(paramTrigAct));
}

void GenicamDevice::process(int startCond)
{
    bool trigged;

    switch (startCond)
    {
    case noDataStartState:
        trigged = false;
        break;
    case allDataStartState:
        trigged = true;
        break;
    default:
        poco_bugcheck_msg("impossible start condition");
        throw Poco::BugcheckException();
    }

    DataAttributeIn inAttr;

    bool stopAfterImage = false;
	bool doNotFlush = false;

    if (trigged)
    {
        readLockInPort(trigPort);
        readInPortDataAttribute(trigPort, &inAttr);
        releaseInPort(trigPort);
    }

    if (inAttr.isStartSequence(seqIndex))
    {
        if (startAcq())
			doNotFlush = true;

        if (pOutAttr)
        {
            delete pOutAttr;
            pOutAttr = NULL;
            poco_bugcheck_msg("output data attribute should be NULL but is not");
        }

        pOutAttr = new DataAttributeOut(inAttr);
        pOutAttr->startSequence();
    }
    else if (inAttr.isInSequence(seqIndex))
    {
        if (inAttr.isEndSequence(seqIndex))
        {
            stopAfterImage = true;

            if (pOutAttr == NULL)
                poco_bugcheck_msg("output data attribute should be allocated but is not");

            pOutAttr->endSequence();
        }
    }
    else
    {
        if (startAcq())
			doNotFlush = true;

        stopAfterImage = true;

        if (trigged)
            pOutAttr = new DataAttributeOut(inAttr);
        else
            pOutAttr = new DataAttributeOut();
    }

    poco_information(logger(),"Image awaiting preparation");

    int dataType;
    if (bPixFormatMono8)
        dataType = CV_8UC1;
    else
        dataType = CV_16UC1;

    poco_information(logger(),"time is (to)");
    Poco::Timestamp now;

    // genTLDSBufferInfo();

	poco_information(logger(), "retrieve acqReady output port lock");
    Poco::Int32* pInt32;
	reserveOutPort(acqReadyOutPort);
    getDataToWrite<Poco::Int32>(acqReadyOutPort, pInt32);
    *pInt32 = 1;
    poco_information(logger(), "acqReady locked; time is (to) + "
        + Poco::NumberFormatter::format(now.elapsed()/1000));

	//if (!doNotFlush)
	//{
	//	poco_information(logger(), "flush the event queue to get the last image");
	//	mGenTL->EventFlush(hEvent);

	//	// genTLDSBufferInfo(); // check if is still grabbing

	//	// requeue last used buffer (lastUsed)
	//	try
	//	{
	//		mGenTL->DSQueueBuffer(
	//			hDataStream,
	//			hBuffer[lastUsed]
	//			);
	//	}
	//	catch (GenTLException& e)
	//	{
	//		poco_warning(logger(), e.displayText());
	//	}

	//	poco_information(logger(), "Event flushed; time is (to) + "
	//		+ Poco::NumberFormatter::format(now.elapsed()/1000));
	//}

    notifyOutPortReady(acqReadyOutPort, *pOutAttr);
    poco_information(logger(), "acq ready output port set dispatched");

    GenTL::EVENT_NEW_BUFFER_DATA data ;
    size_t tmpSize=sizeof(data);

    poco_information(logger(), "Waiting for the image...");

    // genTLDSBufferInfo();

	mGenTL->EventGetData(hEvent,&data,&tmpSize,GENTL_INFINITE); // Baumer genTL.cti only supports GENTL_INFINITE
    //// ----------------------------
    //poco_information(logger(),"got it. ");
    //
    //mGenTL->DSQueueBuffer(
    //  _genTLhDataStream,
    //  _genTLhBuffer1
    //  );
    //
    //mGenTL->EventGetData(_genTLhEvent,&data,&tmpSize,2000);
    //poco_information(logger(),"and twice. ");
    //// ----------------------------

    poco_information(logger(), "Got the image; time is (to) + "
        + Poco::NumberFormatter::format(now.elapsed()/1000)
		+ ". Call processingTerminated(). ");
    processingTerminated();

    char* buffer = NULL; // temporary image buffer address

	for (size_t ind = 0; ind < BUFFER_COUNT; ind++)
	{
		if (data.BufferHandle == hBuffer[ind])
		{
			poco_information(logger(),"buffer#" 
				+ Poco::NumberFormatter::format(ind) + " was filled");
			buffer = pImgBuffer[ind];
			lastUsed = ind;
		}
    }
    if (buffer == NULL)
        throw Poco::RuntimeException(name() + ".process", "Unrecognized buffer");

    cv::Mat imgOut;
	cv::Mat(imgHeight, imgWidth, dataType, buffer).copyTo(imgOut);

    if(!imgOut.data )   // Check for invalid input
    {
		throw Poco::DataException(name() + ".process", "unable to construct the cv::Mat data");
    }

    poco_information(logger(), "Image retrieved; time is (to) + "
        + Poco::NumberFormatter::format(now.elapsed()/1000));

	//genTLDSBufferInfo(); // check if is still grabbing

	if (stopAfterImage)
		stopAcq();
	else
		requeueBuffers();

	reserveOutPort(imgOutPort);

    cv::Mat* pMat;
    getDataToWrite<cv::Mat>(imgOutPort, pMat);

	*pMat = imgOut;

    if (!pMat->data)
        poco_warning(logger(), "Empty image. Check the given file name. ");

    notifyOutPortReady(imgOutPort, *pOutAttr);

    if (stopAfterImage)
    {
        delete pOutAttr;
        pOutAttr = NULL;
    }
    else
    {
        (*pOutAttr)++;
    }
}

void GenicamDevice::requeueBuffers()
{
	GenTL::INFO_DATATYPE dataType;
	bool flag;
	size_t flagSize = 1;

	for (size_t ind = 0; ind < BUFFER_COUNT; ind++)
	{
	    mGenTL->DSGetBufferInfo( hDataStream, hBuffer[ind],
                GenTL::BUFFER_INFO_IS_QUEUED,
                &dataType,
                &flag, &flagSize);

		if (!flag)
		{
			poco_information(logger(), "requeue buffer#"
				+ Poco::NumberFormatter::format(ind));
			try
			{
				mGenTL->DSQueueBuffer(
					hDataStream,
					hBuffer[ind]
					);
			}
			catch (GenTLException& e)
			{
				poco_warning(logger(), e.displayText());
			}
		}
	}
	poco_information(logger(), "requeuing done. ");
}

bool GenicamDevice::startAcq()
{
    if (acquiring)
        return false;

    Poco::ScopedLock<Poco::FastMutex> lock(acqControlMutex);

    allocBuffers();

    // start acquisition (infinite!)
    poco_information(logger(),"start");
    mGenTL->DSStartAcquisition(hDataStream, 0, GENTL_INFINITE);

    // genTLDSBufferInfo();

    // start grabing
	CCommandPtr ptr = nodeMap._GetNode("AcquisitionStart");
	if (ptr)
		ptr->Execute();
	else
		poco_warning(logger(), "AcquisitionStart node not present");

    acquiring = true;
	return true;
}

void GenicamDevice::stopAcq()
{
    if (!acquiring)
        return;

    Poco::ScopedLock<Poco::FastMutex> lock(acqControlMutex);

	poco_information(logger(), "Stopping acquisition...");
    // stop grabing
	try
	{
		mGenTL->EventKill(hEvent);
		poco_information(logger(),"GenTL event killed...");

		Poco::Thread::yield();

		mGenTL->DSStopAcquisition(hDataStream,0);
	}
	catch (GenTLException& e)
	{
		poco_error(logger(), std::string("StopAcq failed: ") + e.displayText());
	}
	
	try
	{
		CCommandPtr ptr = nodeMap._GetNode("AcquisitionStop");
		if (ptr)
			ptr->Execute();
		else
			poco_warning(logger(), "AcquisitionStop node not present");
	}
	catch (GenICam::GenericException& e)
	{
		poco_error(logger(), std::string("StopAcq failed: ") + e.GetDescription());
		acquiring = false;
	}
	catch (GenTLException& e)
	{
		poco_error(logger(), std::string("StopAcq failed: ") + e.displayText());
	}

	revokeBuffers();

	acquiring = false;
}

void GenicamDevice::cancel()
{
    stopAcq();
}

void GenicamDevice::reset()
{
    seqIndex = 0;

    if (pOutAttr)
    {
        delete pOutAttr;
        pOutAttr = NULL;
    }
}

void GenicamDevice::allocBuffers()
{
    poco_information(logger(), "Retrieving payload size... ");

	CIntegerPtr pWidth = nodeMap._GetNode("Width");
	CIntegerPtr pHeight = nodeMap._GetNode("Height");

	if (pWidth == NULL || pHeight == NULL)
		throw GenTLException("AllocBuffer", "Unable to retrieve width, height");

	imgWidth = static_cast<int>(pWidth->GetValue());
	imgHeight = static_cast<int>(pHeight->GetValue());

    poco_information(logger(), "width: " + Poco::NumberFormatter::format(imgWidth));
    poco_information(logger(), "height: " + Poco::NumberFormatter::format(imgHeight));

	// update pixel format
	getPixelFormat();

	CIntegerPtr pPayload = nodeMap._GetNode("PayloadSize");

	if (pPayload == NULL)
		throw GenTLException("AllocBuffer", "Unable to retrieve the payload size");

	imgBufSize = pPayload->GetValue() ;

    poco_information(logger(),"Calculated payload size is: "
        + Poco::NumberFormatter::format(imgBufSize));

    genTLDSBufferInfo();

	for (size_t ind = 0; ind < BUFFER_COUNT; ind++)
	{
		try
		{
			pImgBuffer[ind] = new char[imgBufSize];
		}
		catch (std::bad_alloc&) // error in `new char[]`
		{
			pImgBuffer[ind] = NULL;
			imgBufSize = 0;
			while (--ind >= 0)
			{
				delete[] pImgBuffer[ind];
				pImgBuffer[ind] = NULL;
			}
			throw Poco::OutOfMemoryException("AllocImageBuffer",
				"Not able to allocate enough memory");
		}
	}

	for (size_t ind = 0; ind < BUFFER_COUNT; ind++)
	{
		hBuffer[ind] = GENTL_INVALID_HANDLE;
		mGenTL->DSAnnounceBuffer(
			hDataStream,
			pImgBuffer[ind],
			imgBufSize,
			NULL,
			&hBuffer[ind]);
	}

    // register new buffer ready event
    mGenTL->GCRegisterEvent(
        hDataStream,
        GenTL::EVENT_NEW_BUFFER,
        &hEvent)  ;

    poco_information(logger(),"New buffer event is now registered. "
		"Enqueuing all buffers. ");

	for (size_t ind = 0; ind < BUFFER_COUNT; ind++)
	{
		mGenTL->DSQueueBuffer(
			hDataStream,
			hBuffer[ind]
			);
	}
    genTLDSBufferInfo();

    poco_information(logger(),"image buffer enqueued for acquisition");
}

void GenicamDevice::revokeBuffers()
{
    poco_information(logger(),"revoking buffers...");

    poco_information(logger(),"flush queue");
    mGenTL->DSFlushQueue(hDataStream,GenTL::ACQ_QUEUE_ALL_DISCARD);

    // unregister event
    mGenTL->GCUnregisterEvent(
        hDataStream,
        GenTL::EVENT_NEW_BUFFER);
    poco_information(logger(),"new buffer event unregistered");

    genTLDSBufferInfo();

	// revoke all announced buffer, not regarding if they are still referenced
	// as announced by DSGetInfo

    for (size_t ind = 0; ind < BUFFER_COUNT; ind++)
    {
		if (hBuffer[ind] == GENTL_INVALID_HANDLE)
			break;
		
        try
        {
			poco_information(logger(),"revoking buffer#" 
				+ Poco::NumberFormatter::format(ind));
            mGenTL->DSRevokeBuffer(
                    hDataStream,
                    hBuffer[ind], NULL,NULL );
        }
        catch (GenTLException& e)
        {
            poco_error(logger(),e.displayText());
        }

		hBuffer[ind] = GENTL_INVALID_HANDLE;
        delete[] pImgBuffer[ind];
        pImgBuffer[ind] = NULL;
    }

    imgBufSize = 0;
}

void GenicamDevice::dispGCDataType(GenTL::INFO_DATATYPE dataType)
{
    switch (dataType)
    {
    case GenTL::INFO_DATATYPE_BOOL8:
        poco_information(logger(),"data type is bool8");
        break;
    case GenTL::INFO_DATATYPE_BUFFER:
        poco_information(logger(),"data type is buffer");
        break;
    case GenTL::INFO_DATATYPE_FLOAT64:
        poco_information(logger(),"data type is float64");
        break;
    case GenTL::INFO_DATATYPE_INT16:
        poco_information(logger(),"data type is int16");
        break;
    case GenTL::INFO_DATATYPE_INT32:
        poco_information(logger(),"data type is int32");
        break;
    case GenTL::INFO_DATATYPE_INT64:
        poco_information(logger(),"data type is int64");
        break;
    case GenTL::INFO_DATATYPE_PTR:
        poco_information(logger(),"data type is PTR");
        break;
    case GenTL::INFO_DATATYPE_PTRDIFF:
        poco_information(logger(),"data type is ptr diff");
        break;
    case GenTL::INFO_DATATYPE_SIZET:
        poco_information(logger(),"data type is size_t");
        break;
    case GenTL::INFO_DATATYPE_STRING:
        poco_information(logger(),"data type is string");
        break;
    case GenTL::INFO_DATATYPE_STRINGLIST:
        poco_information(logger(),"data type is string list");
        break;
    case GenTL::INFO_DATATYPE_UINT16:
        poco_information(logger(),"data type is uint16");
        break;
    case GenTL::INFO_DATATYPE_UINT32:
        poco_information(logger(),"data type is uint32");
        break;
    case GenTL::INFO_DATATYPE_UINT64:
        poco_information(logger(),"data type is uint64");
        break;
    case GenTL::INFO_DATATYPE_UNKNOWN:
        poco_information(logger(),"data type is inknown");
        break;
    default:
        poco_information(logger(),"data type is not supported");
    }
}

void GenicamDevice::retrieveDataStream()
{
    uint32_t numDataStreams;
    mGenTL->DevGetNumDataStreams(hDevice, &numDataStreams);

    poco_information(logger(),
        Poco::NumberFormatter::format(numDataStreams) + " datastream(s) available");

    if (numDataStreams)
    {
        size_t size;
        char* pDsID;
        mGenTL->DevGetDataStreamID(hDevice,0,NULL,&size);
        pDsID = new char[size];
        try
        {
            mGenTL->DevGetDataStreamID(hDevice,0,pDsID,&size);
            mGenTL->DevOpenDataStream(hDevice,pDsID,&hDataStream);

            poco_information(logger(), std::string(pDsID) + " datastream opened...");

            mGenTL->genTLPortInfo(hDataStream, logger());
        }
        catch (...)
        {
            delete[] pDsID;
            throw;
        }

        delete[] pDsID;
    }
}

void GenicamDevice::genTLDSBufferInfo()
{
    poco_information(logger(), "querying datastream buffer info...");

    GenTL::INFO_DATATYPE dataType;

    bool8_t flag;
    size_t flagSize = sizeof(flag);

    size_t data;
    size_t dataSize = sizeof(data);

    try
    {

    mGenTL->DSGetInfo(hDataStream,
        GenTL::STREAM_INFO_IS_GRABBING,
        &dataType,
        &flag,
        &flagSize);

    if (flag)
        poco_information(logger(), "Grabbing...");
    else
        poco_information(logger(), "Not grabbing yet.");

    //// not implemented
    //mGenTL->DSGetInfo(_genTLhDataStream,
    //  GenTL::STREAM_INFO_DEFINES_PAYLOADSIZE,
    //  &dataType,
    //  &flag,
    //  &flagSize);

    //if (flag)
    //  poco_information(logger(), "payload size defined by DS...");
    //else
    //  poco_information(logger(), "payload size not available.");

    //mGenTL->DSGetInfo(_genTLhDataStream,
    //  GenTL::STREAM_INFO_PAYLOAD_SIZE,
    //  &dataType,
    //  &data,
    //  &dataSize);

    //poco_information(logger(),
    //  "Payload size: "
    //  + Poco::NumberFormatter::format(data) );

    //mGenTL->DSGetInfo(_genTLhDataStream,
    //  GenTL::STREAM_INFO_BUF_ANNOUNCE_MIN,
    //  &dataType,
    //  &data,
    //  &dataSize);

    //poco_information(logger(),
    //  "Min announced buffer(s): "
    //  + Poco::NumberFormatter::format(data) );

    mGenTL->DSGetInfo(hDataStream,
        GenTL::STREAM_INFO_NUM_ANNOUNCED,
        &dataType,
        &data,
        &dataSize);

    poco_information(logger(),
        Poco::NumberFormatter::format(data)
        + " buffer(s) are announced. ");

    size_t numBuf = data;

    mGenTL->DSGetInfo(hDataStream,
        GenTL::STREAM_INFO_NUM_QUEUED,
        &dataType,
        &data,
        &dataSize);

    poco_information(logger(),
        Poco::NumberFormatter::format(data)
        + " buffer(s) are queued. ");

    // baumer API
    numBuf += data;

// not available with Baumer sdk
    //mGenTL->DSGetInfo(_genTLhDataStream,
    //  GenTL::STREAM_INFO_NUM_AWAIT_DELIVERY,
    //  &dataType,
    //  &data,
    //  &dataSize);

    //poco_information(logger(),
    //  Poco::NumberFormatter::format(data)
    //  + " buffer(s) await delivery. ");

    for (size_t bufIndex=0; bufIndex < numBuf; bufIndex++)
    {
    try
    {
        GenTL::BUFFER_HANDLE hBuf;

        mGenTL->DSGetBufferID(  hDataStream,
                            static_cast<uint32_t>(bufIndex),
                            &hBuf );

        mGenTL->DSGetBufferInfo( hDataStream, hBuf,
                    GenTL::BUFFER_INFO_SIZE,
                    &dataType,
                    &data, &dataSize);
        poco_information(logger(),
            "buffer#" + Poco::NumberFormatter::format(bufIndex)
            + " size: " + Poco::NumberFormatter::format(data) );

        mGenTL->DSGetBufferInfo( hDataStream, hBuf,
                    GenTL::BUFFER_INFO_SIZE_FILLED,
                    &dataType,
                    &data, &dataSize);
        poco_information(logger(),
            "buffer#" + Poco::NumberFormatter::format(bufIndex)
            + " size filled: " + Poco::NumberFormatter::format(data) );

        try {
        mGenTL->DSGetBufferInfo( hDataStream, hBuf,
            GenTL::BUFFER_INFO_IMAGEPRESENT,
                    &dataType,
                    &flag, &flagSize);
        if (flag)
            poco_information(logger(),
                "buffer#" + Poco::NumberFormatter::format(bufIndex) + " has image. ");
        else
            poco_information(logger(),
                "buffer#" + Poco::NumberFormatter::format(bufIndex) + " has NO image. ");
        } catch (GenTLException&) { }

        mGenTL->DSGetBufferInfo( hDataStream, hBuf,
                    GenTL::BUFFER_INFO_IS_QUEUED,
                    &dataType,
                    &flag, &flagSize);
        if (flag)
            poco_information(logger(),
                "buffer#" + Poco::NumberFormatter::format(bufIndex) + " is queued. ");
        else
            poco_information(logger(),
                "buffer#" + Poco::NumberFormatter::format(bufIndex) + " is NOT queued. ");

        mGenTL->DSGetBufferInfo( hDataStream, hBuf,
                    GenTL::BUFFER_INFO_IS_ACQUIRING,
                    &dataType,
                    &flag, &flagSize);
        if (flag)
            poco_information(logger(),
                "buffer#" + Poco::NumberFormatter::format(bufIndex) + " is acquiring. ");
        else
            poco_information(logger(),
                "buffer#" + Poco::NumberFormatter::format(bufIndex) + " is NOT acquiring. ");

        mGenTL->DSGetBufferInfo( hDataStream, hBuf,
            GenTL::BUFFER_INFO_NEW_DATA,
                    &dataType,
                    &flag, &flagSize);
        if (flag)
            poco_information(logger(),
                "buffer#" + Poco::NumberFormatter::format(bufIndex) + " has new data. ");
        else
            poco_information(logger(),
                "buffer#" + Poco::NumberFormatter::format(bufIndex) + " has NO new data. ");

        mGenTL->DSGetBufferInfo( hDataStream, hBuf,
                    GenTL::BUFFER_INFO_IS_INCOMPLETE,
                    &dataType,
                    &flag, &flagSize);
        if (flag)
            poco_information(logger(),
                "buffer#" + Poco::NumberFormatter::format(bufIndex) + " is incomplete. ");
        else
            poco_information(logger(),
                "buffer#" + Poco::NumberFormatter::format(bufIndex) + " is complete. ");
    }
    catch (GenTLException& e)
    {
        poco_warning(logger(),e.displayText());
    }

    } // end for
    }
    catch (GenTLException& e)
    {
        poco_warning(logger(),e.displayText());
    }
}

std::string GenicamDevice::getStrParameterValue(size_t paramIndex)
{
	return getGenicamStrProperty(genParamList[paramIndex]);
}

void GenicamDevice::setStrParameterValue(size_t paramIndex, std::string value)
{
	setGenicamProperty(genParamList[paramIndex], value);
}

Poco::Int64 GenicamDevice::getIntParameterValue(size_t paramIndex)
{
	return getGenicamIntProperty(genParamList[paramIndex]);
}

void GenicamDevice::setIntParameterValue(size_t paramIndex, Poco::Int64 value)
{
	setGenicamIntProperty(genParamList[paramIndex], value);
}

double GenicamDevice::getFloatParameterValue(size_t paramIndex)
{
	return getGenicamFloatProperty(genParamList[paramIndex]);
}

void GenicamDevice::setFloatParameterValue(size_t paramIndex, double value)
{
	setGenicamFloatProperty(genParamList[paramIndex], value);
}

void GenicamDevice::setGenicamProperty(GenApi::CNodePtr node, std::string value)
{
	try
	{
		CValuePtr ptr = node;
		ptr->FromString(value.c_str());
	}
	catch (GenICam::GenericException& e)
	{
		throw GenTLException("setGenicamProperty", e.GetDescription());
	}
}

void GenicamDevice::setGenicamFloatProperty(GenApi::CNodePtr node, double value)
{
	try
	{
		std::string errMsg;
		switch (node->GetPrincipalInterfaceType())
		{
		case intfIFloat:
			{
				CFloatPtr ptr = node;
				ptr->SetValue(value);
				return;
			}
		case  intfIValue:
			errMsg = "IValue interface";
			break;
		case intfIBase:
			errMsg = "IBase interface";
			break;
		case intfIInteger:
			errMsg = "IInteger interface";
			break;
		case intfIBoolean:
			errMsg = "IBoolean interface";
			break;
		case intfICommand:
			errMsg = "ICommand interface";
			break;
		case intfIString:
			errMsg = "IString interface";
			break;
		case intfIRegister:
			errMsg = "IRegister interface";
			break;
		case intfICategory:
			errMsg = "ICategory interface";
			break;
		case intfIEnumeration:
			errMsg = "IEnumeration interface";
			break;
		case intfIEnumEntry:
			errMsg = "IEnumEntry interface";
			break;
		case intfIPort:
			errMsg = "IPort interface";
			break;
		default:
			poco_information(logger(), "unknown interface type");
		}

		throw Poco::DataFormatException("setGenicamProperty", errMsg);
	}
	catch (GenICam::GenericException& e)
	{
		throw GenTLException("setGenicamProperty", e.GetDescription());
	}

}

void GenicamDevice::setGenicamIntProperty(GenApi::CNodePtr node, Poco::Int64 value)
{
	try
	{
		std::string errMsg;
		switch (node->GetPrincipalInterfaceType())
		{
		case intfIInteger:
			{
				CIntegerPtr ptr = node;
				ptr->SetValue(value);
				return;
			}
		case intfIBoolean:
			{
				CBooleanPtr ptr = node;
				ptr->SetValue(value != 0);
				return;
			}
		case intfICommand:
			{
				CCommandPtr ptr = node;
				if (value)
					ptr->Execute();
				return;
			}
		case  intfIValue:
			errMsg = "IValue interface";
			break;
		case intfIBase:
			errMsg = "IBase interface";
			break;
		case intfIFloat:
			errMsg = "IFloat interface";
			break;
		case intfIString:
			errMsg = "IString interface";
			break;
		case intfIRegister:
			errMsg = "IRegister interface";
			break;
		case intfICategory:
			errMsg = "ICategory interface";
			break;
		case intfIEnumeration:
			errMsg = "IEnumeration interface";
			break;
		case intfIEnumEntry:
			errMsg = "IEnumEntry interface";
			break;
		case intfIPort:
			errMsg = "IPort interface";
			break;
		default:
			errMsg = "unknown interface type";
		}

		throw Poco::DataFormatException("setGenicamProperty", errMsg);
	}
	catch (GenICam::GenericException& e)
	{
		throw GenTLException("setGenicamProperty", e.GetDescription());
	}
}

std::string GenicamDevice::getGenicamStrProperty(GenApi::CNodePtr node)
{
	try
	{
		CValuePtr ptr = node;
		return ptr->ToString().c_str();
	}
	catch (GenICam::GenericException& e)
	{
		throw GenTLException("getGenicamProperty", e.GetDescription());
	}
}

double GenicamDevice::getGenicamFloatProperty(GenApi::CNodePtr node)
{
	try
	{
		std::string errMsg;
		switch (node->GetPrincipalInterfaceType())
		{
		case intfIFloat:
			{
				CFloatPtr ptr = node;
				return ptr->GetValue();
			}
		case  intfIValue:
			errMsg = "IValue interface";
			break;
		case intfIBase:
			errMsg = "IBase interface";
			break;
		case intfIInteger:
			errMsg = "IInteger interface";
			break;
		case intfIBoolean:
			errMsg = "IBoolean interface";
			break;
		case intfICommand:
			errMsg = "ICommand interface";
			break;
		case intfIString:
			errMsg = "IString interface";
			break;
		case intfIRegister:
			errMsg = "IRegister interface";
			break;
		case intfICategory:
			errMsg = "ICategory interface";
			break;
		case intfIEnumeration:
			errMsg = "IEnumeration interface";
			break;
		case intfIEnumEntry:
			errMsg = "IEnumEntry interface";
			break;
		case intfIPort:
			errMsg = "IPort interface";
			break;
		default:
			poco_information(logger(), "unknown interface type");
		}

		throw Poco::DataFormatException("getGenicamProperty", errMsg);
	}
	catch (GenICam::GenericException& e)
	{
		throw GenTLException("getGenicamProperty", e.GetDescription());
	}

}

Poco::Int64 GenicamDevice::getGenicamIntProperty(GenApi::CNodePtr node)
{
	try
	{
		std::string errMsg;
		switch (node->GetPrincipalInterfaceType())
		{
		case intfIInteger:
			{
				CIntegerPtr ptr = node;
				return ptr->GetValue();
			}
		case intfIBoolean:
			{
				CBooleanPtr ptr = node;
				return ptr->GetValue();
			}
		case intfICommand:
			{
				CCommandPtr ptr = node;
				if (ptr->IsDone())
					return 0;
				else
					return 1;
			}
		case  intfIValue:
			errMsg = "IValue interface";
			break;
		case intfIBase:
			errMsg = "IBase interface";
			break;
		case intfIFloat:
			errMsg = "IFloat interface";
			break;
		case intfIString:
			errMsg = "IString interface";
			break;
		case intfIRegister:
			errMsg = "IRegister interface";
			break;
		case intfICategory:
			errMsg = "ICategory interface";
			break;
		case intfIEnumeration:
			errMsg = "IEnumeration interface";
			break;
		case intfIEnumEntry:
			errMsg = "IEnumEntry interface";
			break;
		case intfIPort:
			errMsg = "IPort interface";
			break;
		default:
			errMsg = "unknown interface type";
		}

		throw Poco::DataFormatException("getGenicamProperty", errMsg);
	}
	catch (GenICam::GenericException& e)
	{
		throw GenTLException("getGenicamProperty", e.GetDescription());
	}
}

Poco::Int64 GenicamDevice::getPixelFormat()
{
	CEnumerationPtr pFormat;
	try
	{
		pFormat = nodeMap._GetNode("PixelFormat");
		if (pFormat == NULL)
			throw Poco::InvalidArgumentException("getPixelFormat", "PixelFormat node not found");
	}
	catch (GenICam::GenericException& e)
	{
		throw Poco::InvalidAccessException("getPixelFormat", 
			std::string("PixelFormat node: ") + e.GetDescription());
	}

	std::string format = (**pFormat).c_str();
	
	if (format == "Mono8")
	{
        bPixFormatMono8 = true;
		return 8;
	}
	else if (format == "Mono10")
	{
        bPixFormatMono8 = false;
		return 10;
	}
	else if (format == "Mono12")
	{
        bPixFormatMono8 = false;
		return 12;
	}
	else if (format == "Mono14")
	{
        bPixFormatMono8 = false;
		return 14;
	}
	else if (format == "Mono16")
	{
        bPixFormatMono8 = false;
		return 16;
	}
	else
        throw Poco::NotImplementedException("getPixelFormat",
                                "unreckognized pixel format");
}

#include <fstream>
#include <iostream>

void GenicamDevice::simpleYamlParse(std::string filePath)
{
    // TODO: use libYaml instead

    // warning: do not use logger() here. It mays not be initialized yet

    std::string line;
    std::ifstream file (filePath.c_str());

    if (!file.good())
        throw Poco::FileException("GenicamDevice::simpleYamlParse",
                "Not able to open the config file: " + filePath);

    int count = 0;
    std::getline(file, line);
    while (file.good())
    {
//        std::cout << count++ << ": " << line << std::endl;

        // check if new parameter
        std::string name = newProperty(line);
        if (!name.empty())
        {
			// retrieve node 
			CNodePtr node;

			try
			{
				node = nodeMap._GetNode(name.c_str());
			}
			catch (GenICam::GenericException& e)
			{
				poco_error(logger(), std::string("simpleYamlParse: ") 
					+ e.GetDescription());
				std::getline(file, line);
				continue;
			}

			if (node == NULL)
			{
				poco_warning(logger(), "The given property: "
					+ name + " does not exist. >>> SKIP <<<");
				std::getline(file, line);
				continue;
			}

			if (line.empty())
			{
				genParamList.push_back(node);
			}
			else
			{
				try
				{
					setGenicamProperty(node, line);
				}
				catch (GenICam::GenericException& e)
				{
					poco_error(logger(), std::string("simpleYamlParse: ") 
						+ e.GetDescription());
					std::getline(file, line);
					continue;
				}
			}

        }

		std::getline(file, line);
    }

    file.close();
}

using Poco::RegularExpression;

std::string GenicamDevice::newProperty(std::string& line)
{
    RegularExpression regex("(\\w+):\\s*(\\w*)", RegularExpression::RE_ANCHORED);
    std::vector<std::string> result;

    if(regex.split(line, result))
	{
		line = result[2];
        return result[1];
	}
    else
        return "";
}

#endif /* HAVE_GENAPI */
#endif /* HAVE_OPENCV */
