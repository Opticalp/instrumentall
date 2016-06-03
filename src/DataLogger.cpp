/**
 * @file	src/DataLogger.cpp
 * @date	Mar 2016
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

#include "DataLogger.h"
#include "DataItem.h"

DataLogger::~DataLogger()
{
    detach();
}

void DataLogger::detach()
{
    dataLock.lock();
    detachNoLock();
    dataLock.unlock();
}

void DataLogger::detachNoLock()
{
    if (pData)
    {
        pData->detachLogger(this);
        pData = NULL;
    }
}

void DataLogger::registerData(DataItem* data)
{
    if (empty)
        throw Poco::InvalidAccessException(name(),
                "The logger was deleted");

    dataLock.lock();

    if (!isSupportedDataType(data->dataType()))
    {
        dataLock.unlock();
        throw Poco::RuntimeException("registerLogger",
                "This logger do not support this data type");
    }

    // deregister previous parent
    detachNoLock();

    // bind to data
    data->registerLogger(this);
    pData = data;

    dataLock.unlock();
}

void DataLogger::acquireLock()
{
    dataLock.lock();
    if (pData)
        pData->readLock();
    dataLock.unlock();
}

void DataLogger::setEmpty()
{
    dataLock.lock();

    detachNoLock();
    empty = true;

    dataLock.unlock();
}

DataItem* DataLogger::data()
{
    DataItem* tmp;

    // lock in case somebody else already locks
    dataLock.lock();
    tmp =  pData;
    dataLock.unlock();

    return tmp;
}

void DataLogger::run()
{
    Poco::Mutex::ScopedLock localLock(dataLock);

    if (data() == NULL)
        return;

    try
    {
        log();
    }
    catch (Poco::Exception& e)
    {
        data()->releaseData();
        e.rethrow();
    }

    data()->releaseData();
}
