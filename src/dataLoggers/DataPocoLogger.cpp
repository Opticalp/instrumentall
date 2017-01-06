/**
 * @file	src/dataLoggers/DataPocoLogger.cpp
 * @date	Mar 2016
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

#include "DataPocoLogger.h"

#include "Poco/NumberFormatter.h"

size_t DataPocoLogger::refCount = 0;

void DataPocoLogger::log()
{
    int datatype = sourceDataType();

    if (DataItem::isVector(datatype))
    {
        logVectorValue(DataItem::noContainerDataType(datatype));
    }
    else
    {
        switch (DataItem::noContainerDataType(datatype))
        {
        case DataItem::typeInt32:
            poco_information(recLogger(),
                    Poco::NumberFormatter::format(*(getDataSource()->getData<Poco::Int32>())));
            break;
        case DataItem::typeUInt32:
            poco_information(recLogger(),
                    Poco::NumberFormatter::format(*(getDataSource()->getData<Poco::UInt32>())));
            break;
        case DataItem::typeInt64:
            poco_information(recLogger(),
                    Poco::NumberFormatter::format(*(getDataSource()->getData<Poco::Int64>())));
            break;
        case DataItem::typeUInt64:
            poco_information(recLogger(),
                    Poco::NumberFormatter::format(*(getDataSource()->getData<Poco::UInt64>())));
            break;
        case DataItem::typeFloat:
            poco_information(recLogger(),
                    Poco::NumberFormatter::format(*(getDataSource()->getData<float>())));
            break;
        case DataItem::typeDblFloat:
            poco_information(recLogger(),
                    Poco::NumberFormatter::format(*(getDataSource()->getData<double>())));
            break;
        case DataItem::typeString:
            poco_information(recLogger(), *(getDataSource()->getData<std::string>()));
            break;
        default:
            throw Poco::NotImplementedException("DataPocoLogger",
                    "data type not implemented");
        }
    }
}

bool DataPocoLogger::isSupportedInputDataType(int datatype)
{
    switch (DataItem::noContainerDataType(datatype))
    {
    case DataItem::typeInt32:
    case DataItem::typeUInt32:
    case DataItem::typeInt64:
    case DataItem::typeUInt64:
    case DataItem::typeFloat:
    case DataItem::typeDblFloat:
    case DataItem::typeString:
        return true;
    default:
        return false;
    }
}

std::set<int> DataPocoLogger::supportedInputDataType()
{
	std::set<int> ret;

	ret.insert(DataItem::typeInt32);
	ret.insert(DataItem::typeUInt32);
	ret.insert(DataItem::typeInt64);
	ret.insert(DataItem::typeUInt64);
	ret.insert(DataItem::typeFloat);
	ret.insert(DataItem::typeDblFloat);
	ret.insert(DataItem::typeString);

	ret.insert(DataItem::typeInt32 | DataItem::contVector);
	ret.insert(DataItem::typeUInt32 | DataItem::contVector);
	ret.insert(DataItem::typeInt64 | DataItem::contVector);
	ret.insert(DataItem::typeUInt64 | DataItem::contVector);
	ret.insert(DataItem::typeFloat | DataItem::contVector);
	ret.insert(DataItem::typeDblFloat | DataItem::contVector);
	ret.insert(DataItem::typeString | DataItem::contVector);

	return ret;
}

void DataPocoLogger::logVectorValue(DataItem::DataTypeEnum dataType)
{
    std::string msg;

    switch (dataType)
    {
    case DataItem::typeInt32:
    {
        std::vector<Poco::Int32>* pData;
        pData = getDataSource()->getData< std::vector<Poco::Int32> >();

        for (std::vector<Poco::Int32>::iterator it = pData->begin(),
                ite = pData->end(); it != ite; it++)
        {
            msg += Poco::NumberFormatter::format(*it) + "\t";
        }

        break;
    }
    case DataItem::typeUInt32:
    {
        std::vector<Poco::UInt32>* pData;
        pData = getDataSource()->getData< std::vector<Poco::UInt32> >();

        for (std::vector<Poco::UInt32>::iterator it = pData->begin(),
                ite = pData->end(); it != ite; it++)
        {
            msg += Poco::NumberFormatter::format(*it) + "\t";
        }

        break;
    }
    case DataItem::typeInt64:
    {
        std::vector<Poco::Int64>* pData;
        pData = getDataSource()->getData< std::vector<Poco::Int64> >();

        for (std::vector<Poco::Int64>::iterator it = pData->begin(),
                ite = pData->end(); it != ite; it++)
        {
            msg += Poco::NumberFormatter::format(*it) + "\t";
        }

        break;
    }
    case DataItem::typeUInt64:
    {
        std::vector<Poco::UInt64>* pData;
        pData = getDataSource()->getData< std::vector<Poco::UInt64> >();

        for (std::vector<Poco::UInt64>::iterator it = pData->begin(),
                ite = pData->end(); it != ite; it++)
        {
            msg += Poco::NumberFormatter::format(*it) + "\t";
        }

        break;
    }
    case DataItem::typeFloat:
    {
        std::vector<float>* pData;
        pData = getDataSource()->getData< std::vector<float> >();

       for (std::vector<float>::iterator it = pData->begin(),
                ite = pData->end(); it != ite; it++)
        {
            msg += Poco::NumberFormatter::format(*it) + "\t";
        }

        break;
    }
    case DataItem::typeDblFloat:
    {
        std::vector<double>* pData;
        pData = getDataSource()->getData< std::vector<double> >();

        for (std::vector<double>::iterator it = pData->begin(),
                ite = pData->end(); it != ite; it++)
        {
            msg += Poco::NumberFormatter::format(*it) + "\t";
        }

        break;
    }
    case DataItem::typeString:
    {
        std::vector<std::string>* pData;
        pData = getDataSource()->getData< std::vector<std::string> >();

        for (std::vector<std::string>::iterator it = pData->begin(),
                ite = pData->end(); it != ite; it++)
        {
            msg += "\"" + *it + "\"\t";
        }

        break;
    }
    default:
        throw Poco::NotImplementedException("DataPocoLogger",
                "data type not implemented");
    }

    if (!msg.empty())
        msg.erase(msg.end()-1);

    poco_information(recLogger(), msg);
}

DataPocoLogger::DataPocoLogger():
        DataLogger("DataPocoLogger")
{
	setName(refCount);

    setRecLogger("DataLogger." + name());
    refCount++;
}
