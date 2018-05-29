/**
 * @file	src/modules/devices/genicam/GenTLLib.cpp
 * @date	jan. 2016
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2016 Ph. Renaud-Goud / Opticalp

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the"Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED"AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

#include "Poco/Logger.h"
#include "Poco/NumberFormatter.h"
#include "Poco/NumberParser.h"
#include "Poco/RegularExpression.h"
#include "Poco/String.h"

#include"GenTLLib.h"

#include <typeinfo>
POCO_IMPLEMENT_EXCEPTION( GenTLException, Poco::Exception,"Genicam Transport Layer error")

GenTLLib::GenTLLib(Poco::Path libTLPath):
    _libTLPath(libTLPath),
    TLhSystem(GENTL_INVALID_HANDLE)
{
#ifdef VERBOSE
    setLogger("GenTL");
#endif
    loadLib();
}

GenTLLib::~GenTLLib()
{
    _libTL.unload();
}

void GenTLLib::genTLCheckError(GenTL::GC_ERROR retValue)
{
    if (retValue != GenTL::GC_ERR_SUCCESS)
    {
        GenTL::GC_ERROR lastErr;
        char* txt;
        size_t txtSize;
        if (GenTL::GC_ERR_SUCCESS != pGCGetLastError(&lastErr,NULL,&txtSize))
        {
#ifdef VERBOSE
            poco_warning(logger(),"Unable to retrieve the genTL error using GCGetLastError");
#endif
        }
        else
        {
            txt = new char[txtSize];
            pGCGetLastError(&lastErr,txt,&txtSize);

            std::string msg(txt);
            delete[] txt;
#ifdef VERBOSE
            poco_warning(logger(),"GC last error is:" + msg);
#endif
        }
    }
    else
        return;


    switch (retValue)
    {
        case GenTL::GC_ERR_ERROR               :
            throw GenTLException("GenTL","generic error");
            break;
        case GenTL::GC_ERR_NOT_INITIALIZED     :
            throw GenTLException("GenTL","not initialized");
            break;
        case GenTL::GC_ERR_NOT_IMPLEMENTED     :
            throw GenTLException("GenTL","not implemented");
            break;
        case GenTL::GC_ERR_RESOURCE_IN_USE     :
            throw GenTLException("GenTL","resource in use");
            break;
        case GenTL::GC_ERR_ACCESS_DENIED       :
            throw GenTLException("GenTL","access denied");
            break;
        case GenTL::GC_ERR_INVALID_HANDLE      :
            throw GenTLException("GenTL","invalid handle");
            break;
        case GenTL::GC_ERR_INVALID_ID          :
            throw GenTLException("GenTL","invalid ID");
            break;
        case GenTL::GC_ERR_NO_DATA             :
            throw GenTLException("GenTL","no data");
            break;
        case GenTL::GC_ERR_INVALID_PARAMETER   :
            throw GenTLException("GenTL","invalid parameter");
            break;
        case GenTL::GC_ERR_IO                  :
            throw GenTLException("GenTL","IO error");
            break;
        case GenTL::GC_ERR_TIMEOUT             :
            throw GenTLException("GenTL","timeout");
            break;
        case GenTL::GC_ERR_ABORT               : /* GenTL v1.1 */
            throw GenTLException("GenTL","abort");
            break;
        case GenTL::GC_ERR_INVALID_BUFFER      : /* GenTL v1.1 */
            throw GenTLException("GenTL","invalid buffer");
            break;
        case GenTL::GC_ERR_NOT_AVAILABLE       : /* GenTL v1.2 */
            throw GenTLException("GenTL","not available");
            break;
        case GenTL::GC_ERR_INVALID_ADDRESS     : /* GenTL v1.3 */
            throw GenTLException("GenTL","invalid address");
            break;
        case GenTL::GC_ERR_BUFFER_TOO_SMALL    : /* GenTL v1.4 */
            throw GenTLException("GenTL","buffer too small");
            break;
        case GenTL::GC_ERR_INVALID_INDEX       : /* GenTL v1.4 */
            throw GenTLException("GenTL","invalid index");
            break;
        case GenTL::GC_ERR_PARSING_CHUNK_DATA  : /* GenTL v1.4 */
            throw GenTLException("GenTL","parsing chunk data");
            break;
        case GenTL::GC_ERR_INVALID_VALUE       : /* GenTL v1.4 */
            throw GenTLException("GenTL","invalid value");
            break;
        case GenTL::GC_ERR_RESOURCE_EXHAUSTED  : /* GenTL v1.4 */
            throw GenTLException("GenTL","resource exhausted");
            break;
        case GenTL::GC_ERR_OUT_OF_MEMORY       : /* GenTL v1.4 */
            throw GenTLException("GenTL","out of memory");
            break;

        case GenTL::GC_ERR_CUSTOM_ID           :
            throw GenTLException("GenTL","custom ID error");
            break;

        default:
            poco_bugcheck_msg("should not happen:"
               "Unrecognized GenTL error code");
            throw GenTLException();
    }
}

