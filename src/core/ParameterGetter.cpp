/**
 * @file	src/core/ParameterGetter.cpp
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

#include "ParameterGetter.h"
#include "ParameterizedEntity.h"
#include "ExecutionAbortedException.h"

#include "Poco/NumberFormatter.h"

size_t ParameterGetter::refCount = 0;

ParameterGetter::ParameterGetter(ParameterizedEntity* parameterized,
		size_t paramIndex):
				ParameterWorker(parameterized, paramIndex),
//				DataSource(paramDataType(parameterized, paramIndex)),
				DataSource(getParameterDataType()),
				mName(getParent()->name() + "ParameterGetter")
{
	if (refCount)
		mName += Poco::NumberFormatter::format(refCount);

	refCount++;
}

std::string ParameterGetter::description()
{
	if (getParent())
		return "Get the parameter: " + getParameterName()
				+ " from " + getParent()->name();
	else
		return "Expired parameter getter. You should consider deleting it. ";
}

void ParameterGetter::runTarget()
{
	if (!tryCatchSource())
		poco_bugcheck_msg("ParameterGetter::runTarget, "
				"not able to catch the source");

	if (getParent() == NULL)
	{
		releaseInputData();
		throw Poco::InvalidAccessException("ParameterGetter",
				"The parent is no more valid");
	}

	lockSource();
	DataAttribute attr;
	readInputDataAttribute(&attr);
	releaseInputData();

	while (!tryWriteDataLock())
	{
		if (yield())
			throw ExecutionAbortedException("ParameterGetter::runTarget",
					"Task cancellation upon user request");
	}

	try
	{
		switch (dataType())
		{
		case TypeNeutralData::typeInt64:
			*getData<Poco::Int64>() = getParent()->getParameterValue<Poco::Int64>(getParameterIndex());
			break;
		case TypeNeutralData::typeDblFloat:
			*getData<double>() = getParent()->getParameterValue<double>(getParameterIndex());
			break;
		case TypeNeutralData::typeString:
			*getData<std::string>() = getParent()->getParameterValue<std::string>(getParameterIndex());
			break;
		default:
			poco_bugcheck_msg("ParameterGetter::runTarget, "
					"unknown parameter type");
		}
	}
	catch (...)
	{
		releaseWriteOnFailure();
		throw;
	}

	notifyReady(attr);
}

void ParameterGetter::targetWaitCancelled()
{
    getParent()->waitCancelled();
    waitTargetsCancelled();
}

void ParameterGetter::sourceWaitCancelled()
{
    waitSourceCancelled();
    getParent()->waitCancelled();
}
