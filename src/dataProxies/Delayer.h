/**
 * @file	src/dataProxies/Delayer.h
 * @date	Apr. 2018
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2018 Ph. Renaud-Goud / Opticalp

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

#ifndef SRC_DELAYER_H_
#define SRC_DELAYER_H_

#include "core/DataProxy.h"

/**
 * Delayer
 *
 * This is a special proxy that delays the execution during the duration
 * given as parameter
 */
class Delayer: public DataProxy
{
public:
	Delayer();
	virtual ~Delayer() { }

    std::string description() { return classDescription(); }

    static std::string classDescription()
        { return "Delay data without any conversion. "; }

    std::set<int> supportedInputDataType()
		{ return supportedDataType(); }

    std::set<int> supportedOutputDataType()
		{ return supportedDataType(); }

private:
	static size_t refCount;

	std::set<int> supportedDataType();
    int mDatatype;

	/**
     * No conversion. Copy the input to the output.
     */
    void convert();

    enum params
    {
        paramDuration,
        paramCnt
    };

    unsigned long duration;

    Poco::Int64 getIntParameterValue(size_t paramIndex)
    {
        poco_assert(paramIndex == paramDuration);
        return static_cast<long>(duration);
    }

    void setIntParameterValue(size_t paramIndex, Poco::Int64 value)
    {
        poco_assert(paramIndex == paramDuration);
        if (value < 0)
            throw Poco::RangeException(name() + "::setParameterValue",
                    "Duration has to be positive (ms)");
        duration = static_cast<unsigned long>(value);
    }
};

#endif /* SRC_DELAYER_H_ */
