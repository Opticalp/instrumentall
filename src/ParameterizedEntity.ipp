/**
 * @file	src/ParameterizedEntity.ipp
 * @date	June 2016
 * @author	PhRG - opticalp.fr
 *
 * ParameterizedEntity class specialized templates implementation
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

template <typename T> inline
T ParameterizedEntity::getParameterValue(std::string paramName)
{
    size_t paramIndex = getParameterIndex(paramName);
    return getParameterValue<T>(paramIndex);
}

template <> inline
Poco::Int64 ParameterizedEntity::getParameterValue<Poco::Int64>(size_t paramIndex)
{
    Poco::Mutex::ScopedLock lock(mutex);

    applyParameters(); // "mutex" is a recursive mutex... OK then.

    switch (paramSet[paramIndex].datatype)
    {
    case ParamItem::typeInteger:
        return getIntParameterValue(paramIndex);
    case ParamItem::typeFloat:
        throw Poco::DataFormatException("getParameterValue",
                "The parameter " + paramSet[paramIndex].name + " has float type. "
                "Integer was requested. ");
    case ParamItem::typeString:
        throw Poco::DataFormatException("getParameterValue",
                "The parameter " + paramSet[paramIndex].name + " has string type. "
                "Integer was requested. ");
    default:
        poco_bugcheck_msg("unrecognized parameter type");
        throw Poco::BugcheckException(); // to avoid compiler warning
    }
}

template <> inline
double ParameterizedEntity::getParameterValue<double>(size_t paramIndex)
{
    Poco::Mutex::ScopedLock lock(mutex);

    applyParameters(); // "mutex" is a recursive mutex... OK then.

    switch (paramSet[paramIndex].datatype)
    {
    case ParamItem::typeInteger:
        throw Poco::DataFormatException("getParameterValue",
                "The parameter " + paramSet[paramIndex].name + " has integer type. "
                "Float was requested. ");
    case ParamItem::typeFloat:
        return getFloatParameterValue(paramIndex);
    case ParamItem::typeString:
        throw Poco::DataFormatException("getParameterValue",
                "The parameter " + paramSet[paramIndex].name + " has string type. "
                "Float was requested. ");
    default:
        poco_bugcheck_msg("unrecognized parameter type");
        throw Poco::BugcheckException(); // to avoid compiler warning
    }
}

/// ParameterizedEntity::getParameter specialization for string parameters
template <> inline
std::string ParameterizedEntity::getParameterValue<std::string>(size_t paramIndex)
{
    Poco::Mutex::ScopedLock lock(mutex);

    applyParameters(); // "mutex" is a recursive mutex... OK then.

    switch (paramSet[paramIndex].datatype)
    {
    case ParamItem::typeInteger:
        throw Poco::DataFormatException("getParameterValue",
                "The parameter " + paramSet[paramIndex].name + " has integer type. "
                "String was requested. ");
    case ParamItem::typeFloat:
        throw Poco::DataFormatException("getParameterValue",
                "The parameter " + paramSet[paramIndex].name + " has float type. "
                "String was requested. ");
    case ParamItem::typeString:
        return getStrParameterValue(paramIndex);
    default:
        poco_bugcheck_msg("unrecognized parameter type");
        throw Poco::BugcheckException(); // to avoid compiler warning
    }
}

template <> inline
void ParameterizedEntity::setParameterValue<Poco::Int64>(std::string paramName, Poco::Int64 value, bool immediateApply)
{
    size_t paramIndex = getParameterIndex(paramName);

    switch (paramSet[paramIndex].datatype)
    {
    case ParamItem::typeInteger:
        break;
    case ParamItem::typeFloat:
        throw Poco::DataFormatException("setParameterValue",
                "The parameter " + paramName + " has float type. "
                "Integer was given. ");
    case ParamItem::typeString:
        throw Poco::DataFormatException("setParameterValue",
                "The parameter " + paramName + " has string type. "
                "Integer was given. ");
    default:
        poco_bugcheck_msg("unrecognized parameter type");
        throw Poco::BugcheckException(); // to avoid compiler warning
    }

    Poco::Mutex::ScopedLock lock(mutex);
    paramValues[paramIndex] = Poco::Any(value);
    needApplication[paramIndex] = true;

    if (immediateApply)
    	applyParameters();
}

template <> inline
void ParameterizedEntity::setParameterValue<double>(std::string paramName, double value, bool immediateApply)
{
    size_t paramIndex = getParameterIndex(paramName);

    switch (paramSet[paramIndex].datatype)
    {
    case ParamItem::typeInteger:
        throw Poco::DataFormatException("getParameterValue",
                "The parameter " + paramName + " has integer type. "
                "Float was given. ");
    case ParamItem::typeFloat:
        break;
    case ParamItem::typeString:
        throw Poco::DataFormatException("getParameterValue",
                "The parameter " + paramName + " has string type. "
                "Float was given. ");
    default:
        poco_bugcheck_msg("unrecognized parameter type");
        throw Poco::BugcheckException(); // to avoid compiler warning
    }

    Poco::Mutex::ScopedLock lock(mutex);
    paramValues[paramIndex] = Poco::Any(value);
    needApplication[paramIndex] = true;

    if (immediateApply)
    	applyParameters();
}

/// ParameterizedEntity::getParameter specialization for string parameters
template <> inline
void ParameterizedEntity::setParameterValue<std::string>(std::string paramName, std::string value, bool immediateApply)
{
    size_t paramIndex = getParameterIndex(paramName);

    switch (paramSet[paramIndex].datatype)
    {
    case ParamItem::typeInteger:
        throw Poco::DataFormatException("getParameterValue",
                "The parameter " + paramName + " has integer type. "
                "String was given. ");
    case ParamItem::typeFloat:
        throw Poco::DataFormatException("getParameterValue",
                "The parameter " + paramName + " has float type. "
                "String was given. ");
    case ParamItem::typeString:
        break;
    default:
        poco_bugcheck_msg("unrecognized parameter type");
        throw Poco::BugcheckException(); // to avoid compiler warning
    }

    Poco::Mutex::ScopedLock lock(mutex);
    paramValues[paramIndex] = Poco::Any(value);
    needApplication[paramIndex] = true;

    if (immediateApply)
    	applyParameters();
}

//
// default templates - shouldn't be called!
//

template <typename T> inline
T ParameterizedEntity::getParameterValue(size_t paramIndex)
{
    // report a bug to the developer
    poco_bugcheck_msg("ParameterizedEntity::getParameterValue<T>(): Wrong type T. ");
    throw Poco::BugcheckException();
}

template <typename T> inline
void ParameterizedEntity::setParameterValue(std::string paramName, T value, bool immediateApply)
{
    // report a bug to the developer
    poco_bugcheck_msg("ParameterizedEntity::setParameterValue<T>(): Wrong type T. ");
    throw Poco::BugcheckException();
}



