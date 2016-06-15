/**
 * @file	src/DataItem.cpp
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

#include "DataItem.h"
#include "DataLogger.h"
#include "DataManager.h"

#include "Poco/Types.h"
#include "Poco/Util/Application.h"

DataItem::DataItem(int dataType, OutPort* parent):
		TypeNeutralData(dataType),
        mParentPort(parent),
		expired(true)
{
}

DataItem::~DataItem()
{
    // The loggers are detached by DataManager::removeOutPort()

}

void DataItem::releaseNewData()
{
	expired = false;
    releaseData();

    Poco::Util::Application::instance()
            .getSubsystem<DataManager>()
            .newData(this);
}

void DataItem::releaseBrokenData()
{
    expired = true;
    releaseData();
}

void DataItem::registerLogger(DataLogger* logger)
{
    loggersLock.writeLock();
    allLoggers.insert(logger);
    loggersLock.unlock();
}

std::set<SharedPtr<DataLogger*> > DataItem::loggers()
{
    std::set<SharedPtr<DataLogger*> > tmpList;

    loggersLock.readLock();

    for (std::set< DataLogger* >::iterator it = allLoggers.begin(),
            ite = allLoggers.end(); it != ite; it++ )
    {
        tmpList.insert( Poco::Util::Application::instance()
                            .getSubsystem<DataManager>()
                            .getDataLogger(*it) );
    }

    loggersLock.unlock();

    return tmpList;
}

void DataItem::detachLogger(DataLogger* logger)
{
    loggersLock.writeLock();
    allLoggers.erase(logger);
    loggersLock.unlock();
}
