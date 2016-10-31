/**
 * @file	src/DataManager.cpp
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

// proxies
#include "dataProxies/DataBuffer.h"
#include "dataProxies/SimpleNumConverter.h"

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

    // Register data proxies in the factory using the (augmented) C++ class name
    std::string proxyName;

    proxyName = "DataBuffer";
    proxyFactory.registerClass(proxyName + DataItem::dataTypeShortStr(typeInt32),
    		new DataProxyInstantiator<DataBuffer>(typeInt32));
    proxyClasses.insert(classPair(proxyName + DataItem::dataTypeShortStr(typeInt32),
    		DataBuffer::classDescription()));
    proxyFactory.registerClass(proxyName + DataItem::dataTypeShortStr(typeUInt32),
    		new DataProxyInstantiator<DataBuffer>(typeUInt32));
    proxyClasses.insert(classPair(proxyName + DataItem::dataTypeShortStr(typeUInt32),
    		DataBuffer::classDescription()));
    proxyFactory.registerClass(proxyName + DataItem::dataTypeShortStr(typeInt64),
    		new DataProxyInstantiator<DataBuffer>(typeInt64));
    proxyClasses.insert(classPair(proxyName + DataItem::dataTypeShortStr(typeInt64),
    		DataBuffer::classDescription()));
    proxyFactory.registerClass(proxyName + DataItem::dataTypeShortStr(typeUInt64),
    		new DataProxyInstantiator<DataBuffer>(typeUInt64));
    proxyClasses.insert(classPair(proxyName + DataItem::dataTypeShortStr(typeUInt64),
    		DataBuffer::classDescription()));
    proxyFactory.registerClass(proxyName + DataItem::dataTypeShortStr(typeFloat),
    		new DataProxyInstantiator<DataBuffer>(typeFloat));
    proxyClasses.insert(classPair(proxyName + DataItem::dataTypeShortStr(typeFloat),
    		DataBuffer::classDescription()));
    proxyFactory.registerClass(proxyName + DataItem::dataTypeShortStr(typeDblFloat),
    		new DataProxyInstantiator<DataBuffer>(typeDblFloat));
    proxyClasses.insert(classPair(proxyName + DataItem::dataTypeShortStr(typeDblFloat),
    		DataBuffer::classDescription()));
    proxyFactory.registerClass(proxyName + DataItem::dataTypeShortStr(typeString),
    		new DataProxyInstantiator<DataBuffer>(typeString));
    proxyClasses.insert(classPair(proxyName + DataItem::dataTypeShortStr(typeString),
    		DataBuffer::classDescription()));

    proxyFactory.registerClass(proxyName + DataItem::dataTypeShortStr(typeInt32|contVector),
    		new DataProxyInstantiator<DataBuffer>(typeInt32|contVector));
    proxyClasses.insert(classPair(proxyName + DataItem::dataTypeShortStr(typeInt32|contVector),
    		DataBuffer::classDescription()));
    proxyFactory.registerClass(proxyName + DataItem::dataTypeShortStr(typeUInt32|contVector),
    		new DataProxyInstantiator<DataBuffer>(typeUInt32|contVector));
    proxyClasses.insert(classPair(proxyName + DataItem::dataTypeShortStr(typeUInt32|contVector),
    		DataBuffer::classDescription()));
    proxyFactory.registerClass(proxyName + DataItem::dataTypeShortStr(typeInt64|contVector),
    		new DataProxyInstantiator<DataBuffer>(typeInt64|contVector));
    proxyClasses.insert(classPair(proxyName + DataItem::dataTypeShortStr(typeInt64|contVector),
    		DataBuffer::classDescription()));
    proxyFactory.registerClass(proxyName + DataItem::dataTypeShortStr(typeUInt64|contVector),
    		new DataProxyInstantiator<DataBuffer>(typeUInt64|contVector));
    proxyClasses.insert(classPair(proxyName + DataItem::dataTypeShortStr(typeUInt64|contVector),
    		DataBuffer::classDescription()));
    proxyFactory.registerClass(proxyName + DataItem::dataTypeShortStr(typeFloat|contVector),
    		new DataProxyInstantiator<DataBuffer>(typeFloat|contVector));
    proxyClasses.insert(classPair(proxyName + DataItem::dataTypeShortStr(typeFloat|contVector),
    		DataBuffer::classDescription()));
    proxyFactory.registerClass(proxyName + DataItem::dataTypeShortStr(typeDblFloat|contVector),
    		new DataProxyInstantiator<DataBuffer>(typeDblFloat|contVector));
    proxyClasses.insert(classPair(proxyName + DataItem::dataTypeShortStr(typeDblFloat|contVector),
    		DataBuffer::classDescription()));
    proxyFactory.registerClass(proxyName + DataItem::dataTypeShortStr(typeString|contVector),
    		new DataProxyInstantiator<DataBuffer>(typeString|contVector));
    proxyClasses.insert(classPair(proxyName + DataItem::dataTypeShortStr(typeString|contVector),
    		DataBuffer::classDescription()));

    proxyName = "SimpleNumConverter";
    proxyFactory.registerClass(proxyName + DataItem::dataTypeShortStr(typeInt32),
    		new DataProxyInstantiator<SimpleNumConverter>(typeInt32));
    proxyClasses.insert(classPair(proxyName + DataItem::dataTypeShortStr(typeInt32),
    		SimpleNumConverter::classDescription()));
    proxyFactory.registerClass(proxyName + DataItem::dataTypeShortStr(typeUInt32),
    		new DataProxyInstantiator<SimpleNumConverter>(typeUInt32));
    proxyClasses.insert(classPair(proxyName + DataItem::dataTypeShortStr(typeUInt32),
    		SimpleNumConverter::classDescription()));
    proxyFactory.registerClass(proxyName + DataItem::dataTypeShortStr(typeInt64),
    		new DataProxyInstantiator<SimpleNumConverter>(typeInt64));
    proxyClasses.insert(classPair(proxyName + DataItem::dataTypeShortStr(typeInt64),
    		SimpleNumConverter::classDescription()));
    proxyFactory.registerClass(proxyName + DataItem::dataTypeShortStr(typeUInt64),
    		new DataProxyInstantiator<SimpleNumConverter>(typeUInt64));
    proxyClasses.insert(classPair(proxyName + DataItem::dataTypeShortStr(typeUInt64),
    		SimpleNumConverter::classDescription()));
    proxyFactory.registerClass(proxyName + DataItem::dataTypeShortStr(typeFloat),
    		new DataProxyInstantiator<SimpleNumConverter>(typeFloat));
    proxyClasses.insert(classPair(proxyName + DataItem::dataTypeShortStr(typeFloat),
    		SimpleNumConverter::classDescription()));
    proxyFactory.registerClass(proxyName + DataItem::dataTypeShortStr(typeDblFloat),
    		new DataProxyInstantiator<SimpleNumConverter>(typeDblFloat));
    proxyClasses.insert(classPair(proxyName + DataItem::dataTypeShortStr(typeDblFloat),
    		SimpleNumConverter::classDescription()));

    proxyFactory.registerClass(proxyName + DataItem::dataTypeShortStr(typeInt32|contVector),
    		new DataProxyInstantiator<SimpleNumConverter>(typeInt32|contVector));
    proxyClasses.insert(classPair(proxyName + DataItem::dataTypeShortStr(typeInt32|contVector),
    		SimpleNumConverter::classDescription()));
    proxyFactory.registerClass(proxyName + DataItem::dataTypeShortStr(typeUInt32|contVector),
    		new DataProxyInstantiator<SimpleNumConverter>(typeUInt32|contVector));
    proxyClasses.insert(classPair(proxyName + DataItem::dataTypeShortStr(typeUInt32|contVector),
    		SimpleNumConverter::classDescription()));
    proxyFactory.registerClass(proxyName + DataItem::dataTypeShortStr(typeInt64|contVector),
    		new DataProxyInstantiator<SimpleNumConverter>(typeInt64|contVector));
    proxyClasses.insert(classPair(proxyName + DataItem::dataTypeShortStr(typeInt64|contVector),
    		SimpleNumConverter::classDescription()));
    proxyFactory.registerClass(proxyName + DataItem::dataTypeShortStr(typeUInt64|contVector),
    		new DataProxyInstantiator<SimpleNumConverter>(typeUInt64|contVector));
    proxyClasses.insert(classPair(proxyName + DataItem::dataTypeShortStr(typeUInt64|contVector),
    		SimpleNumConverter::classDescription()));
    proxyFactory.registerClass(proxyName + DataItem::dataTypeShortStr(typeFloat|contVector),
    		new DataProxyInstantiator<SimpleNumConverter>(typeFloat|contVector));
    proxyClasses.insert(classPair(proxyName + DataItem::dataTypeShortStr(typeFloat|contVector),
    		SimpleNumConverter::classDescription()));
    proxyFactory.registerClass(proxyName + DataItem::dataTypeShortStr(typeDblFloat|contVector),
    		new DataProxyInstantiator<SimpleNumConverter>(typeDblFloat|contVector));
    proxyClasses.insert(classPair(proxyName + DataItem::dataTypeShortStr(typeDblFloat|contVector),
    		SimpleNumConverter::classDescription()));
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