void GenTLLib::loadLib()
{
    try
    {
        _libTL.load(_libTLPath.toString()); // will also load the library
    }
    catch (Poco::LibraryLoadException& e)
    {
        throw GenTLException("loadLib","the given genTL library"
           "can not be opened: " + e.displayText());
    }

    // FIXME: indentations...
    try {   pGCGetInfo   = reinterpret_cast<GenTL::PGCGetInfo>(_libTL.getSymbol("GCGetInfo") ); } catch (Poco::NotFoundException&) {    pGCGetInfo   = NULL; }
    try {   pGCGetLastError  = reinterpret_cast<GenTL::PGCGetLastError>(_libTL.getSymbol("GCGetLastError") ); } catch (Poco::NotFoundException&) {    pGCGetLastError  = NULL; }

    // fatal error (exception raised) if GCInitLib is not found
    try
    {
        pGCInitLib   = reinterpret_cast<GenTL::PGCInitLib>(_libTL.getSymbol("GCInitLib") );
    }
    catch (Poco::NotFoundException&)
    {
        throw GenTLException("loadLib","the given genTL library"
            "can not be opened. Failed to find GCInitLib.");
    }

    try {   pGCCloseLib  = reinterpret_cast<GenTL::PGCCloseLib>(_libTL.getSymbol("GCCloseLib") ); } catch (Poco::NotFoundException&) {    pGCCloseLib  = NULL; }
    try {   pGCReadPort  = reinterpret_cast<GenTL::PGCReadPort>(_libTL.getSymbol("GCReadPort") ); } catch (Poco::NotFoundException&) {    pGCReadPort  = NULL; }
    try {   pGCWritePort     = reinterpret_cast<GenTL::PGCWritePort>(_libTL.getSymbol("GCWritePort") ); } catch (Poco::NotFoundException&) {    pGCWritePort     = NULL; }
    try {   pGCGetPortURL    = reinterpret_cast<GenTL::PGCGetPortURL>(_libTL.getSymbol("GCGetPortURL") ); } catch (Poco::NotFoundException&) {    pGCGetPortURL    = NULL; }
    try {   pGCGetPortInfo   = reinterpret_cast<GenTL::PGCGetPortInfo>(_libTL.getSymbol("GCGetPortInfo") ); } catch (Poco::NotFoundException&) {    pGCGetPortInfo   = NULL; }
    try {   pGCRegisterEvent     = reinterpret_cast<GenTL::PGCRegisterEvent>(_libTL.getSymbol("GCRegisterEvent") ); } catch (Poco::NotFoundException&) {    pGCRegisterEvent     = NULL; }
    try {   pGCUnregisterEvent   = reinterpret_cast<GenTL::PGCUnregisterEvent>(_libTL.getSymbol("GCUnregisterEvent") ); } catch (Poco::NotFoundException&) {    pGCUnregisterEvent   = NULL; }
    try {   pEventGetData    = reinterpret_cast<GenTL::PEventGetData>(_libTL.getSymbol("EventGetData") ); } catch (Poco::NotFoundException&) {    pEventGetData    = NULL; }
    try {   pEventGetDataInfo    = reinterpret_cast<GenTL::PEventGetDataInfo    >(_libTL.getSymbol("EventGetDataInfo") ); } catch (Poco::NotFoundException&) {    pEventGetDataInfo    = NULL; }
    try {   pEventGetInfo    = reinterpret_cast<GenTL::PEventGetInfo    >(_libTL.getSymbol("EventGetInfo") ); } catch (Poco::NotFoundException&) {    pEventGetInfo    = NULL; }
    try {   pEventFlush  = reinterpret_cast<GenTL::PEventFlush  >(_libTL.getSymbol("EventFlush") ); } catch (Poco::NotFoundException&) {    pEventFlush  = NULL; }
    try {   pEventKill   = reinterpret_cast<GenTL::PEventKill   >(_libTL.getSymbol("EventKill") ); } catch (Poco::NotFoundException&) {    pEventKill   = NULL; }
    try {   pTLOpen  = reinterpret_cast<GenTL::PTLOpen  >(_libTL.getSymbol("TLOpen") ); } catch (Poco::NotFoundException&) {    pTLOpen  = NULL; }
    try {   pTLClose     = reinterpret_cast<GenTL::PTLClose >(_libTL.getSymbol("TLClose") ); } catch (Poco::NotFoundException&) {    pTLClose     = NULL; }
    try {   pTLGetInfo   = reinterpret_cast<GenTL::PTLGetInfo   >(_libTL.getSymbol("TLGetInfo") ); } catch (Poco::NotFoundException&) {    pTLGetInfo   = NULL; }
    try {   pTLGetNumInterfaces  = reinterpret_cast<GenTL::PTLGetNumInterfaces  >(_libTL.getSymbol("TLGetNumInterfaces") ); } catch (Poco::NotFoundException&) {    pTLGetNumInterfaces  = NULL; }
    try {   pTLGetInterfaceID    = reinterpret_cast<GenTL::PTLGetInterfaceID    >(_libTL.getSymbol("TLGetInterfaceID") ); } catch (Poco::NotFoundException&) {    pTLGetInterfaceID    = NULL; }
    try {   pTLGetInterfaceInfo  = reinterpret_cast<GenTL::PTLGetInterfaceInfo  >(_libTL.getSymbol("TLGetInterfaceInfo") ); } catch (Poco::NotFoundException&) {    pTLGetInterfaceInfo  = NULL; }
    try {   pTLOpenInterface     = reinterpret_cast<GenTL::PTLOpenInterface >(_libTL.getSymbol("TLOpenInterface") ); } catch (Poco::NotFoundException&) {    pTLOpenInterface     = NULL; }
    try {   pTLUpdateInterfaceList   = reinterpret_cast<GenTL::PTLUpdateInterfaceList   >(_libTL.getSymbol("TLUpdateInterfaceList") ); } catch (Poco::NotFoundException&) {    pTLUpdateInterfaceList   = NULL; }
    try {   pIFClose     = reinterpret_cast<GenTL::PIFClose >(_libTL.getSymbol("IFClose") ); } catch (Poco::NotFoundException&) {    pIFClose     = NULL; }
    try {   pIFGetInfo   = reinterpret_cast<GenTL::PIFGetInfo   >(_libTL.getSymbol("IFGetInfo") ); } catch (Poco::NotFoundException&) {    pIFGetInfo   = NULL; }
    try {   pIFGetNumDevices     = reinterpret_cast<GenTL::PIFGetNumDevices >(_libTL.getSymbol("IFGetNumDevices") ); } catch (Poco::NotFoundException&) {    pIFGetNumDevices     = NULL; }
    try {   pIFGetDeviceID   = reinterpret_cast<GenTL::PIFGetDeviceID   >(_libTL.getSymbol("IFGetDeviceID") ); } catch (Poco::NotFoundException&) {    pIFGetDeviceID   = NULL; }
    try {   pIFUpdateDeviceList  = reinterpret_cast<GenTL::PIFUpdateDeviceList  >(_libTL.getSymbol("IFUpdateDeviceList") ); } catch (Poco::NotFoundException&) {    pIFUpdateDeviceList  = NULL; }
    try {   pIFGetDeviceInfo     = reinterpret_cast<GenTL::PIFGetDeviceInfo >(_libTL.getSymbol("IFGetDeviceInfo") ); } catch (Poco::NotFoundException&) {    pIFGetDeviceInfo     = NULL; }
    try {   pIFOpenDevice    = reinterpret_cast<GenTL::PIFOpenDevice    >(_libTL.getSymbol("IFOpenDevice") ); } catch (Poco::NotFoundException&) {    pIFOpenDevice    = NULL; }
    try {   pDevGetPort  = reinterpret_cast<GenTL::PDevGetPort  >(_libTL.getSymbol("DevGetPort") ); } catch (Poco::NotFoundException&) {    pDevGetPort  = NULL; }
    try {   pDevGetNumDataStreams    = reinterpret_cast<GenTL::PDevGetNumDataStreams    >(_libTL.getSymbol("DevGetNumDataStreams") ); } catch (Poco::NotFoundException&) {    pDevGetNumDataStreams    = NULL; }
    try {   pDevGetDataStreamID  = reinterpret_cast<GenTL::PDevGetDataStreamID  >(_libTL.getSymbol("DevGetDataStreamID") ); } catch (Poco::NotFoundException&) {    pDevGetDataStreamID  = NULL; }
    try {   pDevOpenDataStream   = reinterpret_cast<GenTL::PDevOpenDataStream   >(_libTL.getSymbol("DevOpenDataStream") ); } catch (Poco::NotFoundException&) {    pDevOpenDataStream   = NULL; }
    try {   pDevGetInfo  = reinterpret_cast<GenTL::PDevGetInfo  >(_libTL.getSymbol("DevGetInfo") ); } catch (Poco::NotFoundException&) {    pDevGetInfo  = NULL; }
    try {   pDevClose    = reinterpret_cast<GenTL::PDevClose    >(_libTL.getSymbol("DevClose") ); } catch (Poco::NotFoundException&) {    pDevClose    = NULL; }
    try {   pDSAnnounceBuffer    = reinterpret_cast<GenTL::PDSAnnounceBuffer    >(_libTL.getSymbol("DSAnnounceBuffer") ); } catch (Poco::NotFoundException&) {    pDSAnnounceBuffer    = NULL; }
    try {   pDSAllocAndAnnounceBuffer    = reinterpret_cast<GenTL::PDSAllocAndAnnounceBuffer    >(_libTL.getSymbol("DSAllocAndAnnounceBuffer") ); } catch (Poco::NotFoundException&) {    pDSAllocAndAnnounceBuffer    = NULL; }
    try {   pDSFlushQueue    = reinterpret_cast<GenTL::PDSFlushQueue    >(_libTL.getSymbol("DSFlushQueue") ); } catch (Poco::NotFoundException&) {    pDSFlushQueue    = NULL; }
    try {   pDSStartAcquisition  = reinterpret_cast<GenTL::PDSStartAcquisition  >(_libTL.getSymbol("DSStartAcquisition") ); } catch (Poco::NotFoundException&) {    pDSStartAcquisition  = NULL; }
    try {   pDSStopAcquisition   = reinterpret_cast<GenTL::PDSStopAcquisition   >(_libTL.getSymbol("DSStopAcquisition") ); } catch (Poco::NotFoundException&) {    pDSStopAcquisition   = NULL; }
    try {   pDSGetInfo   = reinterpret_cast<GenTL::PDSGetInfo   >(_libTL.getSymbol("DSGetInfo") ); } catch (Poco::NotFoundException&) {    pDSGetInfo   = NULL; }
    try {   pDSGetBufferID   = reinterpret_cast<GenTL::PDSGetBufferID   >(_libTL.getSymbol("DSGetBufferID") ); } catch (Poco::NotFoundException&) {    pDSGetBufferID   = NULL; }
    try {   pDSClose     = reinterpret_cast<GenTL::PDSClose >(_libTL.getSymbol("DSClose") ); } catch (Poco::NotFoundException&) {    pDSClose     = NULL; }
    try {   pDSRevokeBuffer  = reinterpret_cast<GenTL::PDSRevokeBuffer  >(_libTL.getSymbol("DSRevokeBuffer") ); } catch (Poco::NotFoundException&) {    pDSRevokeBuffer  = NULL; }
    try {   pDSQueueBuffer   = reinterpret_cast<GenTL::PDSQueueBuffer   >(_libTL.getSymbol("DSQueueBuffer") ); } catch (Poco::NotFoundException&) {    pDSQueueBuffer   = NULL; }
    try {   pDSGetBufferInfo     = reinterpret_cast<GenTL::PDSGetBufferInfo >(_libTL.getSymbol("DSGetBufferInfo") ); } catch (Poco::NotFoundException&) {    pDSGetBufferInfo     = NULL; }
    /* GenTL v1.1 */
    try {   pGCGetNumPortURLs    = reinterpret_cast<GenTL::PGCGetNumPortURLs    >(_libTL.getSymbol("GCGetNumPortURLs") ); } catch (Poco::NotFoundException&) {    pGCGetNumPortURLs    = NULL; }
    try {   pGCGetPortURLInfo    = reinterpret_cast<GenTL::PGCGetPortURLInfo    >(_libTL.getSymbol("GCGetPortURLInfo") ); } catch (Poco::NotFoundException&) {    pGCGetPortURLInfo    = NULL; }
    try {   pGCReadPortStacked   = reinterpret_cast<GenTL::PGCReadPortStacked   >(_libTL.getSymbol("GCReadPortStacked") ); } catch (Poco::NotFoundException&) {    pGCReadPortStacked   = NULL; }
    try {   pGCWritePortStacked  = reinterpret_cast<GenTL::PGCWritePortStacked  >(_libTL.getSymbol("GCWritePortStacked") ); } catch (Poco::NotFoundException&) {    pGCWritePortStacked  = NULL; }
    /* GenTL v1.3 */
    try {   pDSGetBufferChunkData    = reinterpret_cast<GenTL::PDSGetBufferChunkData    >(_libTL.getSymbol("DSGetBufferChunkData") ); } catch (Poco::NotFoundException&) {    pDSGetBufferChunkData    = NULL; }
    /* GenTL v1.4 */
    try {   pIFGetParentTL   = reinterpret_cast<GenTL::PIFGetParentTL   >(_libTL.getSymbol("IFGetParentTL") ); } catch (Poco::NotFoundException&) {    pIFGetParentTL   = NULL; }
    try {   pDevGetParentIF  = reinterpret_cast<GenTL::PDevGetParentIF  >(_libTL.getSymbol("DevGetParentIF") ); } catch (Poco::NotFoundException&) {    pDevGetParentIF  = NULL; }
    try {   pDSGetParentDev  = reinterpret_cast<GenTL::PDSGetParentDev  >(_libTL.getSymbol("DSGetParentDev") ); } catch (Poco::NotFoundException&) {    pDSGetParentDev  = NULL; }


}

