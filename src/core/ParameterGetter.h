/**
 * @file	src/core/ParameterGetter.h
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
#include "ParameterWorker.h"

#include "Poco/RefCountedObject.h"

class ParameterizedEntityWithWorkers;

/**
 * ParameterGetter
 *
 * @par since 2.1.0-dev.4
 * Emit data retrieved via getParameterValue when the source module executes
 */
class ParameterGetter:
		public ParameterWorker,
		public DataSource,
		public Poco::RefCountedObject
{
public:
	ParameterGetter(ParameterizedEntityWithWorkers* parameterized, size_t paramIndex);
	virtual ~ParameterGetter() { }

	std::string name() { return mName; }
	std::string description();

    /**
     * getParameterValue and send it as dataSource
     */
    void emitParamValue();

	void detach();

private:
	ParameterGetter();

	static size_t refCount;

	std::string mName;

	void incUser() { duplicate(); }
	void decUser() { release();   }
	size_t userCnt() { return referenceCount(); }

	void sourceCancel();
	void sourceWaitCancelled();
	void sourceReset();
};

#endif /* SRC_PARAMETERGETTER_H_ */
