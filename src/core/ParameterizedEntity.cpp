/**
 * @file	src/core/ParameterizedEntity.cpp
 * @date	June 2016
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

#include "ParameterizedEntity.h"

#include "Poco/NumberParser.h"

void ParameterizedEntity::addParameter(size_t index, std::string name, std::string descr, ParamItem::ParamType datatype)
{
    try
    {
        paramSet.at(index).name = name;
        paramSet[index].descr = descr;
        paramSet[index].datatype = datatype;
    }
    catch (std::out_of_range& )
    {
        poco_bugcheck_msg("addParameter: incorrect index. "
                "Please check your module constructor");
    }

    switch (datatype)
    {
    case (ParamItem::typeInteger):
		paramValues[index] = Poco::Any(Poco::Int64(0));
    	break;
    case (ParamItem::typeFloat):
		paramValues[index] = Poco::Any(double(0));
		break;
    case (ParamItem::typeString):
		paramValues[index] = Poco::Any(std::string(""));
		break;
    default:
    	poco_bugcheck_msg("unsupported parameter type");
    }
}

void ParameterizedEntity::addParameter(size_t index, std::string name, std::string descr,
        ParamItem::ParamType datatype, std::string hardCodedValue)
{
    addParameter(index, name, descr, datatype);
    hardCodedValues.insert(std::pair<size_t, std::string>(index, hardCodedValue));
}

std::string ParameterizedEntity::getParameterDefaultValue(size_t index)
{
    // 1. check in the conf file
    std::string keyStr = confPrefixKey + "." + paramSet.at(index).name;

    try
    {
        return appConf().getString(keyStr);
    }
    catch (Poco::NotFoundException&)
    {
        poco_information(logger(),
            keyStr +
            std::string(" key not found in conf file. Using hard-coded default. "));
    }

    // 2. check in the hard coded values
    std::map<size_t, std::string>::iterator it = hardCodedValues.find(index);
    if (it != hardCodedValues.end())
        return it->second;

    throw Poco::NotFoundException("getParameterDefaultValue",
                                        "no default value found");
}

Poco::Int64 ParameterizedEntity::getIntParameterDefaultValue(size_t index)
{
    std::string strValue = getParameterDefaultValue(index);
    return static_cast<Poco::Int64>(Poco::NumberParser::parse64(strValue));
}

double ParameterizedEntity::getFloatParameterDefaultValue(size_t index)
{
    std::string strValue = getParameterDefaultValue(index);
    return Poco::NumberParser::parseFloat(strValue);
}

std::string ParameterizedEntity::getStrParameterDefaultValue(size_t index)
{
    return getParameterDefaultValue(index);
}

void ParameterizedEntity::getParameterSet(ParameterSet* pSet)
{
    pSet->clear();
    pSet->reserve(paramSet.size());
    for (ParameterSet::iterator it = paramSet.begin(),
            ite = paramSet.end(); it != ite; it++)
    {
        pSet->push_back(ParamItem());
        pSet->back().name = it->name;
        pSet->back().descr = it->descr;
        pSet->back().datatype = it->datatype;
    }
}

size_t ParameterizedEntity::getParameterIndex(std::string paramName)
{
    size_t length = paramSet.size();

    for (size_t index = 0; index < length; index++)
        if (paramSet[index].name.compare(paramName) == 0)
            return index;

    throw Poco::NotFoundException("getParameterIndex", "parameter name not found");
}

ParamItem::ParamType ParameterizedEntity::getParameterType(std::string paramName)
{
    return paramSet[getParameterIndex(paramName)].datatype;
}

using Poco::AnyCast;

bool ParameterizedEntity::getInternalIntParameterValue(size_t paramIndex, Poco::Int64& value)
{
    Poco::Mutex::ScopedLock lock(mutex);

	try
	{
		switch (paramSet.at(paramIndex).datatype)
		{
		case (ParamItem::typeInteger):
			break;
		case (ParamItem::typeFloat):
		case (ParamItem::typeString):
			throw Poco::DataFormatException("getInternalIntParameterValue",
					"bad parameter type");
		default:
			poco_bugcheck_msg("getInternalIntParameterValue, "
					"unknown parameter type");
		}
	}
	catch (std::out_of_range&)
	{
		poco_bugcheck_msg("getInternalIntParameterValue, "
				"out of range parameter index");
	}

	value = AnyCast<Poco::Int64>(paramValues[paramIndex]);

	bool ret = needApplication[paramIndex];
	needApplication[paramIndex] = false;
	return ret;
}

bool ParameterizedEntity::getInternalFloatParameterValue(size_t paramIndex, double& value)
{
    Poco::Mutex::ScopedLock lock(mutex);

	try
	{
		switch (paramSet.at(paramIndex).datatype)
		{
		case (ParamItem::typeInteger):
		case (ParamItem::typeString):
			throw Poco::DataFormatException("getInternalFloatParameterValue",
					"bad parameter type");
		case (ParamItem::typeFloat):
			break;
		default:
			poco_bugcheck_msg("getInternalFloatParameterValue, "
					"unknown parameter type");
		}
	}
	catch (std::out_of_range&)
	{
		poco_bugcheck_msg("getInternalFloatParameterValue, "
				"out of range parameter index");
	}

	value = AnyCast<double>(paramValues[paramIndex]);

	bool ret = needApplication[paramIndex];
	needApplication[paramIndex] = false;
	return ret;
}

bool ParameterizedEntity::getInternalStrParameterValue(size_t paramIndex, std::string& value)
{
	try
	{
		switch (paramSet.at(paramIndex).datatype)
		{
		case (ParamItem::typeInteger):
		case (ParamItem::typeFloat):
			throw Poco::DataFormatException("getInternalStrParameterValue",
					"bad parameter type");
		case (ParamItem::typeString):
			break;
		default:
			poco_bugcheck_msg("getInternalStrParameterValue, "
					"unknown parameter type");
		}
	}
	catch (std::out_of_range&)
	{
		poco_bugcheck_msg("getInternalStrParameterValue, "
				"out of range parameter index");
	}

	value = AnyCast<std::string>(paramValues[paramIndex]);

	bool ret = needApplication[paramIndex];
	needApplication[paramIndex] = false;
	return ret;
}

void ParameterizedEntity::applyParameters()
{
	for (size_t index = 0; index < paramSet.size(); index++)
	{
		switch (paramSet[index].datatype)
		{
		case (ParamItem::typeInteger):
			{
				Poco::Int64 tmp;
				if (getInternalIntParameterValue(index, tmp))
					setIntParameterValue(index, tmp);
				break;
			}
		case (ParamItem::typeFloat):
			{
				double tmp;
				if (getInternalFloatParameterValue(index, tmp))
					setFloatParameterValue(index, tmp);
				break;
			}
		case (ParamItem::typeString):
			{
				std::string tmp;
				if (getInternalStrParameterValue(index, tmp))
					setStrParameterValue(index, tmp);
				break;
			}
		default:
			poco_bugcheck_msg("applyParameters, "
					"unknown parameter type");
		}
	}
}

