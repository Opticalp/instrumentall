/**
 * @file	src/Dispatcher.h
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

#ifndef SRC_DISPATCHER_H_
#define SRC_DISPATCHER_H_

#include "VerboseEntity.h"

#include "InPort.h"
#include "InDataPort.h"
#include "TrigPort.h"
#include "OutPort.h"

#include "Poco/Util/Subsystem.h"
#include "Poco/RWLock.h"
#include "Poco/SharedPtr.h"
#include "Poco/TaskManager.h"

#include <vector>

using Poco::RWLock;
using Poco::SharedPtr;

class Module;
class ModuleTask;
class DataSource;

/**
 * Dispatcher
 *
 * Manage the interactions between modules.
 * Every interaction between @ref Module should pass by the dispatcher.
 */
class Dispatcher: public Poco::Util::Subsystem, VerboseEntity
{
public:
    Dispatcher();
    virtual ~Dispatcher();

    /**
      * Subsystem name
      *
      * Shall not contain spaces: see setLogger() call in initialize().
      */
     const char * name() const { return "Dispatcher"; }

     /// @name un/re/initialization methods
     ///@{
     /**
      * Initialize the ports lists
      *
      * interrogate the ModuleManager about the active modules,
      * and populate allInPorts and allOutPorts
      *
      * @note The ModuleManager has to be initialized before the Dispatcher.
      */
     void initialize(Poco::Util::Application& app);

     // void reinitialize(Application & app); // not needed. By default: uninit, then init.

     /**
      * Reset the ports lists
      *
      * Set the empty ports for each port. It will reset all the connections.
      */
     void uninitialize();
     ///@}

     /**
      * Add the ports of a new Module
      *
      * append the new ports (input and output) to their respective lists:
      * allInPorts and allOutPorts
      */
     void addModule(SharedPtr<Module*> module);

     /**
      * Remove the ports of the given Module
      *
      * from the allInPorts list and from the allOutPorts list
      */
     void removeModule(SharedPtr<Module*> module);

     /**
      * Get the shared pointer corresponding to the given input port
      */
     SharedPtr<InPort*> getInPort(InPort* port);

     /**
      * Get the shared pointer corresponding to the given output port
      */
     SharedPtr<OutPort*> getOutPort(OutPort* port);

//     /**
//      * Return the address of the empty inPort
//      */
//     InDataPort* getEmptyInPort() { return &emptyInPort; }

     /**
      * Return the address of the empty outPort
      */
     OutPort* getEmptyOutPort() { return &emptyOutPort; }

     /**
      * Create a connection between two ports
      *
      * @throw Poco::Exception that is forwarded
      * from @ref getInPort. It is issued if the port is deleted
      * during the binding.
      * @note If the target port expired during the binding,
      * no exception is thrown, but the binding is not made.
      */
     void bind (SharedPtr<OutPort*> source, SharedPtr<InPort*> target);

     /**
      * Remove a connection between two ports
      *
      * No exception is thrown
      */
     void unbind(SharedPtr<InPort*> target);

     /**
      * Create a sequence combination connection between two ports
      *
      * @throw Poco::Exception that is forwarded
      * from @ref getInPort. It is issued if the port is deleted
      * during the binding.
      * @note If the target port expired during the binding,
      * no exception is thrown, but the binding is not made.
      */
     void seqBind (SharedPtr<OutPort*> source, SharedPtr<InPort*> target);

     /**
      * Remove a seq combination connection between two ports
      *
      * No exception is thrown
      */
     void seqUnbind(SharedPtr<InPort*> target);

     /**
      * Function to be called when new data is ready on an OutPort
      *
      *  - Translate the DataAttributeOut attribute into a DataAttribute
      *  by checking the sequence targets
      *  - readLock the data for every target InPort
      *  - notify the Modules corresponding to the target Ports (push)
      */
     void setOutPortDataReady(OutPort* port);

     /**
      * Dispatching function for module reset when a task failed.
      *
      *  * called by Module::resetWithTargets via DataSource::ResetTargets
      *  * call Module::resetWithTargets
      *  * Module::reseting flag avoids the recursions
      */
     void dispatchTargetReset(DataSource* port);

     /**
      * Lock the input ports
      *
      * to avoid its usage (mainly InPort::tryLock)
      * during the data update.
      *
      *  - To be called by the OutPort when new data is ready on an OutPort
      *  - the lock is released on new data
      *
      * @see OutPort::notifyReady
      * @see InPort::setNew
      */
     void lockInPorts(OutPort* port);

     /**
      * Launch a module task
      *
      * to be called by the UI or by any mean, but without new
      * input port data.
      *
      * @return The task created for the module to run. The
      * task can be used to check the state of the execution.
      */
     Poco::AutoPtr<ModuleTask> runModule(SharedPtr<Module*> ppModule);

     /**
      * Launch a module task
      *
      * Direct version: to be called by the module itself.
      * E.g. following a parameter set
      */
     void runModule(Module* pModule);

     /**
      * Enqueue a new module task
      *
      * and launch it if possible.
      * Register the task in the Module
      */
     void enqueueModuleTask(ModuleTask* pTask);

private:
     /**
      * Remove a port from the allInPorts list
      *
      * inPortsLock has to be locked from outside this function.
      *
      * Typical use is to delete many ports (see @ref uninitialize
      * or @ref removeModule ). It means that the lock is acquired,
      * then removeInPort (this function) is called multiple times,
      * or possibly removeOutPort also, and then the lock is released.
      *
      * No exception raising.
      */
     void removeInPort(InPort* port);

     /**
      * Add a port to the allInPorts list
      *
      * inPortsLock has to be locked from outside this function.
      * @see removeInPort discussion
      */
     void addInPort(InPort* port);

     /**
      * Remove a port from the allInPorts list
      *
      * @see removeInPort
      */
     void removeOutPort(OutPort* port);

     /**
      * Add a port to the allOutPorts list
      *
      * @see addOutPort
      */
     void addOutPort(OutPort* port);

     /// input ports to be used as targets for output ports
     std::vector< SharedPtr<InPort*> > allInPorts;
     RWLock inPortsLock; ///< lock for the transactions on allInPorts

     /// output ports to be used as sources for input ports
     std::vector< SharedPtr<OutPort*> > allOutPorts;
     RWLock outPortsLock; ///< lock for the transactions on allOutPorts

     /// Flag to decide if addModule adds a newly created module or not
     bool initialized;

     /// empty input port to be used when an input port is deleted
     InDataPort emptyInPort;
     /// empty input port to be used when an input port is deleted
     TrigPort emptyTrigPort;
     /// empty output port to be used when an output port is deleted
     OutPort emptyOutPort;
};

#endif /* SRC_DISPATCHER_H_ */
