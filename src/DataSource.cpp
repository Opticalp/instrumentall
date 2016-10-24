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
		notifying(false),
		sourceCancelling(false),
		users(0)
{

}

DataSource::~DataSource()
{
	if (dataTargets.size())
		poco_bugcheck_msg("DataSource destruction: dataTargets is not empty");
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

    if (sourceCancelling)
    {
        releaseWriteOnFailure();
        throw ExecutionAbortedException(
                "DataSource::notifyReady",
                name() + " cancelling, "
                "not able to dispatch the data to the targets");
    }

    notifying = true;
    try
    {
		releaseWrite();
		Poco::Util::Application::instance()
							.getSubsystem<Dispatcher>()
							.setOutputDataReady(this);
    }
    catch (ExecutionAbortedException&)
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
		throw ExecutionAbortedException(
				"DataSource::registerPendingTarget",
				name() + " cancelling, "
				"not able to lock the data for the target: "
				+ target->name());

    Poco::ScopedLock<Poco::FastMutex> lock(pendingTargetsLock);
    return pendingDataTargets.insert(target).second;
}

bool DataSource::tryWriteDataLock()
{
    if (sourceCancelling)
        throw ExecutionAbortedException(
                "DataSource::tryWriteDataLock",
                name() + " cancelling, not able to lock the data");

    Poco::ScopedLock<Poco::FastMutex> lock(pendingTargetsLock);

	if (notifying || pendingDataTargets.size())
		return false;

	return DataItem::tryWriteDataLock();
}

void DataSource::releaseWrite()
{
	// TODO
	// setDataOk();
    DataItem::unlockData();
}

void DataSource::releaseWriteOnFailure()
{
	// TODO
	// setDataOk(false);  // DataItem::setDataOk
    DataItem::unlockData();
}


bool DataSource::tryReserveDataForTarget(DataTarget* target)
{
	Poco::ScopedLock<Poco::FastMutex> lock(pendingTargetsLock);

	if (pendingDataTargets.count(target))
		return reservedDataTargets.insert(target).second;
	else if (sourceCancelling)
		throw ExecutionAbortedException("DataSource::tryReserveDataForTarget",
				name() + " cancelling, can not reserve "
				+ target->name() + " for reading.");
	else
		return false;
}

void DataSource::readLockDataForTarget(DataTarget* target)
{
	Poco::ScopedLock<Poco::FastMutex> lock(pendingTargetsLock);

	if (sourceCancelling)
		throw ExecutionAbortedException("DataSource::tryReserveDataForTarget",
				name() + " cancelling, can not reserve "
				+ target->name() + " for reading.");

	if (reservedDataTargets.count(target) == 0)
		poco_bugcheck_msg("DataSource::readLockDataForTarget: "
				"trying to lock data that was not reserved");

	if (lockedDataTargets.insert(target).second)
		readDataLock();
}

void DataSource::targetReleaseRead(DataTarget* target)
{
	Poco::ScopedLock<Poco::FastMutex> lock(pendingTargetsLock);
	if (!pendingDataTargets.erase(target))
		poco_bugcheck_msg("call to targetReleaseRead without "
				"previous data reservation");

	reservedDataTargets.erase(target);
	// normal behavior: the target is erased.
	// abnormal behavior (e.g. abortion): the target was not reserved

	if (lockedDataTargets.erase(target))
		unlockData();
}

void DataSource::targetReleaseReadOnFailure(DataTarget* target)
{
    Poco::ScopedLock<Poco::FastMutex> lock(pendingTargetsLock);

    pendingDataTargets.erase(target);
    reservedDataTargets.erase(target);

    if (lockedDataTargets.erase(target))
        unlockData();
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
        poco_bugcheck_msg((name() + ": waiting for targets cancellation, "
                "although not cancelling").c_str());


    Poco::Util::Application::instance()
                        .getSubsystem<Dispatcher>()
                        .dispatchTargetWaitCancelled(this);
}

void DataSource::resetWithTargets()
{
	if (!sourceCancelling)
		return;

	// reseting targets
    Poco::Util::Application::instance()
                        .getSubsystem<Dispatcher>()
                        .dispatchTargetReset(this);

    // self
    sourceCancelling = false;
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

void DataSource::waitCancelledFromTarget(DataTarget* target)
{
    if (waiting->trySet())
    {
        // wait for the other targets (and the calling target...)
        waitTargetsCancelled();
        //self
        sourceWaitCancelled();
        waiting->reset();
    }
}

void DataSource::resetFromTarget(DataTarget* target)
{
	if (!sourceCancelling)
		return;

	// dispatch to the other targets (and the calling target... )
	resetWithTargets();

	// check that the locks are released >>> should --never-- be needed!
	pendingTargetsLock.lock();
	std::set<DataTarget*> targetsToRelease = pendingDataTargets;
	targetsToRelease.insert(reservedDataTargets.begin(), reservedDataTargets.end());
	pendingTargetsLock.unlock();

	for (std::set<DataTarget*>::iterator it = targetsToRelease.begin(),
			ite = targetsToRelease.end(); it != ite; it++)
		targetReleaseReadOnFailure(*it);

	// self
	sourceReset();
}
