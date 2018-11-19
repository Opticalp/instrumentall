/**
 * @file	src/core/TypeNeutralData.cpp
 * @date	june 2016
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

#include "TypeNeutralData.h"

TypeNeutralData::TypeNeutralData(int datatype):
	mDataType(datatype), dataStore(NULL)
{
	setNewData(datatype, true);
}

TypeNeutralData::TypeNeutralData(const TypeNeutralData& other):
		mDataType(other.mDataType), dataStore(NULL)
{
    switch (mDataType)
    {
    // scalar containers
    case (typeInt32 | contScalar):
		{
    	Poco::Int32* tmp = new Poco::Int32(*reinterpret_cast<Poco::Int32*>(other.dataStore));
        dataStore = reinterpret_cast<void*>(tmp);
        break;
		}
    case (typeUInt32 | contScalar):
		{
		Poco::UInt32* tmp = new Poco::UInt32(*reinterpret_cast<Poco::UInt32*>(other.dataStore));
    	dataStore = reinterpret_cast<void*>(tmp);
        break;
		}
    case (typeInt64 | contScalar):
		{
		Poco::Int64* tmp = new Poco::Int64(*reinterpret_cast<Poco::Int64*>(other.dataStore));
    	dataStore = reinterpret_cast<void*>(tmp);
        break;
		}
    case (typeUInt64 | contScalar):
		{
		Poco::UInt64* tmp = new Poco::UInt64(*reinterpret_cast<Poco::UInt64*>(other.dataStore));
    	dataStore = reinterpret_cast<void*>(tmp);
        break;
		}
    case (typeFloat | contScalar):
		{
        float* tmp = new float(*reinterpret_cast<float*>(other.dataStore));
    	dataStore = reinterpret_cast<void*>(tmp);
        break;
		}
    case (typeDblFloat | contScalar):
		{
        double* tmp = new double(*reinterpret_cast<double*>(other.dataStore));
    	dataStore = reinterpret_cast<void*>(tmp);
        break;
		}
    case (typeString | contScalar):
		{
        std::string* tmp = new std::string(*reinterpret_cast<std::string*>(other.dataStore));
    	dataStore = reinterpret_cast<void*>(tmp);
        break;
		}

    // vector containers
    case (typeInt32 | contVector):
		{
    	std::vector<Poco::Int32>* tmp = new std::vector<Poco::Int32>(*reinterpret_cast< std::vector<Poco::Int32>* >(other.dataStore));
    	dataStore = reinterpret_cast<void*>(tmp);
        break;
		}
    case (typeUInt32 | contVector):
		{
    	std::vector<Poco::UInt32>* tmp = new std::vector<Poco::UInt32>(*reinterpret_cast< std::vector<Poco::UInt32>* >(other.dataStore));
    	dataStore = reinterpret_cast<void*>(tmp);
        break;
		}
    case (typeInt64 | contVector):
		{
    	std::vector<Poco::Int64>* tmp = new std::vector<Poco::Int64>(*reinterpret_cast< std::vector<Poco::Int64>* >(other.dataStore));
    	dataStore = reinterpret_cast<void*>(tmp);
        break;
		}
    case (typeUInt64 | contVector):
		{
    	std::vector<Poco::UInt64>* tmp = new std::vector<Poco::UInt64>(*reinterpret_cast< std::vector<Poco::UInt64>* >(other.dataStore));
    	dataStore = reinterpret_cast<void*>(tmp);
        break;
		}
    case (typeFloat | contVector):
		{
    	std::vector<float>* tmp = new std::vector<float>(*reinterpret_cast< std::vector<float>* >(other.dataStore));
    	dataStore = reinterpret_cast<void*>(tmp);
        break;
		}
    case (typeDblFloat | contVector):
		{
    	std::vector<double>* tmp = new std::vector<double>(*reinterpret_cast< std::vector<double>* >(other.dataStore));
    	dataStore = reinterpret_cast<void*>(tmp);
        break;
		}
    case (typeString | contVector):
		{
    	std::vector<std::string>* tmp = new std::vector<std::string>(*reinterpret_cast< std::vector<std::string>* >(other.dataStore));
    	dataStore = reinterpret_cast<void*>(tmp);
        break;
		}

    // others
    case typeUndefined:
        break;
    default:
        poco_bugcheck_msg("DataItem::DataItem: unknown requested data type");
        throw Poco::BugcheckException();
    }
}

TypeNeutralData::~TypeNeutralData()
{
	removeData();
}

TypeNeutralData& TypeNeutralData::operator =(const TypeNeutralData& other)
{
    TypeNeutralData tmp(other);
    swap(tmp);
    return *this;
}

void TypeNeutralData::swap(TypeNeutralData& other)
{
    void* tmpData = other.dataStore;
    other.dataStore = this->dataStore;
    this->dataStore = tmpData;
    
    int tmpDataType = other.mDataType;
    other.mDataType = this->mDataType;
    this->mDataType = tmpDataType;
}

void TypeNeutralData::setNewData(int datatype, bool force)
{
	if ((datatype == mDataType) && !force)
		return;

	mDataType = datatype;

	if (dataStore)
		removeData();

    switch (mDataType)
    {
    // scalar containers
    case (typeInt32 | contScalar):
        dataStore = reinterpret_cast<void*>(new Poco::Int32);
        break;
    case (typeUInt32 | contScalar):
        dataStore = reinterpret_cast<void*>(new Poco::UInt32);
        break;
    case (typeInt64 | contScalar):
        dataStore = reinterpret_cast<void*>(new Poco::Int64);
        break;
    case (typeUInt64 | contScalar):
        dataStore = reinterpret_cast<void*>(new Poco::UInt64);
        break;
    case (typeFloat | contScalar):
        dataStore = reinterpret_cast<void*>(new float);
        break;
    case (typeDblFloat | contScalar):
        dataStore = reinterpret_cast<void*>(new double);
        break;
    case (typeString | contScalar):
        dataStore = reinterpret_cast<void*>(new std::string);
        break;
#ifdef HAVE_OPENCV
    case (typeCvMat | contScalar):
        dataStore =  reinterpret_cast<void*>(new cv::Mat);
        break;
#endif

    // vector containers
    case (typeInt32 | contVector):
        dataStore = reinterpret_cast<void*>(new std::vector<Poco::Int32>);
        break;
    case (typeUInt32 | contVector):
        dataStore = reinterpret_cast<void*>(new std::vector<Poco::UInt32>);
        break;
    case (typeInt64 | contVector):
        dataStore = reinterpret_cast<void*>(new std::vector<Poco::Int64>);
        break;
    case (typeUInt64 | contVector):
        dataStore = reinterpret_cast<void*>(new std::vector<Poco::UInt64>);
        break;
    case (typeFloat | contVector):
        dataStore = reinterpret_cast<void*>(new std::vector<float>);
        break;
    case (typeDblFloat | contVector):
        dataStore = reinterpret_cast<void*>(new std::vector<double>);
        break;
    case (typeString | contVector):
        dataStore = reinterpret_cast<void*>(new std::vector<std::string>);
        break;
#ifdef HAVE_OPENCV
    case (typeCvMat | contVector):
        dataStore =  reinterpret_cast<void*>(new std::vector<cv::Mat>);
        break;
#endif

    // others
    case typeUndefined:
        break;
    default:
        poco_bugcheck_msg("DataItem::DataItem: unknown requested data type");
        throw Poco::BugcheckException();
    }
}

void TypeNeutralData::removeData()
{
    switch (mDataType)
    {
    // scalar containers
    case (typeInt32 | contScalar):
        delete reinterpret_cast<Poco::Int32*>(dataStore);
        break;
    case (typeUInt32 | contScalar):
        delete reinterpret_cast<Poco::UInt32*>(dataStore);
        break;
    case (typeInt64 | contScalar):
        delete reinterpret_cast<Poco::Int64*>(dataStore);
        break;
    case (typeUInt64 | contScalar):
        delete reinterpret_cast<Poco::UInt64*>(dataStore);
        break;
    case (typeFloat | contScalar):
        delete reinterpret_cast<float*>(dataStore);
        break;
    case (typeDblFloat | contScalar):
        delete reinterpret_cast<double*>(dataStore);
        break;
    case (typeString | contScalar):
        delete reinterpret_cast<std::string*>(dataStore);
        break;
#ifdef HAVE_OPENCV
    case (typeCvMat | contScalar):
        delete reinterpret_cast<cv::Mat*>(dataStore);
        break;
#endif

    // vector containers
    case (typeInt32 | contVector):
        delete reinterpret_cast<std::vector<Poco::Int32>*>(dataStore);
        break;
    case (typeUInt32 | contVector):
        delete reinterpret_cast<std::vector<Poco::UInt32>*>(dataStore);
        break;
    case (typeInt64 | contVector):
        delete reinterpret_cast<std::vector<Poco::Int64>*>(dataStore);
        break;
    case (typeUInt64 | contVector):
        delete reinterpret_cast<std::vector<Poco::UInt64>*>(dataStore);
        break;
    case (typeFloat | contVector):
        delete reinterpret_cast<std::vector<float>*>(dataStore);
        break;
    case (typeDblFloat | contVector):
        delete reinterpret_cast<std::vector<double>*>(dataStore);
        break;
    case (typeString | contVector):
        delete reinterpret_cast<std::vector<std::string>*>(dataStore);
        break;
#ifdef HAVE_OPENCV
    case (typeCvMat | contVector):
        delete reinterpret_cast<std::vector<cv::Mat>*>(dataStore);
        break;
#endif

    // others
    case typeUndefined:
    default:
        break;
    }

    dataStore = NULL;
    mDataType = typeUndefined;
}

std::string TypeNeutralData::dataTypeShortStr(int datatype)
{
    if (datatype == typeUndefined)
        return "undef";

    std::string desc;

    switch (datatype & ~contVector)
    {
    case typeInt32:
        desc = "int32";
        break;
    case typeUInt32:
        desc = "uint32";
        break;
    case typeInt64:
        desc = "int64";
        break;
    case typeUInt64:
        desc = "uint64";
        break;
    case typeFloat:
        desc = "float";
        break;
    case typeDblFloat:
        desc = "dblFloat";
        break;
    case typeString:
        desc = "str";
        break;
#ifdef HAVE_OPENCV
    case typeCvMat:
        desc = "cvMat";
        break;
#endif
    default:
        return "";
    }

    if (datatype & contVector)
        desc += "Vect";

    return desc;
}

int TypeNeutralData::getTypeFromShortStr(std::string typeName)
{
    int retType = 0;
    std::string tmp(typeName);

    size_t pos = tmp.rfind("Vect");
    // container?
    if (pos != std::string::npos)
    {
        tmp.erase(pos);
        retType |= contVector;
    }

    if (tmp.compare("int32") == 0)
        retType |= typeInt32;
    else if (tmp.compare("uint32") == 0)
        retType |= typeUInt32;
    else if (tmp.compare("int64") == 0)
        retType |= typeInt64;
    else if (tmp.compare("uint64") == 0)
        retType |= typeUInt64;
    else if (tmp.compare("float") == 0)
        retType |= typeFloat;
    else if (tmp.compare("dblFloat") == 0)
        retType |= typeDblFloat;
    else if (tmp.compare("str") == 0)
        retType |= typeString;
#ifdef HAVE_OPENCV
    else if (tmp.compare("cvMat") == 0)
        retType |= typeCvMat;
#endif

    return retType;
}

void TypeNeutralData::checkDataType(int datatype)
{
    if ((datatype & ~contVector) == (mDataType & ~contVector))
        return;

    throw Poco::DataFormatException(dataTypeStr(mDataType) + " is expected");
}
