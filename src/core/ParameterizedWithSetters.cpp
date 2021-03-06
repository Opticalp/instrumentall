/**
 * @file	src/core/ParameterizedWithSetters.cpp
 * @date	Aug. 2016
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

#include "ParameterizedWithSetters.h"

#include "ParameterizedEntityWithWorkers.h"
#include "ExecutionAbortedException.h"

using Poco::AutoPtr;

ParameterizedWithSetters::~ParameterizedWithSetters()
{
	while (setters.size())
	{
		const_cast<ParameterSetter*>(setters.begin()->get())->invalidate();
		setters.erase(setters.begin());
	}
}

AutoPtr<ParameterSetter> ParameterizedWithSetters::buildParameterSetter(
		size_t paramIndex, bool immediateApply)
{
	for (std::set< Poco::AutoPtr<ParameterSetter> >::iterator it = setters.begin(),
			ite = setters.end(); it != ite; it++)
	{
		if (it->get()->getParameterIndex() == paramIndex)
			const_cast<ParameterSetter*>(it->get())->invalidate();
	}

	AutoPtr<ParameterSetter> ptr(new ParameterSetter(self, paramIndex, this, immediateApply));
	setters.insert(ptr);
	return ptr;
}

Poco::AutoPtr<ParameterSetter> ParameterizedWithSetters::buildParameterSetter(
		std::string paramName)
{
	return buildParameterSetter(self->getParameterIndex(paramName));
}

Poco::AutoPtr<ParameterSetter> ParameterizedWithSetters::buildParameterSetter(
		std::string paramName, bool imm)
{
	return buildParameterSetter(self->getParameterIndex(paramName), imm);
}

void ParameterizedWithSetters::removeParameterSetter(ParameterSetter* setter)
{
	setter->invalidate();

	for (std::set< Poco::AutoPtr<ParameterSetter> >::iterator it = setters.begin(),
		ite = setters.end(); it != ite; it++)
		if (it->get() == setter)
		{
			setters.erase(*it);
			break;
		}
}

bool ParameterizedWithSetters::trySetParameter(size_t paramIndex)
{
    alreadySetLock.lock();

    if (paramAlreadySet.count(paramIndex) == 0)
        return true;
    else
    {
        alreadySetLock.unlock();
        return false;
    }
}

void ParameterizedWithSetters::trigSetParameter(size_t paramIndex)
{

    if (paramAlreadySet.insert(paramIndex).second)
    {
        bool done = (paramAlreadySet.size() == setters.size());

        alreadySetLock.unlock();
        if (done)
        {
            if (preApply)
                self->tryApplyParameters(true);

            allSet.set();
        }
    }
    else
    {
        poco_bugcheck_msg("trying to trig a parameter that was not previously locked");
    }
}

bool ParameterizedWithSetters::tryAllParametersSet()
{
	if (setters.size())
	{
		if (allSet.tryWait(0))
		    return true;
		else
		{
		    for (std::set< Poco::AutoPtr<ParameterSetter> >::iterator it = setters.begin(),
		            ite = setters.end(); it != ite; it++)
		    {
		        alreadySetLock.lock();
		        bool alreadySet = paramAlreadySet.count(const_cast<ParameterSetter*>(it->get())->getParameterIndex()) > 0 ;
		        alreadySetLock.unlock();

		        if ( !alreadySet &&  (it->get())->isTargetCancelling() )
		            throw ExecutionAbortedException("Apply parameters: "
		                    "Cancellation upon user request (setter)");
		    }

		    return false;
		}
	}
	else
		return true;
}

void ParameterizedWithSetters::parametersTreated()
{
	Poco::ScopedLock<Poco::FastMutex> lock(alreadySetLock);
	paramAlreadySet.clear();

	// allSet event should be already reset (auto-reset).
}
