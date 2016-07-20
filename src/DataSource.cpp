/**
 * detailed comment
 * 
 * @file	/Instrumentall-Debug@instru-git-debug/[Source directory]/src/DataSource.cpp
 * @brief	short comment
 * @date	15 juil. 2016
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

#include "DataSource.h"
#include "Module.h"

#include "Dispatcher.h"

#include "Poco/Util/Application.h"

DataSource::DataSource(int datatype):
		DataItem(datatype),
		expired(true)
{

}

DataSource::~DataSource()
{
    Poco::Util::Application::instance()
                        .getSubsystem<Dispatcher>()
						.unbind(this);
}

void DataSource::releaseNewData()
{
	expired = false;
    unlockData();
}

void DataSource::releaseBrokenData()
{
    expired = true;
    unlockData();
}

std::set<DataTarget*> DataSource::getDataTargets()
{
    Poco::Mutex::ScopedLock lock(targetLock);
    return dataTargets;
}

void DataSource::addDataTarget(DataTarget* target)
{
    Poco::Mutex::ScopedLock lock(targetLock);
    dataTargets.insert(target);
}

void DataSource::expire()
{
	expired = true;

//	// forward the expiration
//    targetLock.lock();
//
//    for( std::set<DataTargets*>::iterator it = dataTargets.begin(),
//            ite = dataTargets.end(); it != ite; it++ )
//        (**it)->parent()->expireOutData();
//
//    targetLock.unlock();
}

void DataSource::resetTargets()
{
    Poco::Util::Application::instance()
                        .getSubsystem<Dispatcher>()
                        .dispatchTargetReset(this);
}

void DataSource::removeDataTarget(DataTarget* target)
{
    Poco::Mutex::ScopedLock lock(targetLock);
    dataTargets.erase(target);
}
