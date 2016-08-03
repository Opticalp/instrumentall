/**
 * detailed comment
 * 
 * @file	/Instrumentall-Debug@instru-git-debug/[Source directory]/src/DataSource.cpp
 * @brief	short comment
 * @date	15 juil. 2016
 * @author	PhRG - opticalp.fr
 *
 * $Id$
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

#include "DataSource.h"
#include "Module.h"

#include "Dispatcher.h"

#include "Poco/Util/Application.h"

DataSource::DataSource(int datatype):
		DataItem(datatype),
		notifying(0), users(0)
{

}

DataSource::~DataSource()
{
	if (dataTargets.size())
		poco_bugcheck_msg("DataSource destruction: dataTargets is not empty");
}

void DataSource::releaseWrite()
{
	// TODO
	// setDataOk();
    unlockData();
}

void DataSource::releaseWriteOnFailure()
{
	// TODO
	// setDataOk(false);  // DataItem::setDataOk
	unlockData();
}


void DataSource::targetReleaseRead(DataTarget* target)
{
	// check that the target is not in the pending targets anymore
	Poco::ScopedLockWithUnlock<Poco::FastMutex> lock(pendingTargetsLock);
	if (pendingDataTargets.erase(target))
		poco_bugcheck_msg("call to targetReleaseRead without "
				"previous call to tryCatchSource");
	lock.unlock();

	unlockData();
}

std::set<DataTarget*> DataSource::getDataTargets()
{
    Poco::ScopedLock<Poco::FastMutex> lock(targetsLock);
    return dataTargets;
}

void DataSource::addDataTarget(DataTarget* target)
{
	Poco::ScopedLock<Poco::FastMutex> lock(targetsLock);
    if (dataTargets.insert(target).second)
    	incUser();
}

void DataSource::notifyReady(DataAttribute attribute)
{
    setDataAttribute(attribute);

    notifying = true;
    try
    {
		releaseWrite();
		Poco::Util::Application::instance()
							.getSubsystem<Dispatcher>()
							.setOutputDataReady(this);
    }
    catch (...)
    {
        notifying = false;
        throw;
    }
    notifying = false;
}

void DataSource::detachDataTarget(DataTarget* target)
{
	targetsLock.lock();
	size_t tmp = dataTargets.erase(target);
	targetsLock.unlock();

    if (tmp)
    	decUser();
}

void DataSource::registerPendingTarget(DataTarget* target)
{
    readDataLock();

    pendingTargetsLock.lock();
    pendingDataTargets.insert(target);
    pendingTargetsLock.unlock();
}

bool DataSource::tryWriteDataLock()
{
	if (notifying)
		return false;

	bool ret = DataItem::tryWriteDataLock();

	if (ret)
	{
		Poco::ScopedLock<Poco::FastMutex> lock(pendingTargetsLock);
		if (pendingDataTargets.size())
			poco_bugcheck_msg((name() + " pending targets is not empty, "
					"write lock should not have been possible").c_str());

		return true;
	}
	else
		return false;
}

bool DataSource::tryCatchRead(DataTarget* target)
{
	Poco::ScopedLock<Poco::FastMutex> lock(pendingTargetsLock);
	return (pendingDataTargets.erase(target) != 0);
}
