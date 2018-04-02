/**
 * @file	src/core/DataLogger.cpp
 * @date	Mar 2016
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

#include "DataLogger.h"

#include "ThreadManager.h"

#include "Poco/Util/Application.h"
#include "Poco/NumberFormatter.h"

void DataLogger::runTarget()
{
	Poco::Util::Application::instance()
		.getSubsystem<ThreadManager>()
		.startDataLogger(this);
}

void DataLogger::run()
{
	if (!tryCatchSource())
		poco_bugcheck_msg((name() + ": not able to catch the source").c_str());

	lockSource();

	try
	{
		log();
	}
	catch (...)
	{
		releaseInputData();
		throw;
	}

	releaseInputData();
}

void DataLogger::setName(size_t refCount)
{
    if (className.empty())
        poco_bugcheck_msg("trying to set the name of the data logger "
                "but its class name is empty");

    mName = className;
    if (refCount)
        mName += Poco::NumberFormatter::format(refCount);
}

void DataLogger::setName(std::string newName)
{
    mName = newName;
    std::string identifier = "dataLogger." + getClassName() + "." + newName;
    setLogger(identifier);
    setPrefixKey(identifier);

    setAllParametersFromDefault();
}
