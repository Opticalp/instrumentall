/**
 * @file	src/dataProxies/LinearConverter.h
 * @date	Aug. 2018
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

#ifndef SRC_LINEARCONVERTER_H_
#define SRC_LINEARCONVERTER_H_

#include "core/DataProxy.h"

/**
 * LinearConverter
 *
 * DataProxy to convert between simple data types (int32, uint32, int64,
 * uint64, float, double) with offset and scale
 *
 */
class LinearConverter: public DataProxy
{
public:
	LinearConverter();
	virtual ~LinearConverter() { }

    std::string description() { return classDescription(); }

    static std::string classDescription()
        { return "Conversion between simple data types with offset and scale. \n"
        		"int32, uint32, int64, uint64, float, dbl float are supported. "
        		"Vect version is supported too. \n"
        		"The cast is truncating the out-of-range values. \n"
				"Scale is applied before offset. "; }

    std::set<int> supportedInputDataType();
    std::set<int> supportedOutputDataType();

    /**
     * Implement DataSource::preferredOutputDataType
     *
     * @return input data type
     *
     * @throw Poco::NullPointerException if the input is not plugged
     */
    int preferredOutputDataType()
    	{ return getDataSource()->dataType(); }

private:
	static size_t refCount;

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

	enum params
	{
		paramScale,
		paramOffset,
		paramCnt
	};

	double scale, offset;

	double getFloatParameterValue(size_t paramIndex);
	void setFloatParameterValue(size_t paramIndex, double value);
};

#include "LinearConverter.ipp"

#endif /* SRC_LINEARCONVERTER_H_ */
