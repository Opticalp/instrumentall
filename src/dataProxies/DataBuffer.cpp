/**
 * @file	src/DataBuffer.cpp
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

#include "DataBuffer.h"

#include "Poco/NumberFormatter.h"

size_t DataBuffer::refCount = 0;

DataBuffer::DataBuffer(int datatype):
		DataProxy(datatype), mDatatype(datatype)
{
	mName = "DataBuffer";
	if (refCount)
		mName += Poco::NumberFormatter::format(refCount);

    refCount++;
}

void DataBuffer::convert()
{
    switch (mDatatype)
    {
    // scalar containers
    case (typeInt32 | contScalar):
    	*getData<Poco::Int32>() = *getDataSource()->getData<Poco::Int32>();
        break;
    case (typeUInt32 | contScalar):
		*getData<Poco::UInt32>() = *getDataSource()->getData<Poco::UInt32>();
        break;
    case (typeInt64 | contScalar):
		*getData<Poco::Int64>() = *getDataSource()->getData<Poco::Int64>();
        break;
    case (typeUInt64 | contScalar):
		*getData<Poco::UInt64>() = *getDataSource()->getData<Poco::UInt64>();
        break;
    case (typeFloat | contScalar):
		*getData<float>() = *getDataSource()->getData<float>();
        break;
    case (typeDblFloat | contScalar):
		*getData<double>() = *getDataSource()->getData<double>();
        break;
    case (typeString | contScalar):
		*getData<std::string>() = *getDataSource()->getData<std::string>();
    	break;

    // vector containers
    case (typeInt32 | contVector):
		*getData< std::vector<Poco::Int32> >() = *getDataSource()->getData< std::vector<Poco::Int32> >();
		break;
    case (typeUInt32 | contVector):
		*getData< std::vector<Poco::UInt32> >() = *getDataSource()->getData< std::vector<Poco::UInt32> >();
        break;
    case (typeInt64 | contVector):
		*getData< std::vector<Poco::Int64> >() = *getDataSource()->getData< std::vector<Poco::Int64> >();
        break;
    case (typeUInt64 | contVector):
		*getData< std::vector<Poco::UInt64> >() = *getDataSource()->getData< std::vector<Poco::UInt64> >();
        break;
    case (typeFloat | contVector):
		*getData< std::vector<float> >() = *getDataSource()->getData< std::vector<float> >();
        break;
    case (typeDblFloat | contVector):
		*getData< std::vector<double> >() = *getDataSource()->getData< std::vector<double> >();
        break;
    case (typeString | contVector):
		*getData< std::vector<std::string> >() = *getDataSource()->getData< std::vector<std::string> >();
        break;

    // others
    case typeUndefined:
        poco_bugcheck_msg("convert: undefined data type");
        throw Poco::BugcheckException();
    default:
        poco_bugcheck_msg("convert: unknown requested data type");
        throw Poco::BugcheckException();
    }
}
