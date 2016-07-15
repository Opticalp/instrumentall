/**
 * @file	src/OutPort.cpp
 * @date	feb. 2016
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

#include "OutPort.h"

#include "ModuleManager.h"
#include "Dispatcher.h"
#include "DataManager.h"
#include "DataLogger.h"

OutPort::OutPort(Module* parent,
        std::string name,
        std::string description,
        int datatype,
        size_t index):
    Port(parent, name, description, datatype, index),
	SeqSource(datatype)
{
//    // notify the DataManager of the creation
//    Poco::Util::Application::instance()
//                            .getSubsystem<DataManager>()
//                            .addOutPort(this);
}

OutPort::OutPort():
        Port(Poco::Util::Application::instance()
                    .getSubsystem<ModuleManager>()
                    .getEmptyModule(),
                "emptyOut", "replace an expired port",
                DataItem::typeUndefined, 0)
{
    // nothing to do
}

void OutPort::notifyReady(DataAttributeOut attribute)
{
    if (attribute.isSettingSequence())
    {
        seqTargetPortsLock.readLock();

        for( std::vector< SharedPtr<InPort*> >::iterator it = seqTargetPorts.begin(),
                ite = seqTargetPorts.end(); it != ite; it++ )
            attribute.appendSeqPortTarget(reinterpret_cast<InDataPort*>(**it));

        seqTargetPortsLock.unlock();
    }

    Poco::Util::Application::instance()
                        .getSubsystem<Dispatcher>()
                        .lockInPorts(this);

    setDataAttribute(attribute);
    releaseNewData();

    Poco::Util::Application::instance()
                        .getSubsystem<Dispatcher>()
                        .setOutPortDataReady(this);
}

std::set<SharedPtr<DataLogger*> > OutPort::loggers()
{
    std::set<SharedPtr<DataLogger*> > tmpList;

    loggersLock.readLock();

    for (std::set< DataLogger* >::iterator it = allLoggers.begin(),
            ite = allLoggers.end(); it != ite; it++ )
    {
        tmpList.insert( Poco::Util::Application::instance()
                            .getSubsystem<DataManager>()
                            .getDataLogger(*it) );
    }

    loggersLock.unlock();

    return tmpList;
}

void OutPort::registerLogger(DataLogger* logger)
{
    loggersLock.writeLock();
    allLoggers.insert(logger);
    loggersLock.unlock();
}

OutPort::~OutPort()
{
	while (allLoggers.size())
		(*allLoggers.begin())->detach();
}

void OutPort::detachLogger(DataLogger* logger)
{
    loggersLock.writeLock();
    allLoggers.erase(logger);
    loggersLock.unlock();
}
