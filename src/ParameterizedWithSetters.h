/**
 * @file	src/ParameterizedWithSetters.h
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

#ifndef SRC_PARAMETERIZEDWITHSETTERS_H_
#define SRC_PARAMETERIZEDWITHSETTERS_H_

#include "ParameterSetter.h"

#include <set>

class ParameterizedEntity;

/**
 * ParameterizedWithSetters
 *
 * Manage a ParameterSetter set for a ParameterizedEntity
 */
class ParameterizedWithSetters
{
public:
	/**
	 * Constructor
	 *
	 * @param entity ParameterizedEntity managing the parameters
	 * @param apply flag to determine if applyParameters should be
	 * called directly, once all parameter setters where called.
	 */
	ParameterizedWithSetters(ParameterizedEntity* entity, bool apply = true):
		self(entity), preApply(apply)
	{
	}

	/**
	 * Destructor
	 *
	 * Invalidate parameterSetters before removing it.
	 */
	virtual ~ParameterizedWithSetters();

	/**
	 * Create a ParameterSetter and takes ownership of it
	 */
	Poco::AutoPtr<ParameterSetter> buildParameterSetter(size_t paramIndex, bool immediateApply = false);

	/**
	 * Create a ParameterSetter and takes ownership of it
	 */
	Poco::AutoPtr<ParameterSetter> buildParameterSetter(std::string paramName);
	Poco::AutoPtr<ParameterSetter> buildParameterSetter(std::string paramName, bool immediateApply);

	/**
	 * Retrieve the existing parameter setters
	 */
	std::set< Poco::AutoPtr<ParameterSetter> > getParameterSetters()
		{ return setters; }

	/**
	 * Called by a ParameterSetter to notify the ParameterizedEntity
	 * that a parameter is set
	 *
	 * Insert paramIndex into paramAlreadySet
	 * If all set, set event, and reset paramAlreadySet,
	 * evtl apply parameters
	 *
	 * @return false if the parameter was already set. The calling function
	 * should wait until the return value is true, then.
	 */
	bool trySetParameter(size_t paramIndex);

	/**
	 * Check if the parameter setters have all been called
	 *
	 * @return true if all concerned parameters were sets
	 */
	bool tryAllParametersSet();

	/**
	 * Method called by the entity using the parameters when they are
	 * used.
	 *
	 * Typ.: Module calls this function after the processing.
	 *
	 * The parameter access via the parameter setters is locked via
	 * trySetParameter
	 *
	 * @see trySetParameter
	 */
	void parametersTreated();

private:
	ParameterizedEntity* self;
	std::set< Poco::AutoPtr<ParameterSetter> > setters;

	bool preApply; ///< determine if the parameters have to be applied immediately when all set

	std::set<size_t> paramAlreadySet;
	Poco::FastMutex alreadySetLock;

	Poco::Event allSet; ///< auto reset event to check if all parameter
};

#endif /* SRC_PARAMETERIZEDWITHSETTERS_H_ */
