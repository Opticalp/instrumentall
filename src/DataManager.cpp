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

#include "DataPocoLogger.h"

#include "Poco/Exception.h"
#include "Poco/Util/Application.h"

DataManager::DataManager():
    VerboseEntity(name())
{
    // Register data loggers in the factory using the C++ class name

    loggerFactory.registerClass<DataPocoLogger>("DataPocoLogger");
    loggerClasses.insert(classPair("DataPocoLogger", DataPocoLogger::classDescription()));
}

DataManager::~DataManager()
{
    // uninitialize(); // should have been already called by the system.

    // dataStore should clean itself nicely
}

void DataManager::initialize(Poco::Util::Application& app)
{
    setLogger(name());

    // TODO: init loggers from config file?
}

void DataManager::uninitialize()
{
    poco_information(logger(),"Data manager uninitializing");
    // TODO: remove loggers
    // TODO: empty all data (expired, etc)
}

SharedPtr<DataLogger*> DataManager::newDataLogger(std::string className)
{
    DataLogger* logger = loggerFactory.createInstance(className);
    SharedPtr<DataLogger*> tmpPtr(new (DataLogger*)(logger));

    loggersLock.writeLock();
    loggers.insert(tmpPtr);
    loggersLock.unlock();

    return tmpPtr;
}

std::set<SharedPtr<DataLogger*> > DataManager::dataLoggers()
{
    std::set< SharedPtr<DataLogger*> > tmpLoggers;
    loggersLock.readLock();
    tmpLoggers = loggers;
    loggersLock.unlock();

    return tmpLoggers;
}

SharedPtr<DataLogger*> DataManager::getDataLogger(DataLogger* dataLogger)
{
    loggersLock.readLock();

    for (std::set< SharedPtr<DataLogger*> >::iterator it = loggers.begin(),
            ite = loggers.end(); it != ite; it++ )
    {
        if (**it == dataLogger)
        {
            SharedPtr<DataLogger*> tmp = *it;
            loggersLock.unlock();
            return tmp;
        }
    }

    loggersLock.unlock();
    throw Poco::NotFoundException("DataManager",
            "The given data logger was not found");
}

SharedPtr<DataProxy*> DataManager::newDataProxy(std::string className)
{
    DataProxy* proxy = proxyFactory.createInstance(className);
    SharedPtr<DataProxy*> tmpPtr(new (DataProxy*)(proxy));

    proxiesLock.writeLock();
    proxies.insert(tmpPtr);
    proxiesLock.unlock();

    return tmpPtr;
}

std::set<SharedPtr<DataProxy*> > DataManager::dataProxies()
{
	Poco::ScopedReadRWLock lock(proxiesLock);
    return proxies;
}

SharedPtr<DataProxy*> DataManager::getDataProxy(DataProxy* dataProxy)
{
    proxiesLock.readLock();

    for (std::set< SharedPtr<DataProxy*> >::iterator it = proxies.begin(),
            ite = proxies.end(); it != ite; it++ )
    {
        if (**it == dataProxy)
        {
            SharedPtr<DataProxy*> tmp = *it;
            proxiesLock.unlock();
            return tmp;
        }
    }

    proxiesLock.unlock();
    throw Poco::NotFoundException("DataManager",
            "The given data proxy was not found");
}
