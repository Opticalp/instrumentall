/**
 * @file	src/ParameterizedWithGetters.h
 * @date	Jul. 2016
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

#ifndef SRC_PARAMETERIZEDWITHGETTERS_H_
#define SRC_PARAMETERIZEDWITHGETTERS_H_

#include "ParameterGetter.h"

#include <set>

class ParameterizedEntity;

/**
 * ParameterizedWithGetters
 *
 * Manage a ParameterGetter set for a ParameterizedEntity
 */
class ParameterizedWithGetters
{
public:
	/**
	 * Constructor
	 *
	 */
	ParameterizedWithGetters(ParameterizedEntity* entity):
		self(entity)
	{
	}

	/**
	 * Destructor
	 *
	 * Invalidate parameterGetters before removing it.
	 */
	virtual ~ParameterizedWithGetters();

	/**
	 * Create a ParameterGetter and takes ownership of it
	 */
	Poco::AutoPtr<ParameterGetter> buildParameterGetter(size_t paramIndex);

	/**
	 * Create a ParameterGetter and takes ownership of it
	 */
	Poco::AutoPtr<ParameterGetter> buildParameterGetter(std::string paramName);

	/**
	 * Retrieve the existing parameter getters
	 */
	std::set< Poco::AutoPtr<ParameterGetter> > getParameterGetters()
		{ return getters; }

private:
	ParameterizedEntity* self;
	std::set< Poco::AutoPtr<ParameterGetter> > getters;
};

#endif /* SRC_PARAMETERIZEDWITHGETTERS_H_ */
