/**
 * @file	src/ParameterSetter.cpp
 * @date	Aug. 2016
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

#include "ParameterSetter.h"
#include "ParameterizedEntity.h"
#include "ParameterizedWithSetters.h"

#include "Poco/NumberFormatter.h"

size_t ParameterSetter::refCount = 0;

ParameterSetter::ParameterSetter(ParameterizedEntity* parameterized,
		size_t paramIndex,
		ParameterizedWithSetters* handler,
		bool immediate):
			ParameterWorker(parameterized, paramIndex),
			settersHandler(handler),
			mName(getParent()->name() + "ParameterSetter"),
			immediateApply(immediate)
{
	if (refCount)
		mName += Poco::NumberFormatter::format(refCount);

	refCount++;
}

std::string ParameterSetter::description()
{
	if (getParent())
		return "Set the parameter: " + getParameterName()
				+ " of " + getParent()->name();
	else
		return "Expired parameter setter. You should consider deleting it. ";
}

std::set<int> ParameterSetter::supportedInputDataType()
{
	std::set<int> ret;
	ret.insert(getParameterDataType());
	return ret;
}

void ParameterSetter::runTarget()
{
	if (!tryCatchSource())
		poco_bugcheck_msg("ParameterSetter::runTarget, "
				"not able to catch the source");

	if (getParent() == NULL)
	{
		releaseInputData();
		throw Poco::InvalidAccessException("ParameterSetter",
				"The parent is no more valid");
	}

	while (!settersHandler->trySetParameter(getParameterIndex()))
	{
		if (yield())
		{
			releaseInputData();
			throw Poco::RuntimeException("ParameterSetter::runTarget",
					"Task cancellation upon user request");
		}
	}

	try
	{
		switch (getParameterDataType())
		{
		case TypeNeutralData::typeInt64:
			getParent()->setParameterValue<Poco::Int64>(getParameterIndex(),
					*getDataSource()->getData<Poco::Int64>(),
					immediateApply);
			break;
		case TypeNeutralData::typeDblFloat:
			getParent()->setParameterValue<double>(getParameterIndex(),
					*getDataSource()->getData<double>(),
					immediateApply);
			break;
		case TypeNeutralData::typeString:
			getParent()->setParameterValue<std::string>(getParameterIndex(),
					*getDataSource()->getData<std::string>(),
					immediateApply);
			break;
		default:
			poco_bugcheck_msg("ParameterSetter::runTarget, "
					"unknown parameter type");
		}
	}
	catch (...)
	{
		releaseInputData();
		throw;
	}

	releaseInputData();
}

void ParameterSetter::targetCancel()
{
    if (getParent())
        getParent()->cancel(getDataSource());
}

void ParameterSetter::targetWaitCancelled()
{
    if (getParent())
        getParent()->waitCancelled();
}

void ParameterSetter::targetReset()
{
    if (getParent())
        getParent()->reset();
}
