/**
 * @file	src/DuplicatedSource.h
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

#ifndef SRC_DUPLICATEDSOURCE_H_
#define SRC_DUPLICATEDSOURCE_H_

#include "DataSource.h"

#include "Breaker.h"

#include "Poco/RefCountedObject.h"

class DataTarget;

/**
 * DuplicatedSource
 *
 * Special data source to be used by a UI to replace temporary a dynamic
 * data source.
 * The data item is duplicated and the binding is broken using a breaker.
 * 
 * The lifetime of the object should be managed using a Poco::AutoPtr
 */
class DuplicatedSource: public DataSource, public Poco::RefCountedObject
{
public:
	/**
	 * Replace the source and break all the outgoing connections
	 */
	DuplicatedSource(DataSource* source);

	/**
	 * Replace the source and break only the connection to target
	 */
	DuplicatedSource(DataSource* source, DataTarget* target);

	/**
	 * Release the connections
	 */
	virtual ~DuplicatedSource() { }

	std::string name() { return mName; }

	/**
	 * Launch the targets processes
	 */
	void trigTargets();

private:
	/**
	 * Std Constructor. Should not be used.
	 */
	DuplicatedSource();

	Breaker breaker; ///< connection breaker

	std::string mName;

	void sourceCancel() { }
	void sourceWaitCancelled() { }
	void sourceReset() { }
};

#endif /* SRC_DUPLICATEDSOURCE_H_ */
