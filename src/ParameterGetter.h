/**
 * @file	src/ParameterGetter.h
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

#ifndef SRC_PARAMETERGETTER_H_
#define SRC_PARAMETERGETTER_H_

#include "DataSource.h"
#include "DataTarget.h"

#include "Poco/RefCountedObject.h"
#include "Poco/Thread.h"

class ParameterizedEntity;

/**
 * ParameterGetter
 *
 * Emit data retrieved via getParameterValue when trigged by the
 * input.
 */
class ParameterGetter:
		public DataSource,
		public DataTarget,
		public Poco::RefCountedObject
{
public:
	ParameterGetter(ParameterizedEntity* parameterized, size_t paramIndex);
	virtual ~ParameterGetter() { }

	std::string name() { return mName; }
	std::string description();

	/**
	 * Called by the owner (parameterized entity)
	 * to release the parameter getter.
	 */
	void invalidate()
		{ parent = NULL; }

	ParameterizedEntity* getParent() { return parent; }

private:
	ParameterGetter();

	static size_t refCount;

	/**
	 * getParameterValue and send it as dataSource
	 */
	void runTarget();

	bool isSupportedInputDataType(int datatype)
		{ return (datatype == dataType()); }

    std::set<int> supportedInputDataType()
	{
		std::set<int> ret;
		ret.insert(dataType());
		return ret;
	}

	bool yield() { Poco::Thread::yield(); return false; }

    /**
	 * Retrieve a parameter data type
	 *
	 * @param parameterized parameterized entity which owns the parameter
	 * @param paramIndex parameter index for the parameterized entity
	 * @return TypeNeutralData::DataTypeEnum of the given parameter
	 */
	static int paramDataType(ParameterizedEntity* parameterized, size_t paramIndex);

	std::string mName;
	ParameterizedEntity* parent;
	size_t mParamIndex;

	void incUser() { duplicate(); }
	void decUser() { release();   }
	size_t userCnt() { return referenceCount(); }
};

#endif /* SRC_PARAMETERGETTER_H_ */
