/**
 * @file	src/OutPortUser.cpp
 * @date	June 2016
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

#include "OutPortUser.h"

#include "Dispatcher.h"

#include "Poco/Util/Application.h"
#include "Poco/NumberFormatter.h"

void OutPortUser::addOutPort(Module* parent,
		std::string name, std::string description,
        int dataType, size_t index)
{
    if (index<outPorts.size())
        outPorts[index] = new OutPort(parent, name, description, dataType, index);
    else
        poco_bugcheck_msg(("addOutPort: wrong index "
                + Poco::NumberFormatter::format(index)).c_str());
}

OutPort* OutPortUser::getOutPort(std::string portName)
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

bool OutPortUser::tryOutPortLock(size_t portIndex)
{
    OutPort* outPort = outPorts.at(portIndex);

    if (isOutPortCaught(portIndex))
        poco_bugcheck_msg("try to re-lock an output port that was already locked? ");

    if (outPort->tryWriteDataLock())
    {
    	caughts->insert(portIndex);
    	return true;
    }
    else
    {
    	return false;
    }
}

void OutPortUser::notifyOutPortReady(size_t portIndex,
		DataAttributeOut attribute)
{
    if (!isOutPortCaught(portIndex))
        poco_bugcheck_msg("try to unlock an output port "
                "that was not previously locked? ");

    if (isCancelled())
    	throw ExecutionAbortedException("notify out port ready, "
    			"although the task is cancelled. abort. ");

    outPorts[portIndex]->notifyReady(attribute);

    caughts->erase(portIndex);

	if (caughts->empty())
		unlockOut();
}

void OutPortUser::notifyAllOutPortReady(DataAttributeOut attribute)
{
	if (caughts->empty())
		return;

	for (std::set<size_t>::iterator it = caughts->begin(),
			ite = caughts->end(); it != ite; )
	{
		size_t port = *it++;
		notifyOutPortReady(port, attribute);
	}

	poco_assert(caughts->empty());
	unlockOut();
}

void OutPortUser::releaseAllOutPorts()
{
	if (caughts->empty())
		return;

	for (std::set<size_t>::iterator it = caughts->begin(),
			ite = caughts->end(); it != ite; it++)
	{
		poco_warning(logger(), outPorts[*it]->name()
				+ " was not cleanly released");
        outPorts[*it]->releaseWriteOnFailure();
	}

	caughts->clear();
	unlockOut();
}

void OutPortUser::reserveAllOutPorts()
{
	std::set<size_t> allPorts;

	for (size_t ind = 0; ind < outPorts.size(); ind++)
		allPorts.insert(ind);

	reserveOutPorts(allPorts);
}

void OutPortUser::reserveOutPorts(std::set<size_t> outputs)
{
	if (outputs.empty())
		return;

	bool releaseOut = false; // flag to check if lockOut should be released here in case of error
	if (caughts->empty())
	{
		reserveLockOut();
		releaseOut = true;
	}

	try
	{
		ModuleTask::RunningStates oldState = getRunningState();
		setRunningState(ModuleTask::retrievingOutDataLocks);

		while (!outputs.empty())
		{
			for (std::set<size_t>::iterator it = outputs.begin();
					it != outputs.end(); )
			{
				if (tryOutPortLock(*it))
				{
					releaseOut = false;
					std::set<size_t>::iterator itTmp = it++;
					outputs.erase(itTmp);
				}
				else
				{
					it++;
				}
			}

			if (yield())
				throw ExecutionAbortedException("reserveOutPorts",
						"Task cancellation upon user request");

			poco_information(logger(),"Not all output ports caught. Retrying...");
		}

		setRunningState(oldState);
	}
	catch (...)
	{
		if (releaseOut)
			unlockOut();
		throw;
	}
}

size_t OutPortUser::reserveOutPortOneOf(std::set<size_t>& outputs)
{
	if (outputs.empty())
		poco_bugcheck_msg("empty output port set to lock");

	bool releaseOut = false; // flag to check if lockOut should be released here in case of error
	if (caughts->empty())
	{
		reserveLockOut();
		releaseOut = true;
	}

	ModuleTask::RunningStates oldState = getRunningState();
	setRunningState(ModuleTask::retrievingOutDataLocks);

	do
	{
		for (std::set<size_t>::iterator it = outputs.begin(),
				ite = outputs.end(); it != ite; it++)
		{
			if (tryOutPortLock(*it))
			{
				releaseOut = false;
				size_t retValue = *it;
				outputs.erase(it);
				setRunningState(oldState);
				return retValue;
			}
		}
	}
	while (!yield());

	if (releaseOut)
		unlockOut();

	throw ExecutionAbortedException("reserveOutPortOneOf",
			"Task cancellation upon user request");
}

void OutPortUser::reserveOutPort(size_t output)
{
	bool releaseOut = false; // flag to check if lockOut should be released here in case of error
	if (caughts->empty())
	{
		reserveLockOut();
		releaseOut = true;
	}

	try
	{
		ModuleTask::RunningStates oldState = getRunningState();
		setRunningState(ModuleTask::retrievingOutDataLocks);

		while (!tryOutPortLock(output))
		{
			if (yield())
				throw ExecutionAbortedException("reserveOutPort",
						"Task cancellation upon user request");

	//		poco_information(logger(),"Output port not caught. Retrying...");
		}

		releaseOut = false;

		setRunningState(oldState);
	}
	catch (...)
	{
		if (releaseOut)
			unlockOut();
		throw;
	}
}

OutPortUser::~OutPortUser()
{
    for (std::vector<OutPort*>::iterator it=outPorts.begin(), ite=outPorts.end();
            it!=ite; it++)
        delete *it;
}

void OutPortUser::reserveLockOut()
{
	while (!tryLockOut())
	{
		if (yield())
			throw ExecutionAbortedException(name(), "task cancelled upon user request");
	}
}

void OutPortUser::cancelTargets()
{
    for (std::vector<OutPort*>::iterator it=outPorts.begin(), ite=outPorts.end();
            it!=ite; it++)
    	(*it)->cancelWithTargets();
}

void OutPortUser::resetTargets()
{
    for (std::vector<OutPort*>::iterator it=outPorts.begin(), ite=outPorts.end();
            it!=ite; it++)
    	(*it)->resetWithTargets();
}

