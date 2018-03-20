/**
 * @file	src/dataProxies/DataBuffer.h
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

#ifndef SRC_DATABUFFER_H_
#define SRC_DATABUFFER_H_

#include "core/DataProxy.h"

/**
 * DataBuffer
 *
 * This is a special proxy that does not convert the data type.
 * Its only role is to buffer the data.
 *
 * @par 2.1.0-dev.6
 * Add parameters support
 *
 */
class DataBuffer: public DataProxy
{
public:
	DataBuffer(int datatype);
	virtual ~DataBuffer() { }

    std::string description() { return classDescription(); }

    static std::string classDescription()
        { return "Buffer data without any conversion. "
        		"To be used to release earlier data sources"; }

private:
	DataBuffer();

	static size_t refCount;

    bool isSupportedInputDataType(int datatype)
    {
    	return (datatype == mDatatype);
    }

    std::set<int> supportedInputDataType()
	{
		std::set<int> ret;
		ret.insert(mDatatype);
		return ret;
	}

    /**
     * No conversion. Copy the input to the output.
     */
    void convert();

	int mDatatype;
};

#endif /* SRC_DATABUFFER_H_ */
