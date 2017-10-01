/**
 * @file	src/core/DataTarget.cpp
 * @date	Jul. 2016
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

#include "DataTarget.h"

DataTarget::DataTarget():
	dataSource(NULL), users(0),
	targetCancelling(false)
{

}

DataTarget::~DataTarget()
{
	detachDataSource();
}

DataSource* DataTarget::getDataSource()
{
	Poco::ScopedLock<Poco::FastMutex> lock(sourceLock);

	if (dataSource)
		return dataSource;
	else
		throw Poco::NullPointerException("getDataSource",
				"no data source");
}

void DataTarget::setDataSource(DataSource* source)
{
    if (source && !isSupportedInputDataType(source->dataType()))
        throw Poco::DataFormatException("setDataSource",
                "The target does not support this data type: "
        		+ DataItem::dataTypeStr(source->dataType()));

	bool dec = false; // decrementator flag

    // scoped locked block
    {
		Poco::ScopedLock<Poco::FastMutex> lock(sourceLock);

		if (dataSource)
			dataSource->detachDataTarget(this);

		if (source && (dataSource == NULL))
			incUser();
		else if ((source == NULL) && dataSource)
			dec = true;

		dataSource = source;

		if (dataSource)
			dataSource->addDataTarget(this);
    }

	if (dec)
		decUser();
}

bool DataTarget::tryCatchSource()
{
    Poco::ScopedLock<Poco::FastMutex> lock(sourceLock);
	if (dataSource)
		return dataSource->tryReserveDataForTarget(this);
	else
		return false;
}

void DataTarget::lockSource()
{
    Poco::ScopedLock<Poco::FastMutex> lock(sourceLock);
    dataSource->readLockDataForTarget(this);
}

int DataTarget::sourceDataType()
{
	if (dataSource == NULL)
		throw Poco::InvalidAccessException(name(),
				"This target has no source");
	else
		return dataSource->dataType();
}

void DataTarget::detachDataSource()
{
	setDataSource(NULL);
}

void DataTarget::readInputDataAttribute(DataAttribute* pAttr)
{
	*pAttr = getDataSource()->getDataAttribute();
}

void DataTarget::releaseInputData()
{
    getDataSource()->targetReleaseRead(this);
}

void DataTarget::releaseInputDataOnFailure()
{
    getDataSource()->targetReleaseReadOnFailure(this);
}

void DataTarget::cancelWithSource()
{
	if (targetCancelling)
		return;

	// self
	targetCancelling = true;

	try
	{
	    getDataSource()->cancelFromTarget(this);
	}
	catch (Poco::NullPointerException&)
	{
	    return;
	}
}

void DataTarget::waitSourceCancelled()
{
	if (!targetCancelling)
		poco_bugcheck_msg((name() + ": waiting for source cancellation, "
				"although not cancelling").c_str());

    try
    {
        getDataSource()->waitCancelledFromTarget(this);
    }
    catch (Poco::NullPointerException&)
    {
        return;
    }
}

void DataTarget::resetWithSource()
{
	if (!targetCancelling)
		return;

	// self
	targetCancelling = false;

	try
	{
		getDataSource()->resetFromTarget(this);
    }
    catch (Poco::NullPointerException&)
    {
        return;
    }
}

bool DataTarget::tryRunTarget()
{
	if (targetCancelling)
	{
		releaseInputData();
		return false;
	}

	try
	{
		runTarget();
	}
	catch (...)
	{
		releaseInputDataOnFailure();
		return false;
	}

	return true;
}

void DataTarget::cancelFromSource(DataSource* source)
{
	if (targetCancelling)
		return;

	// self
	targetCancelling = true;
	targetCancel();
}

void DataTarget::waitCancelledFromSource(DataSource* source)
{
    if (waiting->trySet())
    {
        targetWaitCancelled();
        waiting->reset();
    }
}

void DataTarget::resetFromSource(DataSource* source)
{
	if (!targetCancelling)
		return;

    // self
	targetReset();

    targetCancelling = false;
}
