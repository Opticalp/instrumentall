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

#include "Poco/Exception.h"

DataManager::DataManager()
{
    // Register data loggers in the factory using the C++ class name

    // e.g.:
    // loggerFactory.registerClass<DataPocoLogger>("DataPocoLogger");
    // loggerClasses.insert(classPair("DataPocoLogger", DataPocoLogger::description()));
}

DataManager::~DataManager()
{
    uninitialize(); // should have been already called by the system.

    // dataStore should clean itself nicely
}

void DataManager::initialize(Poco::Util::Application& app)
{
    // TODO: init loggers from config file?
}

void DataManager::uninitialize()
{
    // TODO: remove loggers
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
            **it = &emptyDataItem; // replace the pointed factory by something throwing exceptions
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
    // TODO
    // check if loggers are defined for this data

    // if no loggers
    //     return;


    // else
    // for each logger, acquireLock
    //      (*it)->acquireLock();

    // launch logger threads via thread manager.
//    Poco::Util::Application::instance()
//        .getSubsystem<ThreadManager>()
//        .start(*it)
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
