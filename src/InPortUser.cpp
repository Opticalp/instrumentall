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
#include "ExecutionAbortedException.h"

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

bool InPortUser::tryInPortCatchSource(size_t portIndex)
{
    if (isInPortCaught(portIndex))
        poco_bugcheck_msg("try to re-lock an input port that was already locked? ");

    if (inPorts[portIndex]->tryCatchSource())
    {
    	caughts->insert(portIndex);
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
    if (!isInPortCaught(portIndex))
    	poco_bugcheck_msg("try to read an input port that was not previously locked");

    inPorts[portIndex]->readInputDataAttribute(pAttr);
}

void InPortUser::releaseInPort(size_t portIndex)
{
    if (isInPortCaught(portIndex))
    {
        inPorts[portIndex]->releaseInputData();

		caughts->erase(portIndex);
		if (caughts->empty())
		    releaseStartingMutex();
    }
}

void InPortUser::releaseAllInPorts()
{
	for (std::set<size_t>::iterator it = caughts->begin(),
			ite = caughts->end(); it != ite; )
	{
		std::set<size_t>::iterator itTmp = it++;
    	releaseInPort(*itTmp); // caughts is modified by releaseInPort
	}

	releaseStartingMutex();
}

void InPortUser::safeReleaseAllInPorts(InPort* triggingPort)
{
    if (triggingPort)
        caughts->insert(triggingPort->index());

    releaseAllInPorts();
}


int InPortUser::startCondition()
{
	if (inPorts.size() == 0)
		return noDataStartState;

	if (triggingPort() == NULL)
	    return noDataStartState;
	else
		poco_information(logger(), name() + " started by " + triggingPort()->name());

	bool allPresent = false;

	while (!allPresent)
	{
		allPresent = true;
		try
		{
			for (size_t port = 0; port < inPorts.size(); port++)
			{
				if (isInPortCaught(port))
					continue;

				if (!tryInPortCatchSource(port))
				{
					allPresent = false;
				}
				//else
				//{
				//	poco_information(logger(), inPorts[port]->name() + " locked"
				//		+ (inPorts[port]->isNew()?" with new data":" with held data"));
				//}
			}

			if (!allPresent)
			{
				if (yield())
					throw ExecutionAbortedException("startCondition",
							"Task cancellation upon user request");
			}
		}
		catch (...)
		{
			releaseAllInPorts();
			throw;
		}

//		poco_information(logger(), "Not all inputs caught. Retrying...");
	}

	return allDataStartState;
}

InPortUser::~InPortUser()
{
    for (std::vector<InPort*>::iterator it=inPorts.begin(), ite=inPorts.end();
            it!=ite; it++)
        delete *it;
}

void InPortUser::cancelSources()
{
    for (std::vector<InPort*>::iterator it=inPorts.begin(), ite=inPorts.end();
            it!=ite; it++)
    	(*it)->cancelWithSource();
}

void InPortUser::resetSources()
{
    for (std::vector<InPort*>::iterator it=inPorts.begin(), ite=inPorts.end();
            it!=ite; it++)
    	(*it)->resetWithSource();
}
