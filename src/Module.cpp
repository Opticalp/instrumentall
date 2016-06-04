/**
 * @file	src/Module.cpp
 * @date	dec. 2015
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2015 Ph. Renaud-Goud / Opticalp

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
#include "ModuleFactory.h"
#include "ModuleManager.h"
#include "ThreadManager.h"
#include "ModuleTask.h"

#include "TrigPort.h"
#include "InDataPort.h"
#include "OutPort.h"

#include "Poco/NumberFormatter.h"
#include "Poco/NumberParser.h"

std::vector<std::string> Module::names;
Poco::RWLock Module::namesLock;

void Module::notifyCreation()
{
    // if not EmptyModule, add this to module manager
    if (mParent)
        Poco::Util::Application::instance().getSubsystem<ModuleManager>().addModule(this);
}

Module::~Module()
{
	// TODO: tasks?

    // notify parent factory
    if (mParent)
    {
        mParent->removeChildModule(this);

        // notify module manager
        Poco::Util::Application::instance().getSubsystem<ModuleManager>().removeModule(this);
    }

    // delete ports
    for (std::vector<InPort*>::iterator it=inPorts.begin(), ite=inPorts.end();
            it!=ite; it++)
        delete *it;

    for (std::vector<OutPort*>::iterator it=outPorts.begin(), ite=outPorts.end();
            it!=ite; it++)
        delete *it;
}

#include "Poco/Exception.h"

void Module::setInternalName(std::string internalName)
{
    namesLock.writeLock();

    switch (checkName(internalName))
    {
    case nameOk:
        mInternalName = internalName;
        names.push_back(mInternalName);
        namesLock.unlock();
        return;
    case nameExists:
        namesLock.unlock();
        throw Poco::ExistsException("setInternalName",
                internalName + " already in use");
    case nameBadSyntax:
        namesLock.unlock();
        throw Poco::SyntaxException("setInternalName",
                "The name should only contain alphanumeric characters "
                "or \"-\", \"_\", \".\"");
    }
}

void Module::setCustomName(std::string customName)
{
    if (customName.empty() || customName.compare(internalName())==0)
    {
        mName = internalName();
        return;
    }

    namesLock.writeLock();

    switch (checkName(customName))
    {
    case nameOk:
        mName = customName;
        names.push_back(mName);
        namesLock.unlock();
        return;
    case nameExists:
        freeInternalName();
        namesLock.unlock();
        throw Poco::ExistsException("setCustomName",
                customName + " already in use");
    case nameBadSyntax:
        freeInternalName();
        namesLock.unlock();
        throw Poco::SyntaxException("setCustomName",
                "The name should only contain alphanumeric characters "
                "or \"-\", \"_\", \".\"");
    }
}

ModuleFactory* Module::parent()
{
    if (mParent)
        return mParent;
    else
        throw Poco::InvalidAccessException("parent",
                "This module has no valid parent factory");
}

void Module::addInPort(std::string name, std::string description,
        int dataType, size_t index)
{
    if (index<inPorts.size())
        inPorts[index] = new InDataPort(this, name, description, dataType, index);
    else
        poco_bugcheck_msg(("addInPort: wrong index "
                + Poco::NumberFormatter::format(index)).c_str());
}

void Module::addTrigPort(std::string name, std::string description,
        size_t index)
{
    if (index<inPorts.size())
        inPorts[index] = new TrigPort(this, name, description, index);
    else
        poco_bugcheck_msg(("addTrigPort: wrong index "
                + Poco::NumberFormatter::format(index)).c_str());
}

void Module::addOutPort(std::string name, std::string description,
        int dataType, size_t index)
{
    if (index<outPorts.size())
        outPorts[index] = new OutPort(this, name, description, dataType, index);
    else
        poco_bugcheck_msg(("addOutPort: wrong index "
                + Poco::NumberFormatter::format(index)).c_str());
}

#include "Poco/RegularExpression.h"

Module::NameStatus Module::checkName(std::string newName)
{
    // check syntax
    Poco::RegularExpression regex("^[0-9a-zA-Z\\._-]+$");
    if (!regex.match(newName))
        return nameBadSyntax;

    // check existance
    for (std::vector<std::string>::iterator it = names.begin(),
            ite = names.end(); it != ite; it++)
        if (it->compare(newName)==0)
            return nameExists;

    return nameOk;
}

void Module::freeInternalName()
{
    // verify that this is a module creation process
    if (!name().empty())
        return;

    for (std::vector<std::string>::reverse_iterator it = names.rbegin(),
            ite = names.rend(); it != ite; it++ )
    {
        if (it->compare(internalName())==0)
        {
            names.erase((it+1).base());
            return;
        }
    }
}

void Module::addParameter(size_t index, std::string name, std::string descr, ParamItem::ParamType datatype)
{
    try
    {
        paramSet.at(index).name = name;
        paramSet[index].descr = descr;
        paramSet[index].datatype = datatype;
    }
    catch (std::out_of_range& )
    {
        poco_bugcheck_msg("addParameter: incorrect index. "
                "Please check your module constructor");
    }
}

void Module::addParameter(size_t index, std::string name, std::string descr,
        ParamItem::ParamType datatype, std::string hardCodedValue)
{
    addParameter(index, name, descr, datatype);
    hardCodedValues.insert(std::pair<size_t, std::string>(index, hardCodedValue));
}

std::string Module::getParameterDefaultValue(size_t index)
{
    // 1. check in the conf file
    std::string keyStr = "module." + name()
            + "." + paramSet.at(index).name;

    try
    {
        return appConf().getString(keyStr);
    }
    catch (Poco::NotFoundException&)
    {
        poco_information(logger(),
            std::string("property key: ") + keyStr +
            std::string(" not found in config file"));
    }

    // 2. check in the hard coded values
    std::map<size_t, std::string>::iterator it = hardCodedValues.find(index);
    if (it != hardCodedValues.end())
        return it->second;

    throw Poco::NotFoundException("getParameterDefaultValue",
                                        "no default value found");
}

long Module::getIntParameterDefaultValue(size_t index)
{
    std::string strValue = getParameterDefaultValue(index);
    return static_cast<long>(Poco::NumberParser::parse64(strValue));
}

double Module::getFloatParameterDefaultValue(size_t index)
{
    std::string strValue = getParameterDefaultValue(index);
    return Poco::NumberParser::parseFloat(strValue);
}

std::string Module::getStrParameterDefaultValue(size_t index)
{
    return getParameterDefaultValue(index);
}

void Module::getParameterSet(ParameterSet* pSet)
{
    // mainMutex.lock();
    pSet->clear();
    pSet->reserve(paramSet.size());
    for (ParameterSet::iterator it = paramSet.begin(),
            ite = paramSet.end(); it != ite; it++)
    {
        pSet->push_back(ParamItem());
        pSet->back().name = it->name;
        pSet->back().descr = it->descr;
        pSet->back().datatype = it->datatype;
    }
    // mainMutex.unlock();
}

size_t Module::getParameterIndex(std::string paramName)
{
    size_t length = paramSet.size();

    for (size_t index = 0; index < length; index++)
        if (paramSet[index].name.compare(paramName) == 0)
            return index;

    throw Poco::NotFoundException("getParameterIndex", "parameter name not found");
}

ParamItem::ParamType Module::getParameterType(std::string paramName)
{
    // TODO: mainMutex scoped lock
    return paramSet[getParameterIndex(paramName)].datatype;
}

InPort* Module::getInPort(std::string portName)
{
    for (std::vector<InPort*>::iterator it = inPorts.begin(),
            ite = inPorts.end(); it != ite; it++)
    {
        if (portName.compare((*it)->name()) == 0)
            return *it;
    }

    throw Poco::NotFoundException("getInPort",
            "port: " + portName + " not found "
            + "in module: " + name());
}

OutPort* Module::getOutPort(std::string portName)
{
    for (std::vector<OutPort*>::iterator it = outPorts.begin(),
            ite = outPorts.end(); it != ite; it++)
    {
        if (portName.compare((*it)->name()) == 0)
            return *it;
    }

    throw Poco::NotFoundException("getOutPort",
            "port: " + portName + " not found "
            + "in module: " + name());
}

int Module::startCondition(InPortLockUnlock& inPortsAccess)
{
	if (inPorts.size() == 0)
		return noDataStartState;

	bool nakedCall = false;

	if ((*runningTask)->trigPort() == NULL)
		nakedCall = true;

	// we would need a mean to check if held data is available without locking it
	// ... the inPortsAccess should handle that in fact...

	bool allPresent = false;
	bool zeroPresent = true;
	while (!allPresent)
	{
		allPresent = true;
		for (size_t port = 0; port < inPorts.size(); port++)
		{
			if (inPortsAccess.caught(port))
				continue;

			if (inPortsAccess.tryLock(port))
				zeroPresent = false;
			else
				allPresent = false;
		}

		if (nakedCall)
			break;

		if (!allPresent)
		{
			if (yield())
				return cancelledStartState;
		}
	}

	if (allPresent)
		return allDataStartState;

	if (zeroPresent)
		return noDataStartState;
	else
		return unknownStartState;
}

void Module::run()
{
    // try to acquire the mutex
    while (!runTaskMutex.tryLock(TIME_LAPSE))
    {
        if (isCancelled())
            return;
    }

	expireOutData();

	// scoped-lock-like objects for ports
	InPortLockUnlock inPortAccess(getInPorts());
	OutPortLockUnlock outPortAccess(getOutPorts());

    try
    {
        process(inPortAccess, outPortAccess);
    }
    catch (Poco::Exception& e)
    {
        // release input ports data -- even if new --,
        // since the module task exited on error
        inPortAccess.processing();

        runTaskMutex.unlock();
        e.rethrow();
    }

    runTaskMutex.unlock();

    // outPortAccess and inPortAccess are destroyed here...
}

bool Module::sleep(long Milliseconds)
{
	return (*runningTask)->sleep(Milliseconds);
}

bool Module::yield()
{
	return (*runningTask)->yield();
}

void Module::setProgress(float progress)
{
	(*runningTask)->setProgress(progress);
}

bool Module::isCancelled()
{
	return (*runningTask)->isCancelled();
}

void Module::expireOutData()
{
    for (std::vector<OutPort*>::iterator it = outPorts.begin(),
            ite = outPorts.end(); it != ite; it++)
        (*it)->expire();
}

bool Module::enqueueTask(ModuleTask* task)
{
	Poco::FastMutex::ScopedLock lock(taskMngtMutex);

	Poco::Util::Application::instance()
				             .getSubsystem<ThreadManager>()
				             .registerNewModuleTask(task);

	allTasks.insert(task);
	taskQueue.push_back(task);

	// TODO: FIXME (transitional return value)
	return true;
}

void Module::popTask()
{
	Poco::ScopedLockWithUnlock<Poco::FastMutex> lock(taskMngtMutex);

	ModuleTask* nextTask = taskQueue.front();

	// poco_information(logger(), "poping out the next task: " + nextTask->name());
	taskQueue.pop_front();

	lock.unlock();

	Poco::Util::Application::instance()
			             .getSubsystem<ThreadManager>()
			             .startModuleTask(nextTask);
}

void Module::popTaskSync()
{
	Poco::ScopedLockWithUnlock<Poco::FastMutex> lock(taskMngtMutex);

	ModuleTask* nextTask = taskQueue.front();
	taskQueue.pop_front();

	lock.unlock();

	Poco::Util::Application::instance()
			             .getSubsystem<ThreadManager>()
			             .startSyncModuleTask(nextTask);
}

void Module::unregisterTask(ModuleTask* task)
{
	Poco::FastMutex::ScopedLock lock(taskMngtMutex);

	allTasks.erase(task);
}

void Module::mergeTasks(std::set<int> inPortIndexes)
{
	Poco::FastMutex::ScopedLock lock(taskMngtMutex);

	for (std::set<int>::iterator it = inPortIndexes.begin(),
			ite = inPortIndexes.end(); it != ite; it++)
	{
		InPort* trigPort = inPorts.at(*it);

		if ((*runningTask)->trigPort() == trigPort)
			continue; // current task

		bool found = false;

		// seek
		for (std::list<ModuleTask*>::iterator qIt = taskQueue.begin(),
				qIte = taskQueue.end(); qIt != qIte; qIt++)
		{
			if ((*qIt)->trigPort() == trigPort)
			{
				found = true;
				(*runningTask)->merge(*qIt);
				taskQueue.erase(qIt);
				break;
			}
		}

		if (!found)
			poco_warning(logger(),
					"Unable to merge the task for " + trigPort->name());
	}
}
