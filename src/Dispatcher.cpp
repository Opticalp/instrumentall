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
#include "ThreadManager.h"
#include "Module.h"
#include "ModuleTask.h"

#include "InPort.h"
#include "InDataPort.h"
#include "OutPort.h"

#include "Poco/NumberFormatter.h"

Dispatcher::Dispatcher():
    VerboseEntity(name()),
    initialized(false),
    emptyOutPort(),
    emptyInPort(&emptyOutPort),
    emptyTrigPort(&emptyOutPort)
{
    // nothing else to do
}

Dispatcher::~Dispatcher()
{
    if (initialized)
        uninitialize();
}

void Dispatcher::initialize(Poco::Util::Application& app)
{
    if (initialized)
    {
        poco_error(logger(),"The dispatcher is already initialized, "
                "please uninitialize first");
    }

    setLogger(name());

    std::vector< SharedPtr<Module*> > modules;
    modules = Poco::Util::Application::instance().getSubsystem<ModuleManager>().getModules();

    initialized = true;

    for (std::vector< SharedPtr<Module*> >::iterator it=modules.begin(), ite=modules.end();
            it!=ite; it++)
        addModule(*it);

}

void Dispatcher::uninitialize()
{
    initialized = false;

    // poco_information(logger(), "Dispatcher un-initialization.");

    // TODO: data man un-init

    ThreadManager& threadMan = Poco::Util::Application::instance()
                                        .getSubsystem<ThreadManager>();
    size_t taskCnt = threadMan.count();
    if (taskCnt)
        poco_warning(logger(), "Dispatcher uninit: "
            + Poco::NumberFormatter::format(taskCnt)
            + " task(s) to stop");

    threadMan.cancelAll();
    threadMan.waitAll();

    inPortsLock.writeLock();
    outPortsLock.writeLock();

    while( allInPorts.size() )
        removeInPort(*allInPorts.back());

    while( allOutPorts.size() )
        removeOutPort(*allOutPorts.back());

    inPortsLock.unlock();
    outPortsLock.unlock();

    poco_information(logger(), "Dispatcher uninit OK. Locks released");
}


void Dispatcher::addModule(SharedPtr<Module*> module)
{
    if (!initialized)
    {
        // poco_information(logger(),
        //         "module insertion in the dispatcher: not initialized");
        return;
    }

    // n.b. the module is "locked" in the way that it cannot change
    // its connections, as long as the connection changes come from the
    // dispatcher, and the dispatcher has a write lock on the inPorts
    // and the outPorts.

    inPortsLock.writeLock();
    outPortsLock.writeLock();

    std::vector<InPort*> inPorts((*module)->getInPorts());
    std::vector<OutPort*> outPorts((*module)->getOutPorts());

    for (std::vector<InPort*>::iterator it = inPorts.begin(),
            ite = inPorts.end() ; it != ite ; it++)
        addInPort(*it);
    for (std::vector<OutPort*>::iterator it = outPorts.begin(),
            ite = outPorts.end() ; it != ite ; it++)
        addOutPort(*it);

    inPortsLock.unlock();
    outPortsLock.unlock();

    // poco_information(logger(),"module " + (*module)->name() + " added in the dispatcher");
}

void Dispatcher::removeModule(SharedPtr<Module*> module)
{
    poco_information(logger(),"removing module " + (*module)->name());

    if (!initialized)
    {
        // poco_information(logger(),
        //         "module removal from the dispatcher: not initialized");
        return;
    }

    inPortsLock.writeLock();
    outPortsLock.writeLock();

    std::vector<InPort*> inPorts((*module)->getInPorts());
    std::vector<OutPort*> outPorts((*module)->getOutPorts());

    // remove the input ports first in order to break the connections
    for (std::vector<InPort*>::reverse_iterator it = inPorts.rbegin(),
            ite = inPorts.rend() ; it != ite ; it++)
        removeInPort(*it);
    for (std::vector<OutPort*>::reverse_iterator it = outPorts.rbegin(),
            ite = outPorts.rend() ; it != ite ; it++)
        removeOutPort(*it);

    inPortsLock.unlock();
    outPortsLock.unlock();
}

SharedPtr<InPort*> Dispatcher::getInPort(InPort* port)
{
    if (!initialized)
        throw Poco::RuntimeException("getInPort",
                "The dispatcher is not initialized");

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
    throw Poco::NotFoundException("getInPort", "port not found: "
            "Should have been deleted during the query");
}

SharedPtr<OutPort*> Dispatcher::getOutPort(OutPort* port)
{
    if (!initialized)
        throw Poco::RuntimeException("getInPort",
                "The dispatcher is not initialized");

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
    throw Poco::NotFoundException("getOutPort", "port not found: "
            "Should have been deleted during the query");
}

void Dispatcher::removeInPort(InPort* port)
{
    // using a reverse iterator to improve performance when called
    // from the uninitializer
    for (std::vector< SharedPtr<InPort*> >::reverse_iterator it = allInPorts.rbegin(),
            ite = allInPorts.rend(); it!=ite; it++)
    {
        if (port == **it)
        {
            (**it)->releaseSourcePort(); // break the connection

            // replace the pointed factory by something throwing exceptions
            if ((**it)->isTrig())
                **it = &emptyTrigPort;
            else
            {
                reinterpret_cast<InDataPort*>(**it)->releaseSeqSourcePort();
                **it = &emptyInPort;
            }

            allInPorts.erase((it+1).base());
            // poco_information(logger(),
            //         port->name() + " input port from module "
            //         + port->parent()->name() + " is erased "
            //                 "from Dispatcher::allInPorts. ");
            return;
        }
    }

    poco_error(logger(), "removeInPort(): "
            "the port was not found");
}

