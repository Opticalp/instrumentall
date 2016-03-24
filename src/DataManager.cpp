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

#include "DataPocoLogger.h"

#include "Poco/Exception.h"
#include "Poco/Util/Application.h"

DataManager::DataManager():
    VerboseEntity(name())
{
    // Register data loggers in the factory using the C++ class name

    loggerFactory.registerClass<DataPocoLogger>("DataPocoLogger");
    loggerClasses.insert(classPair("DataPocoLogger", DataPocoLogger::description()));
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

void DataManager::addOutPort(OutPort* port)
{
    allDataLock.writeLock();
    allData.push_back(SharedPtr<DataItem*>(new DataItem*(port->dataItem())));
    allDataLock.unlock();
}

void DataManager::removeOutPort(OutPort* port)
{
    allDataLock.writeLock();

    for (std::vector< SharedPtr<DataItem*> >::iterator it = allData.begin(),
            ite = allData.end(); it != ite; it++)
    {
        if (port->dataItem() == **it)
        {
            // unregister data item loggers
            std::set< SharedPtr<DataLogger*> > itemLoggers = port->dataItem()->loggers();
            for (std::set< SharedPtr<DataLogger*> >::iterator setIt = itemLoggers.begin(),
                    setIte = itemLoggers.end(); setIt != setIte; setIt++ )
                (**setIt)->detach();

            // replace the pointed data item by something throwing exceptions
            **it = &emptyDataItem;
            allData.erase(it);
            // poco_information(logger(), port->name() + " port DataItem "
            //         "erased from DataManager::allData. ");
            allDataLock.unlock();
            return;
        }
    }

    allDataLock.unlock();
    poco_error(logger(), "removeOutPort(): "
            "the port was not found");
}

void DataManager::newData(DataItem* self)
{
    if (!self->hasLoggers())
        return;

    std::set< SharedPtr<DataLogger*> > itemLoggers = self->loggers();

    for (std::set< SharedPtr<DataLogger*> >::iterator it = itemLoggers.begin(),
            ite = itemLoggers.end(); it != ite; it++ )
        (**it)->acquireLock();

    for (std::set< SharedPtr<DataLogger*> >::iterator it = itemLoggers.begin(),
            ite = itemLoggers.end(); it != ite; it++ )
    {
        // launch logger threads via thread manager.
        Poco::Util::Application::instance()
            .getSubsystem<ThreadManager>()
            .start(**it);
    }
}

SharedPtr<DataItem*> DataManager::getDataItem(DataItem* dataItem)
{
    allDataLock.readLock();
    for (std::vector< SharedPtr<DataItem*> >::iterator it = allData.begin(),
            ite = allData.end(); it != ite; it++)
    {
        if (dataItem==**it)
        {
            allDataLock.unlock();
            return *it;
        }
    }

    allDataLock.unlock();
    throw Poco::NotFoundException("getDataItem", "Data not found: "
            "Should have been deleted during the query");
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

void DataManager::registerLogger(SharedPtr<DataItem*> data,
        SharedPtr<DataLogger*> dataLogger)
{
    if (*data == &emptyDataItem)
        throw Poco::NotFoundException("registerLogger",
                                        "Data item not found");

    (*dataLogger)->registerData(*data);
}

SharedPtr<DataItem*> DataManager::getSourceDataItem(
        SharedPtr<DataLogger*> dataLogger)
{
    DataItem* data = (*dataLogger)->data();

    if (data)
        return getDataItem(data);
    else
        throw Poco::NotFoundException("DataManager::getsourceDataItem",
                "No source data item found. "
                "The logger may be detached. ");
}


void DataManager::removeDataLogger(SharedPtr<DataLogger*> logger)
{
    // switch the logger into empty state
    (*logger)->setEmpty();

    // remove the data logger from loggers. nothing to delete.
    loggers.erase(logger);
}
