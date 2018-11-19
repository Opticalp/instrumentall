/**
 * @file	src/modules/devices/genicam/GenicamLibFactory.cpp
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

#include "GenicamLibFactory.h"
#include "GenicamIfaceFactory.h"

#include "Poco/NumberFormatter.h"

GenicamLibFactory::GenicamLibFactory(ModuleFactory* parent, std::string selector):
    ModuleFactoryBranch(parent, selector, false)
{
    setLogger(name());

    // load the library
    mGenTL = new GenTLLib(selector);

    mGenTL->GCInitLib();

    poco_information(logger(), selector + ": genTL lib loaded");

    // the TL system handler will be retrieved in discovery
    // just in case it fails e.g. due to resource in use
    // then it will be possible to query it once again.
    discover();
}

GenicamLibFactory::~GenicamLibFactory()
{
    if (mGenTL->TLhSystem != GENTL_INVALID_HANDLE)
    {
        try
        {
            mGenTL->TLClose(mGenTL->TLhSystem);
        }
        catch (GenTLException& e)
        {
            poco_error(logger(),"Closing GenTL system handler: " + e.displayText());
        }
    }

    try
    {
        mGenTL->GCCloseLib();
    }
    catch (GenTLException& e)
    {
        poco_error(logger(),"Closing GenTL lib: " + e.displayText());
    }

    delete mGenTL;
}

std::vector<std::string> GenicamLibFactory::selectValueList()
{
    return interfaces;
}

ModuleFactoryBranch* GenicamLibFactory::newChildFactory(std::string selector)
{
    return new GenicamIfaceFactory(mGenTL, this, selector);
}

void GenicamLibFactory::discover()
{
    // load genTL
    if (mGenTL->TLhSystem == GENTL_INVALID_HANDLE)
    {
        try
        {
            mGenTL->TLOpen(&(mGenTL->TLhSystem));
            poco_information(logger(),
                    "GenTL system handler retrieved. ");

#ifdef GENICAM_GET_XML
            mGenTL->genTLPortInfo(_genTLhSystem, logger());
#endif /* GENICAM_GET_XML */

        }
        catch (GenTLException& e)
        {
            poco_error(logger(),"Opening GenTL system handler: " + e.displayText());
            mGenTL->TLhSystem = GENTL_INVALID_HANDLE;
            return;
        }
    }
    else
    {
        poco_information(logger(), "GenTL system handler already set. ");
    }

    // interface discovery
    try
    {
        mGenTL->TLUpdateInterfaceList(mGenTL->TLhSystem,
                NULL,
                10000); // time out (ms)
    }
    catch (GenTLException& e)
    {
        poco_warning(logger(),"Cannot update interface list: " + e.displayText());
        return;
    }

    uint32_t numIfaces;
    try
    {
        mGenTL->TLGetNumInterfaces(mGenTL->TLhSystem, &numIfaces);
    }
    catch (GenTLException& e)
    {
        poco_warning(logger(),"Cannot retrieve the number of interfaces: "
                + e.displayText());
        return;
    }

    poco_information(logger(),
        Poco::NumberFormatter::format(numIfaces) + " interface(s) available");

    interfaces.clear();

    for (uint32_t iface=0 ; iface < numIfaces ; iface++)
    {
        char *pIfaceID;

        // get interface ID
        size_t size;
        try
        {
            mGenTL->TLGetInterfaceID(mGenTL->TLhSystem, iface, NULL, &size);
        }
        catch (GenTLException&)
        {
            poco_error(logger(), "Cannot retrieve the interface#"
                    + Poco::NumberFormatter::format(iface) + " interface ID" );
        }
        pIfaceID = new char[size];
        try
        {
            mGenTL->TLGetInterfaceID(mGenTL->TLhSystem, iface, pIfaceID, &size);

            genTLIfaceExInfo(pIfaceID);

            poco_information(logger(), "Filter driver interface found: "
                    + std::string(pIfaceID) );

            interfaces.push_back(std::string(pIfaceID));
        }
        catch (GenTLException& e)
        {
            poco_warning(logger(),
                std::string("interfaceDiscover: ")
                + e.displayText() );
        }

        delete[] pIfaceID;
    }

}

void GenicamLibFactory::genTLExInfo()
{
    size_t bufferSize = 1024;
    char *pBuffer = new char[bufferSize];

    genTLExInfoElem("Transport layer ID",
        GenTL::TL_INFO_ID,
        pBuffer,bufferSize);

    genTLExInfoElem("Transport layer vendor name",
        GenTL::TL_INFO_VENDOR,
        pBuffer,bufferSize);

    genTLExInfoElem("Transport layer model name",
        GenTL::TL_INFO_MODEL,
        pBuffer,bufferSize);

    genTLExInfoElem("Transport layer version",
        GenTL::TL_INFO_VERSION,
        pBuffer,bufferSize);

    genTLExInfoElem("Transport layer technology that is supported",
        GenTL::TL_INFO_TLTYPE,
        pBuffer,bufferSize);

    genTLExInfoElem("Full path of the library",
        GenTL::TL_INFO_PATHNAME,
        pBuffer,bufferSize);

    genTLExInfoElem("User readable name of the device",
        GenTL::TL_INFO_DISPLAYNAME,
        pBuffer,bufferSize);

    delete[] pBuffer;
}

void GenicamLibFactory::genTLExInfoElem(std::string title,
        GenTL::TL_INFO_CMD infoCmd, char* pBuffer, size_t bufferSize)
{
    GenTL::INFO_DATATYPE infoType;

    try
    {
        mGenTL->GCGetInfo(infoCmd,&infoType,pBuffer,&bufferSize);

        if (infoType != GenTL::INFO_DATATYPE_STRING)
            throw GenTLException("genTLExInfo", title + ": return type error");

        poco_information(logger(),title + ": " + std::string(pBuffer));
    }
    catch (GenTLException& e)
    {
        poco_warning(logger(), "Could not retrieve " + title
                + ": " + e.displayText());
    }
}

void GenicamLibFactory::genTLIfaceExInfo(char* pIfaceID)
{
    size_t bufferSize = 1024;
    char *pBuffer = new char[bufferSize];

    genTLIfaceExInfoElem("Interface ID", pIfaceID,
        GenTL::INTERFACE_INFO_ID,
        pBuffer,bufferSize);

    genTLIfaceExInfoElem("Interface name", pIfaceID,
        GenTL::INTERFACE_INFO_DISPLAYNAME,
        pBuffer,bufferSize);

    genTLIfaceExInfoElem("Supported GenTL technology", pIfaceID,
        GenTL::INTERFACE_INFO_TLTYPE,
        pBuffer,bufferSize);
}

void GenicamLibFactory::genTLIfaceExInfoElem(std::string title, char* pIfaceID,
        GenTL::INTERFACE_INFO_CMD infoCmd, char* pBuffer,
        size_t bufferSize)
{
     GenTL::INFO_DATATYPE dataType;

     try
     {
         //  get info elem (INTERFACE_CMD_INFO)
         mGenTL->TLGetInterfaceInfo(
                 mGenTL->TLhSystem, pIfaceID,
                 infoCmd, &dataType,
                 pBuffer, &bufferSize  ) ;

         if (dataType != GenTL::INFO_DATATYPE_STRING)
             throw GenTLException("genTLIfaceExInfo", "type error");

         poco_information(logger(),title + ": " + std::string(pBuffer));
     }
     catch (GenTLException& e)
     {
         poco_warning(logger(), "Could not retrieve " + title
                 + ": " + e.displayText());
     }
}
