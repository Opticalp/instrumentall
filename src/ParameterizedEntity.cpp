/**
 * @file	src/ParameterizedEntity.cpp
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

long ParameterizedEntity::getIntParameterDefaultValue(size_t index)
{
    std::string strValue = getParameterDefaultValue(index);
    return static_cast<long>(Poco::NumberParser::parse64(strValue));
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
    // mainMutex.lock();
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
    // mainMutex.unlock();
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
    // TODO: mainMutex scoped lock
    return paramSet[getParameterIndex(paramName)].datatype;
}
