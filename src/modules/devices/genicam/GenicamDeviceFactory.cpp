/**
 * @file	src/modules/devices/genicam/GenicamDeviceFactory.cpp
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

#include "GenicamDeviceFactory.h"

#ifdef HAVE_GENAPI
#include "GenicamConfDeviceFactory.h"
#endif

#include "Poco/String.h"
#include "Poco/Path.h"
#include "Poco/File.h"

GenicamDeviceFactory::GenicamDeviceFactory(GenTLLib* genTL,
        GenTL::IF_HANDLE TLhInterface,
        ModuleFactory* parent, std::string selector):
        ModuleFactoryBranch(parent, selector, false),
        mGenTL(genTL), _TLhInterface(TLhInterface),
        _TLhDevice(GENTL_INVALID_HANDLE)
{
    setLogger(name());

    try
    {
        mGenTL->IFOpenDevice(
                _TLhInterface,
                selector.c_str(),
                // GenTL::DEVICE_ACCESS_CONTROL,
                GenTL::DEVICE_ACCESS_EXCLUSIVE,
                &_TLhDevice );
        poco_information(logger(), "device: " + selector + " opened...");
    }
    catch (GenTLException& e)
    {
        throw Poco::RuntimeException("Cannot open the device "
                + selector + ": " + e.displayText() );
    }

    if (_TLhDevice == GENTL_INVALID_HANDLE)
        throw Poco::RuntimeException("Cannot create the device factory");
    else
        mGenTL->genTLPortInfo(_TLhDevice, logger()); // device port info. driver dependant? 
}

void GenicamDeviceFactory::terminate()
{
    if (_TLhDevice != GENTL_INVALID_HANDLE)
    {
        try
        {
            mGenTL->DevClose(_TLhDevice);
        }
        catch (GenTLException& e) // could be not opened
        {
            poco_warning(logger(), "Closing GenTL device" +
                        getSelector() + " failed: " + e.displayText());
        }
    }
}

std::vector<std::string> GenicamDeviceFactory::selectValueList()
{
    std::vector<std::string> list;
    list.push_back("None");
    list.push_back("");

    return list;
}

std::string GenicamDeviceFactory::validateSelector(std::string selector)
{
    if (Poco::icompare(selector, "None") == 0)
        return "None";

    // is selector existing?
    Poco::Path filePath(selector);
    filePath.makeAbsolute();

    if (filePath.isFile()
            && Poco::File(filePath).exists()
            && Poco::File(filePath).canRead() )
    {
        poco_information(logger(),
                filePath.toString()
                + " potential genicam device config file found. " );

        return filePath.toString();
    }
    else
        throw Poco::InvalidArgumentException("Invalid selector: " + selector
                + ". Not corresponding to a file, nor \"None\". ");
}

ModuleFactoryBranch* GenicamDeviceFactory::newChildFactory(std::string selector)
{
#ifdef HAVE_GENAPI
    return new GenicamConfDeviceFactory(mGenTL, _TLhInterface, _TLhDevice, this, selector);
#else
    throw Poco::NotImplementedException("newChildFactory",
            "GenicamConfDeviceFactory not implemented: GenAPI not present");
#endif
}

