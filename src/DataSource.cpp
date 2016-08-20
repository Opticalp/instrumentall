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
		notifying(0),
		sourceCancelling(false),
		users(0)
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

bool DataSource::registerPendingTarget(DataTarget* target)
{
	if (sourceCancelling)
		throw Poco::InvalidAccessException(
				"DataSource::registerPendingTarget",
				name() + " cancelling, "
				"not able to lock the data for the target: "
				+ target->name());

    bool ret;
    readDataLock();

    pendingTargetsLock.lock();
    ret = pendingDataTargets.insert(target).second;
    pendingTargetsLock.unlock();

    if (!ret)
    {
    	unlockData();
    	return false;
    }
    else
    {
    	return true;
    }
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
	if (pendingDataTargets.erase(target))
	{
		if (sourceCancelling)
		{
			unlockData();
			throw Poco::InvalidAccessException("DataSource::tryCatchRead",
					name() + " cancelling, can not catch read "
					+ target->name() + ". Source lock released. ");
		}

		return true;
	}
	else
	{
		if (sourceCancelling)
			throw Poco::InvalidAccessException("DataSource::tryCatchRead",
					name() + " cancelling, can not catch read "
					+ target->name()
					+ ". No source lock to release. ");

		return false;
	}
}

void DataSource::cancelWithTargets()
{
	if (sourceCancelling)
		return;

	// self
	sourceCancelling = true;

	// cancelling targets
    Poco::Util::Application::instance()
                        .getSubsystem<Dispatcher>()
                        .dispatchTargetCancel(this);
}

void DataSource::waitTargetsCancelled()
{
	if (!sourceCancelling)
		return;

    Poco::Util::Application::instance()
                        .getSubsystem<Dispatcher>()
                        .dispatchTargetWaitCancelled(this);
}

void DataSource::resetWithTargets()
{
	if (!sourceCancelling)
		return;

	// self
	sourceCancelling = false;

	// reseting targets
    Poco::Util::Application::instance()
                        .getSubsystem<Dispatcher>()
                        .dispatchTargetReset(this);
}

void DataSource::cancelFromTarget(DataTarget* target)
{
	if (sourceCancelling)
		return;

	// dispatch to the other targets (and the calling target... )
	cancelWithTargets();
	// self
	sourceCancel();
}

void DataSource::resetFromTarget(DataTarget* target)
{
	if (!sourceCancelling)
		return;

	// dispatch to the other targets (and the calling target... )
	resetWithTargets();
	// self
	sourceReset();
}