void Dispatcher::addInPort(InPort* port)
{
    // poco_information(logger(),"adding port " + port->name()
    //         + " from module " + port->parent()->name());
    allInPorts.push_back(SharedPtr<InPort*>(new (InPort*)(port)));
}

void Dispatcher::removeOutPort(OutPort* port)
{
    // using a reverse iterator to improve performance when called
    // from the uninitializer
    for (std::vector< SharedPtr<OutPort*> >::reverse_iterator it = allOutPorts.rbegin(),
            ite = allOutPorts.rend(); it!=ite; it++)
    {
        if (port == **it)
        {
            // release connections (find targets and act on targets)
            // nb: this is a simple precaution, since all the connections
            // should already be broken by the deletion of the inPorts.
            std::vector< SharedPtr<InPort*> > targets;

            targets = port->getTargetPorts();
            for (std::vector< SharedPtr<InPort*> >::iterator tgtIt = targets.begin(),
                    tgtIte = targets.end() ; tgtIt != tgtIte ; tgtIt++)
                (**tgtIt)->releaseSourcePort();

            targets = port->getSeqTargetPorts();
            for (std::vector< SharedPtr<InPort*> >::iterator tgtIt = targets.begin(),
                    tgtIte = targets.end() ; tgtIt != tgtIte ; tgtIt++)
                reinterpret_cast<InDataPort*>(**tgtIt)->releaseSeqSourcePort();

            **it = &emptyOutPort; // replace the pointed factory by something throwing exceptions
            allOutPorts.erase((it+1).base());
            // poco_information(logger(),
            //         port->name() + " output port from module "
            //         + port->parent()->name() + " is erased "
            //                 "from Dispatcher::allInPorts. ");
            return;
        }
    }

    poco_error(logger(), "removeOutPort(): "
            "the port was not found");
}

void Dispatcher::addOutPort(OutPort* port)
{
    // poco_information(logger(),"adding port " + port->name()
    //         + " from module " + port->parent()->name());
    allOutPorts.push_back(SharedPtr<OutPort*>(new (OutPort*)(port)));
}

void Dispatcher::bind(SharedPtr<OutPort*> source, SharedPtr<InPort*> target)
{
    if (!(*target)->isTrig()
            && (*target)->dataType() != (*source)->dataType())
        throw Poco::DataFormatException("bind",
                "The source and target port data types must fit");

    (*target)->setSourcePort(source);
}

void Dispatcher::unbind(SharedPtr<InPort*> target)
{
    (*target)->releaseSourcePort();
}

void Dispatcher::seqBind(SharedPtr<OutPort*> source, SharedPtr<InPort*> target)
{
    if ((*target)->isTrig())
        throw Poco::RuntimeException("seqBind",
                "The target port is a trig port. "
                "A data sequence can not be bound to a trig port");

    reinterpret_cast<InDataPort*>(*target)->setSeqSourcePort(source);
}

void Dispatcher::seqUnbind(SharedPtr<InPort*> target)
{
    if ((*target)->isTrig())
        throw Poco::RuntimeException("seqBind",
                "The target port is a trig port. "
                "A data sequence can not be bound to a trig port");

    reinterpret_cast<InDataPort*>(*target)->releaseSeqSourcePort();
}

void Dispatcher::lockInPorts(OutPort* port)
{
    std::vector< SharedPtr<InPort*> > targetPorts = port->getTargetPorts();
    for ( std::vector< SharedPtr<InPort*> >::iterator it = targetPorts.begin(),
            ite = targetPorts.end(); it != ite; it++ )
        (**it)->newDataLock();
}

void Dispatcher::setOutPortDataReady(OutPort* port)
{
    // poco_information(logger(), port->name() + " data ready");

    std::vector< SharedPtr<InPort*> > targetPorts = port->getTargetPorts();
    for ( std::vector< SharedPtr<InPort*> >::iterator it = targetPorts.begin(),
            ite = targetPorts.end(); it != ite; it++ )
    {
        (**it)->setNew();

        // get module from module manager
        SharedPtr<Module*> shdMod = Poco::Util::Application::instance()
                                        .getSubsystem<ModuleManager>()
                                        .getModule((**it)->parent());

        poco_information(logger(),port->parent()->name() + " port " + port->name()
                + " STARTS " + (**it)->parent()->name() );

        enqueueModuleTask(new ModuleTask(*shdMod, **it));
    }
}

void Dispatcher::dispatchSeqTargetReset(OutPort* port)
{
    std::vector< SharedPtr<InPort*> > targetPorts = port->getSeqTargetPorts();
    for ( std::vector< SharedPtr<InPort*> >::iterator it = targetPorts.begin(),
            ite = targetPorts.end(); it != ite; it++ )
    {
    	poco_information(logger(), "forwarding module cancellation to "
    			+ (**it)->parent()->name());
    	(**it)->parent()->resetWithSeqTargets();
    }
}

Poco::AutoPtr<ModuleTask> Dispatcher::runModule(SharedPtr<Module*> ppModule)
{
    Poco::AutoPtr<ModuleTask> taskPtr(new ModuleTask(*ppModule), true);
    enqueueModuleTask(taskPtr);

    return taskPtr;
}

void Dispatcher::runModule(Module* pModule)
{
	enqueueModuleTask(new ModuleTask(pModule));
}

void Dispatcher::enqueueModuleTask(ModuleTask* pTask)
{
	pTask->module()->enqueueTask(pTask);
}
