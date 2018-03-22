/**
 * @file	src/dataProxies/SimpleNumConverter.h
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

#ifndef SRC_SIMPLENUMCONVERTER_H_
#define SRC_SIMPLENUMCONVERTER_H_

#include "core/DataProxy.h"

/**
 * SimpleNumConverter
 *
 * DataProxy to convert between simple data types (int32, uint32, int64,
 * uint64, float, double).
 *
 * @par 2.1.0-dev.6
 * Add parameters support
 */
class SimpleNumConverter: public DataProxy
{
public:
	SimpleNumConverter();
	virtual ~SimpleNumConverter() { }

    std::string description() { return classDescription(); }

    static std::string classDescription()
        { return "Conversion between simple data types. "
        		"int32, uint32, int64, uint64, float, dbl float are supported. "
        		"Vect version is supported too. "
        		"The cast is truncating the out-of-range values"; }

private:
	static size_t refCount;

    std::set<int> supportedInputDataType();
    std::set<int> supportedOutputDataType();

    /**
     * Implement DataSource::preferredOutputDataType
     *
     * Do not really make sense since this proxy should operate a conversion
     *
     * @return input data type
     *
     * @throw Poco::NullPointerException if the input is not plugged
     */
    int preferredOutputDataType()
    	{ return getDataSource()->dataType(); }

    /**
     * Main logic
     */
    void convert();

    template <typename T> Poco::Int32 getInt32(const T& data);
    template <typename T> Poco::UInt32 getUInt32(const T& data);
    template <typename T> Poco::Int64 getInt64(const T& data);
    template <typename T> Poco::UInt64 getUInt64(const T& data);
    template <typename T> float getFloat(const T& data);
    template <typename T> double getDblFloat(const T& data);

    template <typename T> void assignVectorInt32(std::vector<Poco::Int32>* destination);
    template <typename T> void assignVectorUInt32(std::vector<Poco::UInt32>* destination);
    template <typename T> void assignVectorInt64(std::vector<Poco::Int64>* destination);
    template <typename T> void assignVectorUInt64(std::vector<Poco::UInt64>* destination);
    template <typename T> void assignVectorFloat(std::vector<float>* destination);
    template <typename T> void assignVectorDblFloat(std::vector<double>* destination);
};

#include "SimpleNumConverter.ipp"

#endif /* SRC_SIMPLENUMCONVERTER_H_ */