void GenTLLib::genTLPortInfo(GenTL::PORT_HANDLE hPort, Poco::Logger& log)
{
    size_t bufferSize = 1024;
    char *pBuffer = new char[bufferSize];

    genTLPortInfoElem("port ID", hPort,
            GenTL::PORT_INFO_ID,
            pBuffer, bufferSize, log);

    genTLPortInfoElem("port name as referenced in the XML descr", hPort,
            GenTL::PORT_INFO_PORTNAME,
            pBuffer, bufferSize, log);

    genTLPortInfoElem("port vendor", hPort,
            GenTL::PORT_INFO_VENDOR,
            pBuffer, bufferSize, log);

    genTLPortInfoElem("port model", hPort,
            GenTL::PORT_INFO_MODEL,
            pBuffer, bufferSize, log);

    genTLPortInfoElem("port TL technology", hPort,
            GenTL::PORT_INFO_TLTYPE,
            pBuffer, bufferSize, log);

    genTLPortInfoElem("port TL version", hPort,
            GenTL::PORT_INFO_VERSION,
            pBuffer, bufferSize, log);

    genTLPortInfoElem("port parent module", hPort,
            GenTL::PORT_INFO_MODULE,
            pBuffer, bufferSize, log);

    delete[] pBuffer;

    genTLPortFlagInfoElem("port not available", hPort,
            GenTL::PORT_INFO_ACCESS_NA, log);

    genTLPortFlagInfoElem("port readable", hPort,
            GenTL::PORT_INFO_ACCESS_READ, log);

    genTLPortFlagInfoElem("port writeable", hPort,
            GenTL::PORT_INFO_ACCESS_WRITE, log);

    genTLPortFlagInfoElem("port not implemented", hPort,
            GenTL::PORT_INFO_ACCESS_NI, log);

    genTLPortFlagInfoElem("port big endian", hPort,
            GenTL::PORT_INFO_BIG_ENDIAN, log);

    genTLPortFlagInfoElem("port little endian", hPort,
            GenTL::PORT_INFO_LITTLE_ENDIAN, log);

    genTLPortExInfo(hPort, log);
}

