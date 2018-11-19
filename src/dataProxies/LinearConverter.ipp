/**
 * @file	src/dataProxies/LinearConverter.ipp
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

#include "LinearConverter.h"

//#include <math.h>
#include <limits>

#include "tools/customRound.h"

//------------//
//  getInt32  //
//------------//

template <> inline
Poco::Int32 LinearConverter::getInt32(const Poco::UInt32& data)
{
	double ret = data * scale + offset;

	if (ret > std::numeric_limits<Poco::Int32>::max())
		return std::numeric_limits<Poco::Int32>::max();
	else
		return static_cast<Poco::Int32>(round(ret));
}

template <> inline
Poco::Int32 LinearConverter::getInt32(const Poco::Int64& data)
{
	double ret = data * scale + offset;

	if (ret > std::numeric_limits<Poco::Int32>::max())
		return std::numeric_limits<Poco::Int32>::max();
	else if (ret < std::numeric_limits<Poco::Int32>::min())
		return std::numeric_limits<Poco::Int32>::min();
	else
		return static_cast<Poco::Int32>(round(ret));
}

template <> inline
Poco::Int32 LinearConverter::getInt32(const Poco::UInt64& data)
{
	double ret = data * scale + offset;

	if (ret > std::numeric_limits<Poco::Int32>::max())
		return std::numeric_limits<Poco::Int32>::max();
	else
		return static_cast<Poco::Int32>(round(ret));
}

template <> inline
Poco::Int32 LinearConverter::getInt32(const float& data)
{
	double ret = data * scale + offset;

	if (ret > std::numeric_limits<Poco::Int32>::max())
		return std::numeric_limits<Poco::Int32>::max();
	else if (ret < std::numeric_limits<Poco::Int32>::min())
		return std::numeric_limits<Poco::Int32>::min();
	else
		return static_cast<Poco::Int32>(round(ret));
}

template <> inline
Poco::Int32 LinearConverter::getInt32(const double& data)
{
	double ret = data * scale + offset;

	if (ret > std::numeric_limits<Poco::Int32>::max())
		return std::numeric_limits<Poco::Int32>::max();
	else if (ret < std::numeric_limits<Poco::Int32>::min())
		return std::numeric_limits<Poco::Int32>::min();
	else
		return static_cast<Poco::Int32>(round(ret));
}

template<typename T>
inline Poco::Int32 LinearConverter::getInt32(const T& data)
{
	double ret = data * scale + offset;

	return static_cast<Poco::Int32>(round(ret));
}

//-------------//
//  getUInt32  //
//-------------//

template <> inline
Poco::UInt32 LinearConverter::getUInt32(const Poco::Int32& data)
{
	double ret = data * scale + offset;

	if (ret < 0)
		return 0;
	else
		return static_cast<Poco::UInt32>(round(ret));
}

template <> inline
Poco::UInt32 LinearConverter::getUInt32(const Poco::Int64& data)
{
	double ret = data * scale + offset;

	if (ret < 0)
		return 0;
	else if (ret > std::numeric_limits<Poco::UInt32>::max())
		return std::numeric_limits<Poco::UInt32>::max();
	else
		return static_cast<Poco::UInt32>(round(ret));
}

template <> inline
Poco::UInt32 LinearConverter::getUInt32(const Poco::UInt64& data)
{
	double ret = data * scale + offset;

	if (ret > std::numeric_limits<Poco::UInt32>::max())
		return std::numeric_limits<Poco::UInt32>::max();
	else
		return static_cast<Poco::UInt32>(round(ret));
}

template <> inline
Poco::UInt32 LinearConverter::getUInt32(const float& data)
{
	double ret = data * scale + offset;

	if (ret < 0)
		return 0;
	else if (ret > std::numeric_limits<Poco::UInt32>::max())
		return std::numeric_limits<Poco::UInt32>::max();
	else
		return static_cast<Poco::UInt32>(round(ret));
}

template <> inline
Poco::UInt32 LinearConverter::getUInt32(const double& data)
{
	double ret = data * scale + offset;

	if (ret < 0)
		return 0;
	else if (ret > std::numeric_limits<Poco::UInt32>::max())
		return std::numeric_limits<Poco::UInt32>::max();
	else
		return static_cast<Poco::UInt32>(round(ret));
}

template<typename T>
inline Poco::UInt32 LinearConverter::getUInt32(const T& data)
{
	double ret = data * scale + offset;

	return static_cast<Poco::UInt32>(round(ret));
}

//------------//
//  getInt64  //
//------------//

template <> inline
Poco::Int64 LinearConverter::getInt64(const Poco::UInt64& data)
{
	double ret = data * scale + offset;

	if (ret > std::numeric_limits<Poco::Int64>::max())
		return std::numeric_limits<Poco::Int64>::max();
	else
		return static_cast<Poco::Int64>(round(ret));
}

template <> inline
Poco::Int64 LinearConverter::getInt64(const float& data)
{
	double ret = data * scale + offset;

	if (ret > std::numeric_limits<Poco::Int64>::max())
		return std::numeric_limits<Poco::Int64>::max();
	else if (ret < std::numeric_limits<Poco::Int64>::min())
		return std::numeric_limits<Poco::Int64>::min();
	else
		return static_cast<Poco::Int64>(round(ret));
}

template <> inline
Poco::Int64 LinearConverter::getInt64(const double& data)
{
	double ret = data * scale + offset;

	if (ret > std::numeric_limits<Poco::Int64>::max())
		return std::numeric_limits<Poco::Int64>::max();
	else if (ret < std::numeric_limits<Poco::Int64>::min())
		return std::numeric_limits<Poco::Int64>::min();
	else
		return static_cast<Poco::Int64>(round(ret));
}

template<typename T>
inline Poco::Int64 LinearConverter::getInt64(const T& data)
{
	double ret = data * scale + offset;

	return static_cast<Poco::Int64>(round(ret));
}

//-------------//
//  getUInt64  //
//-------------//

template <> inline
Poco::UInt64 LinearConverter::getUInt64(const Poco::Int32& data)
{
	double ret = data * scale + offset;

	if (ret < 0)
		return 0;
	else
		return static_cast<Poco::UInt64>(round(ret));
}

template <> inline
Poco::UInt64 LinearConverter::getUInt64(const Poco::Int64& data)
{
	double ret = data * scale + offset;

	if (ret < 0)
		return 0;
	else
		return static_cast<Poco::UInt64>(round(ret));
}

template <> inline
Poco::UInt64 LinearConverter::getUInt64(const float& data)
{
	double ret = data * scale + offset;

	if (ret < 0)
		return 0;
	else if (ret > std::numeric_limits<Poco::UInt64>::max())
		return std::numeric_limits<Poco::UInt64>::max();
	else
		return static_cast<Poco::UInt64>(round(ret));
}

template <> inline
Poco::UInt64 LinearConverter::getUInt64(const double& data)
{
	double ret = data * scale + offset;

	if (ret < 0)
		return 0;
	else if (ret > std::numeric_limits<Poco::UInt64>::max())
		return std::numeric_limits<Poco::UInt64>::max();
	else
		return static_cast<Poco::UInt64>(round(ret));
}

template<typename T>
inline Poco::UInt64 LinearConverter::getUInt64(const T& data)
{
	double ret = data * scale + offset;

	return static_cast<Poco::UInt64>(round(ret));
}

//-------------//
//  getFloat   //
//-------------//

template <> inline
float LinearConverter::getFloat(const double& data)
{
	double ret = data * scale + offset;

	if (ret > std::numeric_limits<float>::max())
		return std::numeric_limits<float>::max();
	else if (ret < std::numeric_limits<float>::min())
		return std::numeric_limits<float>::min();
	else
		return static_cast<float>(ret);
}

template<typename T>
inline float LinearConverter::getFloat(const T& data)
{
	double ret = data * scale + offset;

	return static_cast<float>(ret);
}

//----------------//
//  getDblFloat   //
//----------------//

template<typename T>
inline double LinearConverter::getDblFloat(const T& data)
{
	double ret = data * scale + offset;

	return static_cast<double>(ret);
}

//-----------------//
//  assignVector   //
//-----------------//

template<typename T>
inline void LinearConverter::assignVectorInt32(
		std::vector<Poco::Int32>* destination)
{
	std::vector<T>* dataIn = getDataSource()->getData< std::vector<T> >();
	size_t length = dataIn->size();
	destination->resize(length);

	for (size_t index = 0; index < length; index++)
		(*destination)[index] = getInt32<T>((*dataIn)[index]);
}

template<typename T>
inline void LinearConverter::assignVectorUInt32(
		std::vector<Poco::UInt32>* destination)
{
	std::vector<T>* dataIn = getDataSource()->getData< std::vector<T> >();
	size_t length = dataIn->size();
	destination->resize(length);

	for (size_t index = 0; index < length; index++)
		(*destination)[index] = getUInt32<T>((*dataIn)[index]);
}

template<typename T>
inline void LinearConverter::assignVectorInt64(
		std::vector<Poco::Int64>* destination)
{
	std::vector<T>* dataIn = getDataSource()->getData< std::vector<T> >();
	size_t length = dataIn->size();
	destination->resize(length);

	for (size_t index = 0; index < length; index++)
		(*destination)[index] = getInt64<T>((*dataIn)[index]);
}

template<typename T>
inline void LinearConverter::assignVectorUInt64(
		std::vector<Poco::UInt64>* destination)
{
	std::vector<T>* dataIn = getDataSource()->getData< std::vector<T> >();
	size_t length = dataIn->size();
	destination->resize(length);

	for (size_t index = 0; index < length; index++)
		(*destination)[index] = getUInt64<T>((*dataIn)[index]);
}

template<typename T>
inline void LinearConverter::assignVectorFloat(
		std::vector<float>* destination)
{
	std::vector<T>* dataIn = getDataSource()->getData< std::vector<T> >();
	size_t length = dataIn->size();
	destination->resize(length);

	for (size_t index = 0; index < length; index++)
		(*destination)[index] = getFloat<T>((*dataIn)[index]);
}

template<typename T>
inline void LinearConverter::assignVectorDblFloat(
		std::vector<double>* destination)
{
	std::vector<T>* dataIn = getDataSource()->getData< std::vector<T> >();
	size_t length = dataIn->size();
	destination->resize(length);

	for (size_t index = 0; index < length; index++)
		(*destination)[index] = getDblFloat<T>((*dataIn)[index]);
}

