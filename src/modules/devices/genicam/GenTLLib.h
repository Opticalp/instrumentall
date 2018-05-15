/**
 * @file	src/modules/devices/genicam/GenTLLib.h
 * @date	jan. 2016
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2016 Ph. Renaud-Goud / Opticalp

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

#ifndef SRC_GENICAM_GENTLLIB_H_
#define SRC_GENICAM_GENTLLIB_H_

#include "GenTL_v1_5.h"
#ifdef HAVE_GENAPI
#    include "GenICam.h"
#endif


#include "Poco/SharedLibrary.h"
#include "Poco/Path.h"
#include "Poco/Logger.h"

#ifdef VERBOSE
#include "VerboseEntity.h"
#endif

POCO_DECLARE_EXCEPTION( , GenTLException, Poco::Exception)

/**
 * GenTLLib
 *
 * Give access to a genTL .cti library.
 * Only load the shared library. Do not init the Transport layer itself.
 */
#ifdef VERBOSE
class GenTLLib: public VerboseEntity
#else
class GenTLLib
#endif
{
public:
    GenTLLib(Poco::Path libTLPath);
    virtual ~GenTLLib();

    /// @name GenTL functions forwarders
    ///@{
    void  GCGetInfo(GenTL::TL_INFO_CMD iInfoCmd, GenTL::INFO_DATATYPE *piType, void *pBuffer, size_t *piSize )
    {
        if (pGCGetInfo)
            genTLCheckError(pGCGetInfo(iInfoCmd, piType, pBuffer, piSize));
        else
            throw GenTLException("GCGetInfo not implemented");
    }

    void  GCGetLastError(GenTL::GC_ERROR *piErrorCode, char *sErrText, size_t *piSize )
    {
        if (pGCGetLastError)
            genTLCheckError(pGCGetLastError(piErrorCode, sErrText, piSize));
        else
            throw GenTLException("GCGetLastError not implemented ");
    }

    void  GCInitLib()
    {
        if (pGCInitLib)
            genTLCheckError(pGCInitLib());
        else
            throw GenTLException("GCInitLib not implemented ");
    }
    void  GCCloseLib()
    {
        if (pGCCloseLib)
            genTLCheckError(pGCCloseLib());
        else
            throw GenTLException("GCCloseLib not implemented ");
    }

    void  GCReadPort( GenTL::PORT_HANDLE hPort, uint64_t iAddress, void *pBuffer, size_t *piSize )
    {
        if (pGCReadPort)
            genTLCheckError(pGCReadPort(hPort, iAddress, pBuffer, piSize));
        else
            throw GenTLException("GCReadPort not implemented ");
    }

    // FIXME: indentation
    void  GCWritePort( GenTL::PORT_HANDLE hPort, uint64_t iAddress, const void *pBuffer, size_t *piSize )   { if (  pGCWritePort    ) { genTLCheckError(    pGCWritePort             ( hPort, iAddress, pBuffer, piSize )   ); } else {throw GenTLException("  GCWritePort  not implemented "); } }
    void  GCGetPortURL( GenTL::PORT_HANDLE hPort, char *sURL, size_t *piSize )   { if (  pGCGetPortURL   ) { genTLCheckError(    pGCGetPortURL            ( hPort, sURL, piSize )    ); } else {throw GenTLException("  GCGetPortURL     not implemented "); } }

    void  GCGetPortInfo( GenTL::PORT_HANDLE hPort, GenTL::PORT_INFO_CMD iInfoCmd, GenTL::INFO_DATATYPE *piType, void *pBuffer, size_t *piSize ) { if (  pGCGetPortInfo  ) { genTLCheckError(    pGCGetPortInfo           ( hPort, iInfoCmd, piType, pBuffer, piSize )   ); } else {throw GenTLException("  GCGetPortInfo    not implemented "); } }

    void  GCRegisterEvent( GenTL::EVENTSRC_HANDLE hEventSrc, GenTL::EVENT_TYPE iEventID, GenTL::EVENT_HANDLE *phEvent )   { if (  pGCRegisterEvent    ) { genTLCheckError(    pGCRegisterEvent         ( hEventSrc, iEventID, phEvent )   ); } else {throw GenTLException("  GCRegisterEvent  not implemented "); } }
    void  GCUnregisterEvent( GenTL::EVENTSRC_HANDLE hEventSrc, GenTL::EVENT_TYPE iEventID )  { if (  pGCUnregisterEvent  ) { genTLCheckError(    pGCUnregisterEvent       ( hEventSrc, iEventID )    ); } else {throw GenTLException("  GCUnregisterEvent    not implemented "); } }

    void  EventGetData( GenTL::EVENT_HANDLE hEvent, void *pBuffer, size_t *piSize, uint64_t iTimeout )   { if (  pEventGetData   ) { genTLCheckError(    pEventGetData            ( hEvent, pBuffer, piSize, iTimeout )  ); } else {throw GenTLException("  EventGetData     not implemented "); } }
    void  EventGetDataInfo( GenTL::EVENT_HANDLE hEvent, const void *pInBuffer, size_t iInSize, GenTL::EVENT_DATA_INFO_CMD iInfoCmd, GenTL::INFO_DATATYPE *piType, void *pOutBuffer, size_t *piOutSize )    { if (  pEventGetDataInfo   ) { genTLCheckError(    pEventGetDataInfo        ( hEvent, pInBuffer, iInSize, iInfoCmd, piType, pOutBuffer, piOutSize )    ); } else {throw GenTLException("  EventGetDataInfo     not implemented "); } }
    void  EventGetInfo( GenTL::EVENT_HANDLE hEvent, GenTL::EVENT_INFO_CMD iInfoCmd, GenTL::INFO_DATATYPE *piType, void *pBuffer, size_t *piSize )  { if (  pEventGetInfo   ) { genTLCheckError(    pEventGetInfo            ( hEvent, iInfoCmd, piType, pBuffer, piSize )  ); } else {throw GenTLException("  EventGetInfo     not implemented "); } }
    void  EventFlush( GenTL::EVENT_HANDLE hEvent ) { if (  pEventFlush ) { genTLCheckError(    pEventFlush              ( hEvent ) ); } else {throw GenTLException("  EventFlush   not implemented "); } }
    void  EventKill( GenTL::EVENT_HANDLE hEvent ) { if (  pEventKill  ) { genTLCheckError(    pEventKill               ( hEvent ) ); } else {throw GenTLException("  EventKill    not implemented "); } }

    void  TLOpen( GenTL::TL_HANDLE *phTL ) { if (  pTLOpen ) { genTLCheckError(    pTLOpen                  ( phTL )   ); } else {throw GenTLException("  TLOpen   not implemented "); } }
    void  TLClose( GenTL::TL_HANDLE hTL )   { if (  pTLClose    ) { genTLCheckError(    pTLClose                 ( hTL )    ); } else {throw GenTLException("  TLClose  not implemented "); } }
    void  TLGetInfo( GenTL::TL_HANDLE hTL, GenTL::TL_INFO_CMD iInfoCmd, GenTL::INFO_DATATYPE *piType, void *pBuffer, size_t *piSize )   { if (  pTLGetInfo  ) { genTLCheckError(    pTLGetInfo               ( hTL, iInfoCmd, piType, pBuffer, piSize ) ); } else {throw GenTLException("  TLGetInfo    not implemented "); } }

    void  TLGetNumInterfaces( GenTL::TL_HANDLE hTL, uint32_t *piNumIfaces )    { if (  pTLGetNumInterfaces ) { genTLCheckError(    pTLGetNumInterfaces      ( hTL, piNumIfaces )   ); } else {throw GenTLException("  TLGetNumInterfaces   not implemented "); } }
    void  TLGetInterfaceID( GenTL::TL_HANDLE hTL, uint32_t iIndex,  char *sID, size_t *piSize )  { if (  pTLGetInterfaceID   ) { genTLCheckError(    pTLGetInterfaceID        ( hTL, iIndex,  sID, piSize )  ); } else {throw GenTLException("  TLGetInterfaceID     not implemented "); } }
    void  TLGetInterfaceInfo( GenTL::TL_HANDLE hTL, const char *sIfaceID, GenTL::INTERFACE_INFO_CMD iInfoCmd, GenTL::INFO_DATATYPE *piType, void *pBuffer, size_t *piSize )  { if (  pTLGetInterfaceInfo ) { genTLCheckError(    pTLGetInterfaceInfo      ( hTL, sIfaceID, iInfoCmd, piType, pBuffer, piSize )   ); } else {throw GenTLException("  TLGetInterfaceInfo   not implemented "); } }
    void  TLOpenInterface( GenTL::TL_HANDLE hTL, const char *sIfaceID, GenTL::IF_HANDLE *phIface ) { if (  pTLOpenInterface    ) { genTLCheckError(    pTLOpenInterface         ( hTL, sIfaceID, phIface ) ); } else {throw GenTLException("  TLOpenInterface  not implemented "); } }
    void  TLUpdateInterfaceList( GenTL::TL_HANDLE hTL, bool8_t *pbChanged, uint64_t iTimeout )    { if (  pTLUpdateInterfaceList  ) { genTLCheckError(    pTLUpdateInterfaceList   ( hTL, pbChanged, iTimeout )   ); } else {throw GenTLException("  TLUpdateInterfaceList    not implemented "); } }

    void  IFClose( GenTL::IF_HANDLE hIface )    { if (  pIFClose    ) { genTLCheckError(    pIFClose                 ( hIface ) ); } else {throw GenTLException("  IFClose  not implemented "); } }
    void  IFGetInfo( GenTL::IF_HANDLE hIface, GenTL::INTERFACE_INFO_CMD iInfoCmd, GenTL::INFO_DATATYPE *piType, void *pBuffer, size_t *piSize ) { if (  pIFGetInfo  ) { genTLCheckError(    pIFGetInfo               ( hIface, iInfoCmd, piType, pBuffer, piSize )  ); } else {throw GenTLException("  IFGetInfo    not implemented "); } }

    void  IFGetNumDevices( GenTL::IF_HANDLE hIface, uint32_t *piNumDevices )    { if (  pIFGetNumDevices    ) { genTLCheckError(    pIFGetNumDevices         ( hIface, piNumDevices )   ); } else {throw GenTLException("  IFGetNumDevices  not implemented "); } }
    void  IFGetDeviceID( GenTL::IF_HANDLE hIface, uint32_t iIndex, char *sIDeviceID, size_t *piSize ) { if (  pIFGetDeviceID  ) { genTLCheckError(    pIFGetDeviceID           ( hIface, iIndex, sIDeviceID, piSize ) ); } else {throw GenTLException("  IFGetDeviceID    not implemented "); } }
    void  IFUpdateDeviceList( GenTL::IF_HANDLE hIface, bool8_t *pbChanged, uint64_t iTimeout ) { if (  pIFUpdateDeviceList ) { genTLCheckError(    pIFUpdateDeviceList      ( hIface, pbChanged, iTimeout )    ); } else {throw GenTLException("  IFUpdateDeviceList   not implemented "); } }
    void  IFGetDeviceInfo( GenTL::IF_HANDLE hIface, const char *sDeviceID, GenTL::DEVICE_INFO_CMD iInfoCmd, GenTL::INFO_DATATYPE *piType, void *pBuffer, size_t *piSize ) { if (  pIFGetDeviceInfo    ) { genTLCheckError(    pIFGetDeviceInfo         ( hIface, sDeviceID, iInfoCmd, piType, pBuffer, piSize )   ); } else {throw GenTLException("  IFGetDeviceInfo  not implemented "); } }
    void  IFOpenDevice( GenTL::IF_HANDLE hIface, const char *sDeviceID, GenTL::DEVICE_ACCESS_FLAGS iOpenFlags, GenTL::DEV_HANDLE *phDevice )   { if (  pIFOpenDevice   ) { genTLCheckError(    pIFOpenDevice            ( hIface, sDeviceID, iOpenFlags, phDevice )    ); } else {throw GenTLException("  IFOpenDevice     not implemented "); } }

    void  DevGetPort( GenTL::DEV_HANDLE hDevice, GenTL::PORT_HANDLE *phRemoteDevice ) { if (  pDevGetPort ) { genTLCheckError(    pDevGetPort              ( hDevice, phRemoteDevice )    ); } else {throw GenTLException("  DevGetPort   not implemented "); } }
    void  DevGetNumDataStreams( GenTL::DEV_HANDLE hDevice, uint32_t *piNumDataStreams )  { if (  pDevGetNumDataStreams   ) { genTLCheckError(    pDevGetNumDataStreams    ( hDevice, piNumDataStreams )  ); } else {throw GenTLException("  DevGetNumDataStreams     not implemented "); } }
    void  DevGetDataStreamID( GenTL::DEV_HANDLE hDevice, uint32_t iIndex, char *sDataStreamID, size_t *piSize )    { if (  pDevGetDataStreamID ) { genTLCheckError(    pDevGetDataStreamID      ( hDevice, iIndex, sDataStreamID, piSize ) ); } else {throw GenTLException("  DevGetDataStreamID   not implemented "); } }
    void  DevOpenDataStream( GenTL::DEV_HANDLE hDevice, const char *sDataStreamID, GenTL::DS_HANDLE *phDataStream )  { if (  pDevOpenDataStream  ) { genTLCheckError(    pDevOpenDataStream       ( hDevice, sDataStreamID, phDataStream )   ); } else {throw GenTLException("  DevOpenDataStream    not implemented "); } }
    void  DevGetInfo( GenTL::DEV_HANDLE hDevice, GenTL::DEVICE_INFO_CMD iInfoCmd, GenTL::INFO_DATATYPE *piType, void *pBuffer, size_t *piSize )  { if (  pDevGetInfo ) { genTLCheckError(    pDevGetInfo              ( hDevice, iInfoCmd, piType, pBuffer, piSize ) ); } else {throw GenTLException("  DevGetInfo   not implemented "); } }
    void  DevClose( GenTL::DEV_HANDLE hDevice )  { if (  pDevClose   ) { genTLCheckError(    pDevClose                ( hDevice )    ); } else {throw GenTLException("  DevClose     not implemented "); } }

    void  DSAnnounceBuffer( GenTL::DS_HANDLE hDataStream, void *pBuffer, size_t iSize, void *pPrivate, GenTL::BUFFER_HANDLE *phBuffer ) { if (  pDSAnnounceBuffer   ) { genTLCheckError(    pDSAnnounceBuffer        ( hDataStream, pBuffer, iSize, pPrivate, phBuffer )    ); } else {throw GenTLException("  DSAnnounceBuffer     not implemented "); } }
    void  DSAllocAndAnnounceBuffer( GenTL::DS_HANDLE hDataStream, size_t iSize, void *pPrivate, GenTL::BUFFER_HANDLE *phBuffer )    { if (  pDSAllocAndAnnounceBuffer   ) { genTLCheckError(    pDSAllocAndAnnounceBuffer( hDataStream, iSize, pPrivate, phBuffer ) ); } else {throw GenTLException("  DSAllocAndAnnounceBuffer     not implemented "); } }
    void  DSFlushQueue( GenTL::DS_HANDLE hDataStream, GenTL::ACQ_QUEUE_TYPE iOperation )    { if (  pDSFlushQueue   ) { genTLCheckError(    pDSFlushQueue            ( hDataStream, iOperation )    ); } else {throw GenTLException("  DSFlushQueue     not implemented "); } }
    void  DSStartAcquisition( GenTL::DS_HANDLE hDataStream, GenTL::ACQ_START_FLAGS iStartFlags, uint64_t iNumToAcquire )  { if (  pDSStartAcquisition ) { genTLCheckError(    pDSStartAcquisition      ( hDataStream, iStartFlags, iNumToAcquire )    ); } else {throw GenTLException("  DSStartAcquisition   not implemented "); } }
    void  DSStopAcquisition( GenTL::DS_HANDLE hDataStream, GenTL::ACQ_STOP_FLAGS iStopFlags )    { if (  pDSStopAcquisition  ) { genTLCheckError(    pDSStopAcquisition       ( hDataStream, iStopFlags )    ); } else {throw GenTLException("  DSStopAcquisition    not implemented "); } }
    void  DSGetInfo( GenTL::DS_HANDLE hDataStream, GenTL::STREAM_INFO_CMD iInfoCmd, GenTL::INFO_DATATYPE *piType, void *pBuffer, size_t *piSize )   { if (  pDSGetInfo  ) { genTLCheckError(    pDSGetInfo               ( hDataStream, iInfoCmd, piType, pBuffer, piSize ) ); } else {throw GenTLException("  DSGetInfo    not implemented "); } }
    void  DSGetBufferID( GenTL::DS_HANDLE hDataStream, uint32_t iIndex, GenTL::BUFFER_HANDLE *phBuffer ) { if (  pDSGetBufferID  ) { genTLCheckError(    pDSGetBufferID           ( hDataStream, iIndex, phBuffer )  ); } else {throw GenTLException("  DSGetBufferID    not implemented "); } }
    void  DSClose( GenTL::DS_HANDLE hDataStream )   { if (  pDSClose    ) { genTLCheckError(    pDSClose                 ( hDataStream )    ); } else {throw GenTLException("  DSClose  not implemented "); } }

    void  DSRevokeBuffer( GenTL::DS_HANDLE hDataStream, GenTL::BUFFER_HANDLE hBuffer, void **pBuffer, void **pPrivate )   { if (  pDSRevokeBuffer ) { genTLCheckError(    pDSRevokeBuffer          ( hDataStream, hBuffer, pBuffer, pPrivate )    ); } else {throw GenTLException("  DSRevokeBuffer   not implemented "); } }
    void  DSQueueBuffer( GenTL::DS_HANDLE hDataStream, GenTL::BUFFER_HANDLE hBuffer )    { if (  pDSQueueBuffer  ) { genTLCheckError(    pDSQueueBuffer           ( hDataStream, hBuffer )   ); } else {throw GenTLException("  DSQueueBuffer    not implemented "); } }
    void  DSGetBufferInfo( GenTL::DS_HANDLE hDataStream, GenTL::BUFFER_HANDLE hBuffer, GenTL::BUFFER_INFO_CMD iInfoCmd, GenTL::INFO_DATATYPE *piType, void *pBuffer, size_t *piSize )    { if (  pDSGetBufferInfo    ) { genTLCheckError(    pDSGetBufferInfo         ( hDataStream, hBuffer, iInfoCmd, piType, pBuffer, piSize )    ); } else {throw GenTLException("  DSGetBufferInfo  not implemented "); } }

    /* GenTL v1.1 */
    void  GCGetNumPortURLs( GenTL::PORT_HANDLE hPort, uint32_t *piNumURLs )  { if (  pGCGetNumPortURLs   ) { genTLCheckError(    pGCGetNumPortURLs        ( hPort, piNumURLs )   ); } else {throw GenTLException("  GCGetNumPortURLs     not implemented "); } }
    void  GCGetPortURLInfo( GenTL::PORT_HANDLE hPort, uint32_t iURLIndex, GenTL::URL_INFO_CMD iInfoCmd, GenTL::INFO_DATATYPE *piType, void *pBuffer, size_t *piSize )  { if (  pGCGetPortURLInfo   ) { genTLCheckError(    pGCGetPortURLInfo        ( hPort, iURLIndex, iInfoCmd, piType, pBuffer, piSize )    ); } else {throw GenTLException("  GCGetPortURLInfo     not implemented "); } }
    void  GCReadPortStacked( GenTL::PORT_HANDLE hPort, GenTL::PORT_REGISTER_STACK_ENTRY *pEntries, size_t *piNumEntries )    { if (  pGCReadPortStacked  ) { genTLCheckError(    pGCReadPortStacked       ( hPort, pEntries, piNumEntries )  ); } else {throw GenTLException("  GCReadPortStacked    not implemented "); } }
    void  GCWritePortStacked( GenTL::PORT_HANDLE hPort, GenTL::PORT_REGISTER_STACK_ENTRY *pEntries, size_t *piNumEntries )    { if (  pGCWritePortStacked ) { genTLCheckError(    pGCWritePortStacked      ( hPort, pEntries, piNumEntries )  ); } else {throw GenTLException("  GCWritePortStacked   not implemented "); } }

    /* GenTL v1.3 */
    void  DSGetBufferChunkData( GenTL::DS_HANDLE hDataStream, GenTL::BUFFER_HANDLE hBuffer, GenTL::SINGLE_CHUNK_DATA *pChunkData, size_t *piNumChunks )    { if (  pDSGetBufferChunkData   ) { genTLCheckError(    pDSGetBufferChunkData    ( hDataStream, hBuffer, pChunkData, piNumChunks )  ); } else {throw GenTLException("  DSGetBufferChunkData     not implemented "); } }

    /* GenTL v1.4 */
    void  IFGetParentTL( GenTL::IF_HANDLE hIface, GenTL::TL_HANDLE *phSystem )   { if (  pIFGetParentTL  ) { genTLCheckError(    pIFGetParentTL           ( hIface, phSystem )   ); } else {throw GenTLException("  IFGetParentTL    not implemented "); } }
    void  DevGetParentIF( GenTL::DEV_HANDLE hDevice, GenTL::IF_HANDLE *phIface )  { if (  pDevGetParentIF ) { genTLCheckError(    pDevGetParentIF          ( hDevice, phIface )   ); } else {throw GenTLException("  DevGetParentIF   not implemented "); } }
    void  DSGetParentDev( GenTL::DS_HANDLE hDataStream, GenTL::DEV_HANDLE *phDevice ) { if (  pDSGetParentDev ) { genTLCheckError(    pDSGetParentDev          ( hDataStream, phDevice )  ); } else {throw GenTLException("  DSGetParentDev   not implemented "); } }
    ///@}

    GenTL::TL_HANDLE TLhSystem; ///< handle on the GenTL system module


    /**
     * Retrieve info about the given port
     *
     * The port can be any genTL handler.
     * See GCGetPortInfo() in GenTL documentation.
     */
    void genTLPortInfo(GenTL::PORT_HANDLE hPort, Poco::Logger& log);

    /**
     * Elementary info for genTLPortInfo
     *
     * The info data type corresponding to the infoCmd is string
     *
     * Should not throw any exception:
     * treated internally and
     * displayed using poco_warning
     */
    void genTLPortInfoElem( std::string title,
        GenTL::PORT_HANDLE hPort,
        GenTL::PORT_INFO_CMD infoCmd,
        char* pBuffer, size_t bufferSize, Poco::Logger& log);

    /**
     * Elementary info for genTLPortInfo
     *
     * The info data type corresponding to the infoCmd is bool8
     *
     * Should not throw any exception:
     * treated internally and
     * displayed using poco_warning
     */
    void genTLPortFlagInfoElem( std::string title,
        GenTL::PORT_HANDLE hPort,
        GenTL::PORT_INFO_CMD infoCmd, Poco::Logger& log);

    /**
     * Retrieve extended infos about the port
     *
     * The port can be any genTL handler.
     * See GCGetPortInfo() in GenTL documentation.
     */
    void genTLPortExInfo(GenTL::PORT_HANDLE hPort, Poco::Logger& log);

#ifdef HAVE_GENAPI
	/**
	 * Retrieve the nodeMap of the given port
	 * 
	 * Supported URL format: zip or xml, local only (module register map)
	 * 
	 * The XML is loaded, but the nodeMap is not connected to the transport layer. 
	 * 
	 * @throw GenTLException on error
	 */
	void retrieveNodeMap(GenTL::PORT_HANDLE hPort, GenApi::CNodeMapRef& nodeMap, Poco::Logger& log);
#endif

private:
    Poco::Path _libTLPath; ///< path of the GenTL library
    Poco::SharedLibrary _libTL; ///< GenTL shared lib object

    /// @name dynamic GenTL function pointers
    ///@{
    GenTL::PGCGetInfo             pGCGetInfo;
    GenTL::PGCGetLastError        pGCGetLastError;
    GenTL::PGCInitLib             pGCInitLib;
    GenTL::PGCCloseLib            pGCCloseLib;
    GenTL::PGCReadPort            pGCReadPort;
    GenTL::PGCWritePort           pGCWritePort;
    GenTL::PGCGetPortURL          pGCGetPortURL;
    GenTL::PGCGetPortInfo         pGCGetPortInfo;

    GenTL::PGCRegisterEvent       pGCRegisterEvent;
    GenTL::PGCUnregisterEvent     pGCUnregisterEvent;
    GenTL::PEventGetData          pEventGetData;
    GenTL::PEventGetDataInfo      pEventGetDataInfo;
    GenTL::PEventGetInfo          pEventGetInfo;
    GenTL::PEventFlush            pEventFlush;
    GenTL::PEventKill             pEventKill;
    GenTL::PTLOpen                pTLOpen;
    GenTL::PTLClose               pTLClose;
    GenTL::PTLGetInfo             pTLGetInfo;
    GenTL::PTLGetNumInterfaces    pTLGetNumInterfaces;
    GenTL::PTLGetInterfaceID      pTLGetInterfaceID;
    GenTL::PTLGetInterfaceInfo    pTLGetInterfaceInfo;
    GenTL::PTLOpenInterface       pTLOpenInterface;
    GenTL::PTLUpdateInterfaceList pTLUpdateInterfaceList;
    GenTL::PIFClose               pIFClose;
    GenTL::PIFGetInfo             pIFGetInfo;
    GenTL::PIFGetNumDevices       pIFGetNumDevices;
    GenTL::PIFGetDeviceID         pIFGetDeviceID;
    GenTL::PIFUpdateDeviceList    pIFUpdateDeviceList;
    GenTL::PIFGetDeviceInfo       pIFGetDeviceInfo;
    GenTL::PIFOpenDevice          pIFOpenDevice;

    GenTL::PDevGetPort            pDevGetPort;
    GenTL::PDevGetNumDataStreams  pDevGetNumDataStreams;
    GenTL::PDevGetDataStreamID    pDevGetDataStreamID;
    GenTL::PDevOpenDataStream     pDevOpenDataStream ;
    GenTL::PDevGetInfo            pDevGetInfo;
    GenTL::PDevClose              pDevClose;

    GenTL::PDSAnnounceBuffer      pDSAnnounceBuffer;
    GenTL::PDSAllocAndAnnounceBuffer pDSAllocAndAnnounceBuffer;
    GenTL::PDSFlushQueue          pDSFlushQueue;
    GenTL::PDSStartAcquisition    pDSStartAcquisition;
    GenTL::PDSStopAcquisition     pDSStopAcquisition;
    GenTL::PDSGetInfo             pDSGetInfo;
    GenTL::PDSGetBufferID         pDSGetBufferID;
    GenTL::PDSClose               pDSClose;
    GenTL::PDSRevokeBuffer        pDSRevokeBuffer;
    GenTL::PDSQueueBuffer         pDSQueueBuffer;
    GenTL::PDSGetBufferInfo       pDSGetBufferInfo;

    /* GenTL v1.1 */
    GenTL::PGCGetNumPortURLs      pGCGetNumPortURLs;
    GenTL::PGCGetPortURLInfo      pGCGetPortURLInfo;
    GenTL::PGCReadPortStacked     pGCReadPortStacked;
    GenTL::PGCWritePortStacked    pGCWritePortStacked;

    /* GenTL v1.3 */
    GenTL::PDSGetBufferChunkData  pDSGetBufferChunkData;

    /* GenTL v1.4 */
    GenTL::PIFGetParentTL         pIFGetParentTL;
    GenTL::PDevGetParentIF        pDevGetParentIF;
    GenTL::PDSGetParentDev        pDSGetParentDev;
    ///@}

    /**
     * Analyse GenTL error codes and translate it into exceptions
     *
     * @throw GenTLException on error
     */
    void genTLCheckError(GenTL::GC_ERROR retValue);

    /**
     * Load genTL lib functions
     *
     * @throw GenTLException on error
     */
    void loadLib();
};

#endif /* SRC_GENICAM_GENTLLIB_H_ */
