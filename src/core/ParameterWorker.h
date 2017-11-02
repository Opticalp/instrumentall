/**
 * @file	src/core/ParameterWorker.h
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

#ifndef SRC_PARAMETERWORKER_H_
#define SRC_PARAMETERWORKER_H_

#include "Poco/Thread.h"

#include <string>

class ParameterizedEntityWithWorkers;

/**
 * ParameterWorker
 *
 * Base class for ParameterGetter and ParameterSetter
 */
class ParameterWorker
{
public:
	ParameterWorker(ParameterizedEntityWithWorkers* parameterized, size_t paramIndex);
	virtual ~ParameterWorker() { }

	/**
	 * Called by the owner (parameterized entity)
	 * to release the parameter worker.
	 */
	void invalidate()
		{ parent = NULL; }

	ParameterizedEntityWithWorkers* getParent() const
		{ return parent; }

	/**
	 * Retrieve the name of the pointed parameter
	 */
	std::string getParameterName() const;

	size_t getParameterIndex() const
		{ return mParamIndex; }

	/**
	 * Retrieve the data type of the pointed parameter
	 */
	int getParameterDataType() const
		{ return mDatatype; }

protected:
    /**
	 * Retrieve a parameter data type
	 *
	 * @param parameterized parameterized entity which owns the parameter
	 * @param paramIndex parameter index for the parameterized entity
	 * @return TypeNeutralData::DataTypeEnum of the given parameter
	 */
	static int paramDataType(ParameterizedEntityWithWorkers* parameterized, size_t paramIndex);

	bool yield() { Poco::Thread::yield(); return false; }

private:
	ParameterWorker();

	ParameterizedEntityWithWorkers* parent;
	size_t mParamIndex;
	int mDatatype;
};

#endif /* SRC_PARAMETERWORKER_H_ */
