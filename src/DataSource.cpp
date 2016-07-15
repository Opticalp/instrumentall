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

std::vector<SharedPtr<InPort*> > DataSource::getTargetPorts()
{
    std::vector<SharedPtr<InPort*> > list;

    targetPortsLock.readLock();
    list = targetPorts;
    targetPortsLock.unlock();

    return list;
}

void DataSource::addTargetPort(InPort* port)
{
    Poco::ScopedRWLock lock(targetPortsLock, true);

    SharedPtr<InPort*> sharedPort =
        Poco::Util::Application::instance()
                    .getSubsystem<Dispatcher>()
                    .getInPort(port);
    targetPorts.push_back(sharedPort);
}

void DataSource::expire()
{
	expired = true;

	// forward the expiration
    targetPortsLock.readLock();

    for( std::vector< SharedPtr<InPort*> >::iterator it = targetPorts.begin(),
            ite = targetPorts.end(); it != ite; it++ )
        (**it)->parent()->expireOutData();

    targetPortsLock.unlock();
}

void DataSource::removeTargetPort(InPort* port)
{
    Poco::ScopedRWLock lock(targetPortsLock, true);

    for (std::vector< SharedPtr<InPort*> >::iterator it=targetPorts.begin(),
            ite=targetPorts.end(); it != ite; it++ )
    {
        if (**it==port)
        {
            targetPorts.erase(it);
            return;
        }
    }
}
