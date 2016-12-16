/**
 * @file	src/core/ParameterWorker.cpp
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

#include "ParameterWorker.h"
#include "ParameterizedEntity.h"
#include "TypeNeutralData.h"

int ParameterWorker::paramDataType(ParameterizedEntity* parameterized,
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
			throw Poco::BugcheckException();
		}
	}
	catch (std::out_of_range&)
	{
		throw Poco::RangeException("ParameterGetter::paramDataType",
				"out of range parameter index");
	}
}

ParameterWorker::ParameterWorker(ParameterizedEntity* parameterized,
		size_t paramIndex):
		parent(parameterized),
		mParamIndex(paramIndex),
		mDatatype(paramDataType(parameterized, paramIndex))
{
}

std::string ParameterWorker::getParameterName() const
{
	if (parent)
	{
		ParameterSet tmp;
		parent->getParameterSet(&tmp);
		return tmp.at(mParamIndex).name;
	}
	else
		throw Poco::InvalidAccessException("getParameterName",
				"Invalidated parameter getter");
}

