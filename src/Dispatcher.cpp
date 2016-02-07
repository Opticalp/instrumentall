/**
 * @file	src/Dispatcher.cpp
 * @date	Feb. 2016
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

#include "Dispatcher.h"

#include "ModuleManager.h"
#include "Module.h"

#include "InPort.h"
#include "OutPort.h"

Dispatcher::Dispatcher()
{
    // TODO Auto-generated constructor stub

}

Dispatcher::~Dispatcher()
{
    // TODO Auto-generated destructor stub
}

void Dispatcher::initialize(Poco::Util::Application& app)
{
    std::vector< SharedPtr<Module*> > modules;
    modules = Poco::Util::Application::instance().getSubsystem<ModuleManager>().getModules();

    for (std::vector< SharedPtr<Module*> >::iterator it=modules.begin(), ite=modules.end();
            it!=ite; it++)
        addModule(*it);
}

void Dispatcher::uninitialize()
{
    // remove all ports.
}


void Dispatcher::addModule(SharedPtr<Module*> module)
{
    // TODO

    // lock module
    // retrieve inPorts
    // retrieve outPorts

    inPortsLock.writeLock();
    outPortsLock.writeLock();
    // unlock module

    // write ports

    inPortsLock.unlock();
    outPortsLock.unlock();
}

void Dispatcher::removeModule(SharedPtr<Module*> module)
{
    // TODO

    // lock module
    // retrieve inPorts
    // retrieve outPorts

    inPortsLock.writeLock();
    outPortsLock.writeLock();
    // unlock module

    // write ports

    inPortsLock.unlock();
    outPortsLock.unlock();

}

SharedPtr<InPort*> Dispatcher::getInPort(InPort* port)
{
    inPortsLock.readLock();
    for (std::vector< SharedPtr<InPort*> >::iterator it=allInPorts.begin(), ite=allInPorts.end();
            it!=ite; it++)
    {
        if (port==**it)
        {
            inPortsLock.unlock();
            return *it;
        }
    }

    inPortsLock.unlock();
    throw ModuleException("getInPort", "port not found: "
            "Should have been deleted during the query");
}

SharedPtr<OutPort*> Dispatcher::getOutPort(OutPort* port)
{
    outPortsLock.readLock();
    for (std::vector< SharedPtr<OutPort*> >::iterator it=allOutPorts.begin(), ite=allOutPorts.end();
            it!=ite; it++)
    {
        if (port==**it)
        {
            outPortsLock.unlock();
            return *it;
        }
    }

    outPortsLock.unlock();
    throw ModuleException("getOutPort", "port not found: "
            "Should have been deleted during the query");
}
