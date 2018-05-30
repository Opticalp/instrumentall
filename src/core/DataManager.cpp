/**
 * @file	src/core/DataManager.cpp
 * @date	Feb. 2016
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

#include "DataManager.h"
#include "ThreadManager.h"

#include "OutPort.h"
#include "DataItem.h"
#include "Dispatcher.h"

// loggers
#include "dataLoggers/DataPocoLogger.h"
#ifdef HAVE_OPENCV
#    include "dataLoggers/ShowImageLogger.h"
#    include "dataLoggers/SaveImageLogger.h"
#endif

// proxies
#include "dataProxies/DataBuffer.h"
#include "dataProxies/SimpleNumConverter.h"
#include "dataProxies/Delayer.h"
#ifdef HAVE_OPENCV
#    include "dataProxies/ImageReticle.h"
#endif

#include "Poco/Exception.h"
#include "Poco/Util/Application.h"

enum
{
	contScalar = TypeNeutralData::contScalar,
	contVector = TypeNeutralData::contVector,
	typeInt32 = TypeNeutralData::typeInt32,
	typeUInt32 = TypeNeutralData::typeUInt32,
	typeInt64 = TypeNeutralData::typeInt64,
	typeUInt64 = TypeNeutralData::typeUInt64,
	typeFloat = TypeNeutralData::typeFloat,
	typeDblFloat = TypeNeutralData::typeDblFloat,
	typeString = TypeNeutralData::typeString
};

DataManager::DataManager():
    VerboseEntity(name())
{
    // Register data loggers in the factory using the C++ class name
    loggerFactory.registerClass<DataPocoLogger>("DataPocoLogger");
    loggerClasses.insert(classPair("DataPocoLogger", DataPocoLogger::classDescription()));

#ifdef HAVE_OPENCV
    loggerFactory.registerClass<ShowImageLogger>("ShowImageLogger");
    loggerClasses.insert(classPair("ShowImageLogger", ShowImageLogger::classDescription()));
    loggerFactory.registerClass<SaveImageLogger>("SaveImageLogger");
    loggerClasses.insert(classPair("SaveImageLogger", SaveImageLogger::classDescription()));
#endif

    // Register data proxies in the factory using their C++ class name
    proxyFactory.registerClass<DataBuffer>("DataBuffer");
    proxyClasses.insert(classPair("DataBuffer", DataBuffer::classDescription()));

    proxyFactory.registerClass<SimpleNumConverter>("SimpleNumConverter");
    proxyClasses.insert(classPair("SimpleNumConverter", SimpleNumConverter::classDescription()));

    proxyFactory.registerClass<Delayer>("Delayer");
    proxyClasses.insert(classPair("Delayer", Delayer::classDescription()));

#ifdef HAVE_OPENCV
    proxyFactory.registerClass<ImageReticle>("ImageReticle");
    proxyClasses.insert(classPair("ImageReticle", ImageReticle::classDescription()));
#endif
}

DataManager::~DataManager()
{
    // uninitialize(); // should have been already called by the system.
}

void DataManager::initialize(Poco::Util::Application& app)
{
    setLogger(name());

    // TODO: init loggers and proxies from config file?
}

void DataManager::uninitialize()
{
    poco_information(logger(),"Data manager uninitializing");
}

AutoPtr<DataLogger> DataManager::newDataLogger(std::string className)
{
	// create and take ownership
	AutoPtr<DataLogger> ret(loggerFactory.createInstance(className));
    return ret;
}

AutoPtr<DataProxy> DataManager::newDataProxy(std::string className)
{
	// create and take ownership
    AutoPtr<DataProxy> ret(proxyFactory.createInstance(className));
    return ret;
}
