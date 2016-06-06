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

    if ((*caughts)[portIndex])
        poco_bugcheck_msg("try to re-lock an output port that was already locked? ");


    bool retValue = outPort->tryLock();

    if (retValue)
    	(*caughts)[portIndex] = true;

    return retValue;
}

void OutPortUser::notifyOutPortReady(size_t portIndex,
		DataAttributeOut attribute)
{
    if (!(*caughts)[portIndex])
        poco_bugcheck_msg("try to unlock an output port "
                "that was not already locked? ");

    outPorts[portIndex]->notifyReady(attribute);

    (*caughts)[portIndex] = false;
}

void OutPortUser::notifyAllOutPortReady(DataAttributeOut attribute)
{
    for (size_t index = 0; index < outPorts.size(); index++)
    	if ((*caughts)[index])
    		notifyOutPortReady(index, attribute);
}

void OutPortUser::releaseAllOutPorts()
{
    for (size_t index = 0; index < outPorts.size(); index++)
        if ((*caughts)[index])
            outPorts[index]->releaseOnFailure();
}

void OutPortUser::reserveOutPorts(std::set<size_t> outputs)
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
				std::set<size_t>::iterator itTmp = it++;
				outputs.erase(itTmp);
			}
			else
			{
				it++;
			}
		}

		if (yield())
			throw Poco::RuntimeException("reserveOutPorts",
					"Task cancellation upon user request");

//		poco_information(logger(),"Not all output ports caught. Retrying...");
	}

	setRunningState(oldState);
}

void OutPortUser::reserveOutPort(size_t output)
{
	ModuleTask::RunningStates oldState = getRunningState();
	setRunningState(ModuleTask::retrievingOutDataLocks);

	while (!tryOutPortLock(output))
	{
		if (yield())
			throw Poco::RuntimeException("reserveOutPort",
					"Task cancellation upon user request");

//		poco_information(logger(),"Output port not caught. Retrying...");
	}

	setRunningState(oldState);
}

OutPortUser::~OutPortUser()
{
    for (std::vector<OutPort*>::iterator it=outPorts.begin(), ite=outPorts.end();
            it!=ite; it++)
        delete *it;
}

