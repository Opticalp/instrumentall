/**
 * @file	src/ParameterizedWithSetters.cpp
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

#include "ParameterizedEntity.h"

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

bool ParameterizedWithSetters::trySetParameter(size_t paramIndex)
{
	Poco::ScopedLock<Poco::FastMutex> lock(alreadySetLock);

	if (paramAlreadySet.insert(paramIndex).second)
	{
		if (paramAlreadySet.size() == setters.size())
		{
			if (preApply)
				self->applyParameters();

			allSet.set();
			return true;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}

bool ParameterizedWithSetters::tryAllParametersSet()
{
	if (setters.size())
		return allSet.tryWait(0);
	else
		return true;
}

void ParameterizedWithSetters::parametersTreated()
{
	Poco::ScopedLock<Poco::FastMutex> lock(alreadySetLock);
	paramAlreadySet.clear();

	// allSet event should be already reset (auto-reset).
}
