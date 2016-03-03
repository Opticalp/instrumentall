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

OutPort::OutPort(Module* parent,
        std::string name,
        std::string description,
        DataItem::DataTypeEnum datatype,
        size_t index):
    Port(parent, name, description, datatype, index),
    data(datatype)
{
    // notify the DataManager of the creation
    Poco::Util::Application::instance()
                            .getSubsystem<DataManager>()
                            .addOutPort(this);
}

std::vector<SharedPtr<InPort*> > OutPort::getTargetPorts()
{
    std::vector<SharedPtr<InPort*> > list;

    targetPortsLock.readLock();
    list = targetPorts;
    targetPortsLock.unlock();

    return list;
}

std::vector<SharedPtr<InPort*> > OutPort::getSeqTargetPorts()
{
    std::vector<SharedPtr<InPort*> > list;

    seqTargetPortsLock.readLock();
    list = seqTargetPorts;
    seqTargetPortsLock.unlock();

    return list;
}

void OutPort::addTargetPort(InPort* port)
{
    Poco::ScopedRWLock lock(targetPortsLock, true);

    SharedPtr<InPort*> sharedPort =
        Poco::Util::Application::instance()
                    .getSubsystem<Dispatcher>()
                    .getInPort(port);
    targetPorts.push_back(sharedPort);
}

void OutPort::removeTargetPort(InPort* port)
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

void OutPort::addSeqTargetPort(InPort* port)
{
    Poco::ScopedRWLock lock(seqTargetPortsLock, true);

    SharedPtr<InPort*> sharedPort =
        Poco::Util::Application::instance()
                    .getSubsystem<Dispatcher>()
                    .getInPort(port);
        seqTargetPorts.push_back(sharedPort);
}

void OutPort::removeSeqTargetPort(InPort* port)
{
    Poco::ScopedRWLock lock(seqTargetPortsLock, true);

    for (std::vector< SharedPtr<InPort*> >::iterator it=seqTargetPorts.begin(),
            ite=seqTargetPorts.end(); it != ite; it++ )
    {
        if (**it==port)
        {
            seqTargetPorts.erase(it);
            return;
        }
    }
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

//template<typename T>
//bool OutPort::tryData(T*& pData)
//{
//    return data.tryGetDataToWrite<T>(pData);
//}

void OutPort::notifyReady(DataAttributeOut attribute)
{
    if (attribute.isStartSequence())
    {
        seqTargetPortsLock.readLock();

        for( std::vector< SharedPtr<InPort*> >::iterator it = seqTargetPorts.begin(),
                ite = seqTargetPorts.end(); it != ite; it++ )
            attribute.appendStartSeqPortTarget(**it);

        seqTargetPortsLock.unlock();
    }

    if (attribute.isEndSequence())
    {
        seqTargetPortsLock.readLock();

        for( std::vector< SharedPtr<InPort*> >::iterator it = seqTargetPorts.begin(),
                ite = seqTargetPorts.end(); it != ite; it++ )
            attribute.appendEndSeqPortTarget(**it);

        seqTargetPortsLock.unlock();
    }

    dataItem()->setDataAttribute(attribute);
    dataItem()->releaseData();

    Poco::Util::Application::instance()
                        .getSubsystem<Dispatcher>()
                        .setOutPortDataReady(this);
}
