/**
 * @file	src/modules/devices/genicam/GenicamIfaceFactory.cpp
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

#include "GenicamIfaceFactory.h"
#include "GenicamDeviceFactory.h"

#include "Poco/NumberFormatter.h"

GenicamIfaceFactory::GenicamIfaceFactory(GenTLLib* genTL,
        ModuleFactory* parent, std::string selector):
        ModuleFactoryBranch(parent, selector, false),
        _TLhInterface(GENTL_INVALID_HANDLE), mGenTL(genTL)
{
    setLogger(name());

    try
    {
        mGenTL->TLOpenInterface( mGenTL->TLhSystem,
                selector.c_str(),
                &_TLhInterface );
        poco_information(logger(), "interface: " + selector + " opened...");
    }
    catch (GenTLException& e)
    {
        throw Poco::RuntimeException("Cannot open the interface "
                + selector + ": " + e.displayText() );
    }

    if (_TLhInterface == GENTL_INVALID_HANDLE)
        throw Poco::RuntimeException("Cannot create the interface factory");
    else
        mGenTL->genTLPortInfo(_TLhInterface, logger());

    discover();
}

void GenicamIfaceFactory::terminate()
{
     if (_TLhInterface != GENTL_INVALID_HANDLE)
     {
         try
         {
             mGenTL->IFClose(_TLhInterface);
         }
         catch (GenTLException& e) // could be not opened
         {
             poco_warning(logger(), "Closing GenTL interface" +
                         getSelector() + " failed: " + e.displayText());
         }
     }
}

std::vector<std::string> GenicamIfaceFactory::selectValueList()
{
    return devices;
}

ModuleFactoryBranch* GenicamIfaceFactory::newChildFactory(std::string selector)
{
    return new GenicamDeviceFactory(mGenTL, _TLhInterface, this, selector);
}

void GenicamIfaceFactory::discover()
{
    try
    {
    mGenTL->IFUpdateDeviceList(_TLhInterface, NULL,
            2000); // 2000 == timeout 2s
    }
    catch (GenTLException& e)
    {
        poco_warning(logger(),"Cannot update device list: " + e.displayText());
        return;
    }

    uint32_t numDevices;
    try
    {
        mGenTL->IFGetNumDevices(_TLhInterface, &numDevices);
    }
    catch (GenTLException& e)
    {
        poco_warning(logger(),"Cannot retrieve the number of devices: "
                + e.displayText());
        return;
    }

    poco_information(logger(),
        Poco::NumberFormatter::format(numDevices) + " device(s) available");

    for (uint32_t device=0 ; device < numDevices ; device++)
    {
        char *pDeviceID;

        // get device ID
        size_t size;
        try
        {
            mGenTL->IFGetDeviceID(_TLhInterface, device, NULL, &size);
        }
        catch (GenTLException&)
        {
            poco_error(logger(), "Cannot retrieve the device#"
                    + Poco::NumberFormatter::format(device) + " device ID" );
        }
        pDeviceID = new char[size];

        try
        {
            mGenTL->IFGetDeviceID(_TLhInterface, device, pDeviceID, &size);

            genTLDeviceExInfo(pDeviceID);

            poco_information(logger(), "Device found: "
                    + std::string(pDeviceID));

            devices.push_back(std::string(pDeviceID));

        }
        catch (GenTLException& e)
        {
            poco_warning( logger(),
                std::string("deviceDiscover: ")
                + e.displayText() );
        }

        delete[] pDeviceID;
    }
}

void GenicamIfaceFactory::genTLDeviceExInfo(char* pDeviceID)
{
    size_t bufferSize = 1024;
    char *pBuffer = new char[bufferSize];

    genTLDeviceExInfoElem("Device ID", pDeviceID,
        GenTL::DEVICE_INFO_ID,
        pBuffer,bufferSize);

    genTLDeviceExInfoElem("Device vendor", pDeviceID,
        GenTL::DEVICE_INFO_VENDOR,
        pBuffer,bufferSize);

    genTLDeviceExInfoElem("Device model", pDeviceID,
        GenTL::DEVICE_INFO_MODEL,
        pBuffer,bufferSize);

    genTLDeviceExInfoElem("Device supported GenTL technology", pDeviceID,
        GenTL::DEVICE_INFO_TLTYPE,
        pBuffer,bufferSize);

    genTLDeviceExInfoElem("Device name", pDeviceID,
        GenTL::DEVICE_INFO_DISPLAYNAME,
        pBuffer,bufferSize);

    genTLDeviceExInfoElem("Device user defined name", pDeviceID,
        GenTL::DEVICE_INFO_USER_DEFINED_NAME,
        pBuffer,bufferSize);

    genTLDeviceExInfoElem("Device serial number", pDeviceID,
        GenTL::DEVICE_INFO_SERIAL_NUMBER,
        pBuffer,bufferSize);

    genTLDeviceExInfoElem("Device version", pDeviceID,
        GenTL::DEVICE_INFO_VERSION,
        pBuffer,bufferSize);

    delete[] pBuffer;
}

void GenicamIfaceFactory::genTLDeviceExInfoElem(std::string title,
        char* pDeviceID, GenTL::DEVICE_INFO_CMD infoCmd,
        char* pBuffer, size_t bufferSize)
{
    GenTL::INFO_DATATYPE dataType;

    try
    {
        //  get info elem (DEVICE_CMD_INFO)
        mGenTL->IFGetDeviceInfo(
                _TLhInterface, pDeviceID,
                infoCmd, &dataType,
                pBuffer, &bufferSize ) ;

        if (dataType != GenTL::INFO_DATATYPE_STRING)
            throw GenTLException("genTLDeviceExInfo", "type error");

        poco_information(logger(),title + ": " + std::string(pBuffer));
    }
    catch (GenTLException& e)
    {
        poco_warning(logger(), "Could not retrieve " + title
                + ": " + e.displayText());
    }
}
