/**
 * @file	src/ParameterGetter.cpp
 * @date	30 Jul. 2016
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

int ParameterGetter::paramDataType(ParameterizedEntity* parameterized,
		size_t paramIndex)
{
	if (parameterized == NULL)
		throw Poco::NullPointerException("ParameterGetter::paramDataType");

	ParameterSet params;
	parameterized->getParameterSet(&params);

	try
	{
		switch (params.at(paramIndex).datatype)
		{
		case (ParamItem::typeInteger):
			return TypeNeutralData::typeInt64;
		case (ParamItem::typeFloat):
			return TypeNeutralData::typeDblFloat;
		case (ParamItem::typeString):
			return TypeNeutralData::typeString;
		default:
			poco_bugcheck_msg("ParameterGetter::paramDataType, "
					"unknown parameter type");
		}
	}
	catch (std::out_of_range&)
	{
		throw Poco::RangeException("ParameterGetter::paramDataType",
				"out of range parameter index");
	}
}

ParameterGetter::ParameterGetter(ParameterizedEntity* parameterized,
		size_t paramIndex):
				DataSource(paramDataType(parameterized, paramIndex)),
				parent(parameterized),
				mParamIndex(paramIndex)
{

}

void ParameterGetter::runTarget()
{
	if (!tryCatchSource())
		poco_bugcheck_msg("ParameterGetter::runTarget, "
				"not able to catch the source");

	DataAttribute attr;
	readInputDataAttribute(&attr);
	releaseInputData();

	while (!tryWriteDataLock())
	{
		if (yield())
			throw Poco::RuntimeException("ParameterGetter::runTarget",
					"Task cancellation upon user request");
	}

	try
	{
		switch (dataType())
		{
		case TypeNeutralData::typeInt64:
			*getData<Poco::Int64>() = parent->getParameterValue<Poco::Int64>(mParamIndex);
			break;
		case TypeNeutralData::typeDblFloat:
			*getData<double>() = parent->getParameterValue<double>(mParamIndex);
			break;
		case TypeNeutralData::typeString:
			*getData<std::string>() = parent->getParameterValue<std::string>(mParamIndex);
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