void GenTLLib::genTLPortInfoElem(std::string title,
        GenTL::PORT_HANDLE hPort,
        GenTL::PORT_INFO_CMD infoCmd, char* pBuffer,
        size_t bufferSize, Poco::Logger& log)
{
    GenTL::INFO_DATATYPE dataType;

    try
    {
        GCGetPortInfo( hPort, infoCmd, &dataType, pBuffer, &bufferSize);

        if (dataType != GenTL::INFO_DATATYPE_STRING)
            throw GenTLException("genTLExInfo", "type error: should be string");

        poco_information(log,title + ": " + std::string(pBuffer, bufferSize));
    }
    catch (GenTLException& e)
    {
        poco_warning(log, "Could not retrieve " + title
                + ": " + e.displayText());
    }
}

void GenTLLib::genTLPortFlagInfoElem(std::string title,
        GenTL::PORT_HANDLE hPort,
        GenTL::PORT_INFO_CMD infoCmd, Poco::Logger& log)
{
    GenTL::INFO_DATATYPE dataType;
     bool8_t flag;
     size_t flagSize = sizeof(flag);

     try
     {
         GCGetPortInfo( hPort, infoCmd, &dataType, &flag, &flagSize);

         if (dataType != GenTL::INFO_DATATYPE_BOOL8)
             throw GenTLException("genTLExInfo", "type error: should be bool");

         poco_information(log,title + (flag?": ON":": OFF") );
     }
     catch (GenTLException& e)
     {
         poco_warning(log, "Could not retrieve " + title
                 + ": " + e.displayText());
     }
}

