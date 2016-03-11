/**
 * Module class specialized templates implementation
 * 
 * @file	src/Module.ipp
 * @date	10 mars 2016
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


#include "Module.h"

template <> inline
long Module::getParameterValue<long>(std::string paramName)
{
    // TODO: scoped mainMutex

    size_t paramIndex = getParameterIndex(paramName);

    switch (paramSet[paramIndex].datatype)
    {
    case ParamItem::typeInteger:
        return getIntParameterValue(paramIndex);
    case ParamItem::typeFloat:
        throw Poco::DataFormatException("getParameterValue",
                "The parameter " + paramName + " has float type. "
                "Integer was requested. ");
    case ParamItem::typeString:
        throw Poco::DataFormatException("getParameterValue",
                "The parameter " + paramName + " has string type. "
                "Integer was requested. ");
    default:
        poco_bugcheck_msg("unrecognized parameter type");
        throw Poco::BugcheckException(); // to avoid compiler warning
    }
}

template <> inline
double Module::getParameterValue<double>(std::string paramName)
{
    // TODO: scoped mainMutex

    size_t paramIndex = getParameterIndex(paramName);

    switch (paramSet[paramIndex].datatype)
    {
    case ParamItem::typeInteger:
        throw Poco::DataFormatException("getParameterValue",
                "The parameter " + paramName + " has integer type. "
                "Float was requested. ");
    case ParamItem::typeFloat:
        return getFloatParameterValue(paramIndex);
    case ParamItem::typeString:
        throw Poco::DataFormatException("getParameterValue",
                "The parameter " + paramName + " has string type. "
                "Float was requested. ");
    default:
        poco_bugcheck_msg("unrecognized parameter type");
        throw Poco::BugcheckException(); // to avoid compiler warning
    }
}

/// Module::getParameter specialization for string parameters
template <> inline
std::string Module::getParameterValue<std::string>(std::string paramName)
{
    // TODO: scoped mainMutex

    size_t paramIndex = getParameterIndex(paramName);

    switch (paramSet[paramIndex].datatype)
    {
    case ParamItem::typeInteger:
        throw Poco::DataFormatException("getParameterValue",
                "The parameter " + paramName + " has integer type. "
                "String was requested. ");
    case ParamItem::typeFloat:
        throw Poco::DataFormatException("getParameterValue",
                "The parameter " + paramName + " has float type. "
                "String was requested. ");
    case ParamItem::typeString:
        return getStrParameterValue(paramIndex);
    default:
        poco_bugcheck_msg("unrecognized parameter type");
        throw Poco::BugcheckException(); // to avoid compiler warning
    }
}

template <> inline
void Module::setParameterValue<long>(std::string paramName, long value)
{
    // TODO: scoped mainMutex

    size_t paramIndex = getParameterIndex(paramName);

    switch (paramSet[paramIndex].datatype)
    {
    case ParamItem::typeInteger:
        setIntParameterValue(paramIndex, value);
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
}

template <> inline
void Module::setParameterValue<double>(std::string paramName, double value)
{
    // TODO: scoped mainMutex

    size_t paramIndex = getParameterIndex(paramName);

    switch (paramSet[paramIndex].datatype)
    {
    case ParamItem::typeInteger:
        throw Poco::DataFormatException("getParameterValue",
                "The parameter " + paramName + " has integer type. "
                "Float was given. ");
    case ParamItem::typeFloat:
        setFloatParameterValue(paramIndex, value);
        break;
    case ParamItem::typeString:
        throw Poco::DataFormatException("getParameterValue",
                "The parameter " + paramName + " has string type. "
                "Float was given. ");
    default:
        poco_bugcheck_msg("unrecognized parameter type");
        throw Poco::BugcheckException(); // to avoid compiler warning
    }
}

/// Module::getParameter specialization for string parameters
template <> inline
void Module::setParameterValue<std::string>(std::string paramName, std::string value)
{
    // TODO: scoped mainMutex

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
        setStrParameterValue(paramIndex, value);
        break;
    default:
        poco_bugcheck_msg("unrecognized parameter type");
        throw Poco::BugcheckException(); // to avoid compiler warning
    }
}

//
// default templates - shouldn't be called!
//

template <typename T> inline
T Module::getParameterValue(std::string paramName)
{
    // report a bug to the developer
    poco_bugcheck_msg("Module::getParameterValue<T>(): Wrong type T. ");
    throw Poco::BugcheckException();
}

template <typename T> inline
void Module::setParameterValue(std::string paramName, T value)
{
    // report a bug to the developer
    poco_bugcheck_msg("Module::setParameterValue<T>(): Wrong type T. ");
    throw Poco::BugcheckException();
}
