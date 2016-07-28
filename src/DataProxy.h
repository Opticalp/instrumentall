/**
 * @file	src/DataProxy.h
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

#ifndef SRC_DATAPROXY_H_
#define SRC_DATAPROXY_H_

#include "DataSource.h"
#include "DataTarget.h"

#include "Poco/Thread.h"

/**
 * DataProxy
 *
 * Data converter used between a data source and a data target of incompatible
 * type. The conversion is done synchronously (no new thread).
 *
 * The derived classes shall implement a classDescription
 * static method. DataTarget::description can be implemented
 * as linking to this method.
 */
class DataProxy: public DataTarget, public DataSource
{
public:
	/**
	 * Constructor
	 *
	 * In the implementations, the constructor could set
	 * the unique name of the data proxy.
	 */
	DataProxy(int datatype):
		DataSource(datatype) { }

	virtual ~DataProxy() { }

	virtual std::string name() = 0;

protected:
	/**
	 * Convert the input data into the desired data type
	 */
	virtual void convert() = 0;

	bool yield() { Poco::Thread::yield(); return false; }

private:
	DataProxy();

	/**
	 * Launch the conversion (sync run)
	 *
	 *  - lock the outputs, tryCatch the input
	 *  - launch the conversion
	 *  - forward the data attributes
	 *  - release the inputs and notify the outputs
	 */
	void runTarget();
};

#include "Poco/DynamicFactory.h"

/**
 * DataProxy instantiator
 *
 * Allow the data proxies to be constructed easily using the Poco::DynamicFactory
 *
 * Proxy is a class derived from DataProxy
 */
template <class Proxy> class DataProxyInstantiator:
		public Poco::AbstractInstantiator<DataProxy>
{
public:
	DataProxyInstantiator(int datatype): mDatatype(datatype) { }

	DataProxy* createInstance() const
	{
		return new Proxy(mDatatype);
	}

private:
	int mDatatype;
};

#endif /* SRC_DATAPROXY_H_ */