void GenTLLib::genTLPortExInfo(GenTL::PORT_HANDLE hPort, Poco::Logger& log)
{
    uint32_t numURLs;
    try
    {
        GCGetNumPortURLs(hPort, &numURLs);
        poco_information(log,"number of URLs on that port: "
            + Poco::NumberFormatter::format(numURLs));
    }
    catch (GenTLException& e)
    {
        poco_warning(log,
                "Cannot retrieve the number of URLs on that port: "
                + e.displayText());
        return;
    }

    for (uint32_t iPort=0; iPort < numURLs; iPort++)
    {
		size_t bufferSize = 1024; 
        char* pBuffer = new char[bufferSize]; 

        try
        {
            GenTL::INFO_DATATYPE dataType;

            GCGetPortURLInfo( hPort, iPort,
                GenTL::URL_INFO_URL,
                &dataType,
                pBuffer,
                &bufferSize ) ;

            if (dataType != GenTL::INFO_DATATYPE_STRING)
                throw GenTLException("genTLExInfo", "type error: should be string");
        }
        catch (GenTLException& e)
        {
            poco_warning(log, "Cannot retrieve port#"
                    + Poco::NumberFormatter::format(iPort)
                    + " URL info: " + e.displayText());
            return;
        }

		std::string sURL(pBuffer, bufferSize);
		delete[] pBuffer;

        poco_information(log,
            "URL#" + Poco::NumberFormatter::format(iPort) + ": " + sURL );

#		ifdef HAVE_GENAPI

		// check if it is a module register address
		Poco::RegularExpression modRegisterRegex("^[Ll]ocal:.*");
		Poco::RegularExpression fileRegex("^[Ff]ile:.*");
		Poco::RegularExpression webRegex("^http:.*");

		if (webRegex.match(sURL))
		{
			poco_information(log, "The given port URL is at the vendor website's. "
								  "Retrieving not supported. ");
			continue;
		}
		else if (fileRegex.match(sURL))
		{
			poco_information(log, "The given port URL is a local file. Not supported. ");
			continue;
		}
		else if (!modRegisterRegex.match(sURL))
		{
			poco_warning(log, "not recognized port URL.");
			continue;
		}
		
		// module register map
		// zipped? address and length?
		Poco::RegularExpression parseRegex("^[Ll]ocal:.*\\.([Xx][Mm][Ll]|[Zz][Ii][Pp]);([0-9a-fA-F]*);([0-9a-fA-F]*).*");
		Poco::RegularExpression::MatchVec posVec;
		parseRegex.match(sURL, 0, posVec); 	// [0]: full string, [1]: extension, [2]: address, [3]: length

		if (posVec.size() != 4)
		{
			poco_warning(log, "Unable to parse the port URL as module register map...");
			continue;
		}

		std::string extension(sURL, posVec[1].offset, posVec[1].length);
		bool zipped = false;

		if (Poco::icompare(extension,"zip") == 0)
		{
			zipped = true;
		}
		else if (Poco::icompare(extension,"xml"))
		{
			poco_warning(log, "module map register unrecognized extension: " + extension + "... not supported");
			continue;
		}

		std::string sAddress(sURL, posVec[2].offset, posVec[2].length);
		size_t address = Poco::NumberParser::parseHex64(sAddress);

		std::string sLength(sURL, posVec[3].offset, posVec[3].length);
		size_t length = Poco::NumberParser::parseHex64(sLength);

		pBuffer = new char[length];
		// retrieve the xml string using a GCReadPort
		try
		{
			GCReadPort(hPort, address, pBuffer, &length);
		}
        catch (GenTLException& e)
        {
            poco_warning(log, "Cannot retrieve XML from register map: " 
				+ e.displayText());
            continue;
        }

		// poco_information(log, std::string(pBuffer, length));

		GenICam::gcstring xmlString(pBuffer, length);

		// create a CNodeMapRef and load the xml. 
		GenApi::CNodeMapRef nodeMap;

		try
		{
			if (!zipped)
				nodeMap._LoadXMLFromString(xmlString);
			else
				nodeMap._LoadXMLFromZIPData(xmlString, length);

			poco_information(log, "XML file loaded...");
		}
		catch (GenICam::GenericException& e)
		{
			poco_error(log, e.GetDescription());
			continue;
		}

		try
		{
			GenApi::NodeList_t nodeList;
			nodeMap._GetNodes(nodeList);
			poco_information(log, Poco::NumberFormatter::format(nodeList.size()) + " nodes loaded.");
		}
		catch (GenICam::GenericException& e)
		{
			poco_error(log, e.GetDescription());
			continue;
		}

		delete[] pBuffer;
#		endif
    }
}

