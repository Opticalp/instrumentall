/**
 * @file	src/InPortUser.cpp
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

#include "InPortUser.h"

#include "TrigPort.h"
#include "InDataPort.h"

#include "Poco/NumberFormatter.h"

void InPortUser::addInPort(Module* parent,
		std::string name, std::string description,
        int dataType, size_t index)
{
    if (index<inPorts.size())
        inPorts[index] = new InDataPort(parent, name, description, dataType, index);
    else
        poco_bugcheck_msg(("addInPort: wrong index "
                + Poco::NumberFormatter::format(index)).c_str());
}

void InPortUser::addTrigPort(Module* parent,
		std::string name, std::string description,
        size_t index)
{
    if (index<inPorts.size())
        inPorts[index] = new TrigPort(parent, name, description, index);
    else
        poco_bugcheck_msg(("addTrigPort: wrong index "
                + Poco::NumberFormatter::format(index)).c_str());
}

InPort* InPortUser::getInPort(std::string portName)
{
    for (std::vector<InPort*>::iterator it = inPorts.begin(),
            ite = inPorts.end(); it != ite; it++)
    {
        if (portName.compare((*it)->name()) == 0)
            return *it;
    }

    throw Poco::NotFoundException("getInPort",
            "port: " + portName + " not found "
            + "in: " + name());
}

bool InPortUser::tryInPortLock(size_t portIndex)
{
    if ((*caughts)[portIndex])
        poco_bugcheck_msg("try to re-lock an input port that was already locked? ");

    if (inPorts[portIndex]->tryLock())
    {
    	(*caughts)[portIndex] = true;
    	return true;
    }
    else
    {
    	return false;
    }
}

void InPortUser::readInPortDataAttribute(size_t portIndex,
		DataAttributeIn* pAttr)
{
    if (!(*caughts)[portIndex])
    	poco_bugcheck_msg("try to read an input port that was not previously locked");

    inPorts[portIndex]->readDataAttribute(pAttr);
}

bool InPortUser::tryInPortDataAttribute(size_t portIndex,
        DataAttributeIn* pAttr)
{
    if (!inPorts.at(portIndex)->isTrig())
        poco_bugcheck_msg("The port at the given index is a data port");

    TrigPort* trigPort = reinterpret_cast<TrigPort*>(inPorts[portIndex]);

    if ((*caughts)[portIndex])
        poco_bugcheck_msg("try to re-lock an input port that was already locked? ");


    bool retValue = trigPort->tryDataAttribute(pAttr);

    if (retValue)
        (*caughts)[portIndex] = true;

    return retValue;
}

void InPortUser::releaseInPort(size_t portIndex)
{
    if ((*caughts)[portIndex])
        inPorts[portIndex]->release();

    (*caughts)[portIndex] = false;
}

void InPortUser::releaseAllInPorts()
{
    for (size_t portIndex = 0; portIndex < inPorts.size(); portIndex++)
    	releaseInPort(portIndex);
}

int InPortUser::startCondition()
{
	if (inPorts.size() == 0)
		return noDataStartState;

	bool nakedCall = false;

	if (triggingPort() == NULL)
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
			if (isInPortCaught(port))
				continue;

			if (tryInPortLock(port))
				zeroPresent = false;
			else
				allPresent = false;
		}

		if (nakedCall)
			break;

		if (!allPresent)
		{
			if (yield())
				throw Poco::RuntimeException("startCondition",
						"Task cancellation upon user request");
		}

//		poco_information(logger(), "Not all inputs caught. Retrying...");
	}

	if (allPresent)
		return allDataStartState;

	if (zeroPresent)
		return noDataStartState;
	else
		return unknownStartState;
}

std::set<size_t> InPortUser::portsWithNewData()
{
	std::set<size_t> portSet;

	for (size_t portIndex=0; portIndex < inPorts.size(); portIndex++)
	{
		if ((*caughts)[portIndex] && inPorts[portIndex]->isNew())
			portSet.insert(portIndex);
	}

	return portSet;
}

InPortUser::~InPortUser()
{
    for (std::vector<InPort*>::iterator it=inPorts.begin(), ite=inPorts.end();
            it!=ite; it++)
        delete *it;
}

