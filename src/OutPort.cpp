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

OutPort::OutPort(Module* parent,
        std::string name,
        std::string description,
        dataTypeEnum datatype,
        size_t index):
    Port(parent, name, description, datatype, index)
{
    // nothing to do
}

std::vector<SharedPtr<InPort*> > OutPort::getTargetPorts()
{
    std::vector<SharedPtr<InPort*> > list;

    lock.readLock();
    list = targetPorts;
    lock.unlock();

    return list;
}

void OutPort::addTargetPort(InPort* port)
{
    lock.writeLock();

    try
    {
        SharedPtr<InPort*> sharedPort =
            Poco::Util::Application::instance()
                        .getSubsystem<Dispatcher>()
                        .getInPort(port);
        targetPorts.push_back(sharedPort);
        lock.unlock();
    }
    catch (DispatcherException& e)
    {
        lock.unlock();
        e.rethrow();
    }
}

void OutPort::removeTargetPort(InPort* port)
{
    lock.writeLock();
    for (std::vector< SharedPtr<InPort*> >::iterator it=targetPorts.begin(),
            ite=targetPorts.end(); it != ite; it++ )
    {
        if (**it==port)
        {
            targetPorts.erase(it);
            lock.unlock();
            return;
        }
    }
    lock.unlock();
}

OutPort::OutPort():
        Port(Poco::Util::Application::instance()
                    .getSubsystem<ModuleManager>()
                    .getEmptyModule(),
                "emptyOut", "replace an expired port",
                Port::typeUndefined, 0)
{
    // nothing to do
}