#ifdef HAVE_GENAPI
void GenTLLib::retrieveNodeMap(GenTL::PORT_HANDLE hPort, GenApi::CNodeMapRef& nodeMap, Poco::Logger& log)
{
	uint32_t numURLs;

    GCGetNumPortURLs(hPort, &numURLs);
    poco_information(log,"number of URLs on that port: "
        + Poco::NumberFormatter::format(numURLs));

    for (uint32_t iPort=0; iPort < numURLs; iPort++)
    {
		size_t bufferSize = 1024; 
        char* pBuffer = new char[bufferSize]; 

        GenTL::INFO_DATATYPE dataType;

        GCGetPortURLInfo( hPort, iPort,
            GenTL::URL_INFO_URL,
            &dataType,
            pBuffer,
            &bufferSize ) ;

        if (dataType != GenTL::INFO_DATATYPE_STRING)
            throw GenTLException("genTLExInfo", "type error: should be string");

		std::string sURL(pBuffer, bufferSize);
		delete[] pBuffer;

        poco_information(log,
            "URL#" + Poco::NumberFormatter::format(iPort) + ": " + sURL );

		// check if it is a module register address
		Poco::RegularExpression modRegisterRegex("^[Ll]ocal:.*");
		Poco::RegularExpression fileRegex("^[Ff]ile:.*");
		Poco::RegularExpression webRegex("^http:.*");

		if (webRegex.match(sURL))
		{
			poco_information(log, "The given port URL is at the vendor website's. "
								  "Retrieving not supported. ");
			continue;
		}
		else if (fileRegex.match(sURL))
		{
			poco_information(log, "The given port URL is a local file. Not supported. ");
			continue;
		}
		else if (!modRegisterRegex.match(sURL))
		{
			poco_warning(log, "not recognized port URL.");
			continue;
		}
		
		// module register map
		// zipped? address and length?
		Poco::RegularExpression parseRegex("^[Ll]ocal:.*\\.([Xx][Mm][Ll]|[Zz][Ii][Pp]);([0-9a-fA-F]*);([0-9a-fA-F]*).*");
		Poco::RegularExpression::MatchVec posVec;
		parseRegex.match(sURL, 0, posVec); 	// [0]: full string, [1]: extension, [2]: address, [3]: length

		if (posVec.size() != 4)
		{
			poco_warning(log, "Unable to parse the port URL as module register map...");
			continue;
		}

		std::string extension(sURL, posVec[1].offset, posVec[1].length);
		bool zipped = false;

		if (Poco::icompare(extension,"zip") == 0)
			zipped = true;

		std::string sAddress(sURL, posVec[2].offset, posVec[2].length);
		size_t address = Poco::NumberParser::parseHex64(sAddress);

		std::string sLength(sURL, posVec[3].offset, posVec[3].length);
		size_t length = Poco::NumberParser::parseHex64(sLength);

		pBuffer = new char[length];
		// retrieve the xml string using a GCReadPort
		try
		{
			GCReadPort(hPort, address, pBuffer, &length);
		}
        catch (GenTLException& e)
        {
            poco_warning(log, "Cannot retrieve XML from register map: " 
				+ e.displayText());
            continue;
        }

		// poco_information(log, std::string(pBuffer, length));

		GenICam::gcstring xmlString(pBuffer, length);

		try
		{
			if (!zipped)
				nodeMap._LoadXMLFromString(xmlString);
			else
				nodeMap._LoadXMLFromZIPData(xmlString, length);

			poco_information(log, "XML file loaded...");
			return;
		}
		catch (GenICam::GenericException& e)
		{
			throw GenTLException("loadXML", e.GetDescription());
		}

		delete[] pBuffer;
    }

	throw GenTLException("RetrieveNodeMap", "Unable to find a supported port URL. ");
}
#endif /* HAVE_GENAPI */