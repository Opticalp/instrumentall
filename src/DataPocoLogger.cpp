/**
 * @file	src/DataPocoLogger.cpp
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

void DataPocoLogger::runTask()
{
    // TODO: lock main mutex?

    if (data() == NULL)
        return;

    int dataType = data()->dataType();

    if (DataItem::isVector(dataType))
    {
        logVectorValue(DataItem::noContainerDataType(dataType));
    }
    else
    {
        switch (DataItem::noContainerDataType(dataType))
        {
        case DataItem::typeInt32:
            poco_information(logger(),
                    Poco::NumberFormatter::format(*(data()->getDataToRead<Poco::Int32>())));
            break;
        case DataItem::typeUInt32:
            poco_information(logger(),
                    Poco::NumberFormatter::format(*(data()->getDataToRead<Poco::UInt32>())));
            break;
        case DataItem::typeInt64:
            poco_information(logger(),
                    Poco::NumberFormatter::format(*(data()->getDataToRead<Poco::Int64>())));
            break;
        case DataItem::typeUInt64:
            poco_information(logger(),
                    Poco::NumberFormatter::format(*(data()->getDataToRead<Poco::UInt64>())));
            break;
        case DataItem::typeFloat:
            poco_information(logger(),
                    Poco::NumberFormatter::format(*(data()->getDataToRead<float>())));
            break;
        case DataItem::typeDblFloat:
            poco_information(logger(),
                    Poco::NumberFormatter::format(*(data()->getDataToRead<double>())));
            break;
        case DataItem::typeString:
            poco_information(logger(), *(data()->getDataToRead<std::string>()));
            break;
        default:
            data()->releaseData();
            throw Poco::NotImplementedException("DataPocoLogger",
                    "data type not implemented");
        }
    }

    data()->releaseData();
}

void DataPocoLogger::logVectorValue(DataItem::DataTypeEnum dataType)
{
    switch (dataType)
    {
    case DataItem::typeInt32:
    {
        std::vector<Poco::Int32>* pData;
        pData = data()->getDataToRead< std::vector<Poco::Int32> >();

        std::string msg;

        for (std::vector<Poco::Int32>::iterator it = pData->begin(),
                ite = pData->end(); it != ite; it++)
        {
            msg += Poco::NumberFormatter::format(*it) + "\t";
        }

        msg.erase(msg.end()-1);

        poco_information(logger(), msg);
        break;
    }
    case DataItem::typeUInt32:
    {
        std::vector<Poco::UInt32>* pData;
        pData = data()->getDataToRead< std::vector<Poco::UInt32> >();

        std::string msg;

        for (std::vector<Poco::UInt32>::iterator it = pData->begin(),
                ite = pData->end(); it != ite; it++)
        {
            msg += Poco::NumberFormatter::format(*it) + "\t";
        }

        msg.erase(msg.end()-1);

        poco_information(logger(), msg);
        break;
    }
    case DataItem::typeInt64:
    {
        std::vector<Poco::Int64>* pData;
        pData = data()->getDataToRead< std::vector<Poco::Int64> >();

        std::string msg;

        for (std::vector<Poco::Int64>::iterator it = pData->begin(),
                ite = pData->end(); it != ite; it++)
        {
            msg += Poco::NumberFormatter::format(*it) + "\t";
        }

        msg.erase(msg.end()-1);

        poco_information(logger(), msg);
        break;
    }
    case DataItem::typeUInt64:
    {
        std::vector<Poco::UInt64>* pData;
        pData = data()->getDataToRead< std::vector<Poco::UInt64> >();

        std::string msg;

        for (std::vector<Poco::UInt64>::iterator it = pData->begin(),
                ite = pData->end(); it != ite; it++)
        {
            msg += Poco::NumberFormatter::format(*it) + "\t";
        }

        msg.erase(msg.end()-1);

        poco_information(logger(), msg);
        break;
    }
    case DataItem::typeFloat:
    {
        std::vector<float>* pData;
        pData = data()->getDataToRead< std::vector<float> >();

        std::string msg;

        for (std::vector<float>::iterator it = pData->begin(),
                ite = pData->end(); it != ite; it++)
        {
            msg += Poco::NumberFormatter::format(*it) + "\t";
        }

        msg.erase(msg.end()-1);

        poco_information(logger(), msg);
        break;
    }
    case DataItem::typeDblFloat:
    {
        std::vector<double>* pData;
        pData = data()->getDataToRead< std::vector<double> >();

        std::string msg;

        for (std::vector<double>::iterator it = pData->begin(),
                ite = pData->end(); it != ite; it++)
        {
            msg += Poco::NumberFormatter::format(*it) + "\t";
        }

        msg.erase(msg.end()-1);

        poco_information(logger(), msg);
        break;
    }
    case DataItem::typeString:
    {
        std::vector<std::string>* pData;
        pData = data()->getDataToRead< std::vector<std::string> >();

        std::string msg;

        for (std::vector<std::string>::iterator it = pData->begin(),
                ite = pData->end(); it != ite; it++)
        {
            msg += "\"" + *it + "\"\t";
        }

        msg.erase(msg.end()-1);

        poco_information(logger(), msg);
        break;
    }
    default:
        data()->releaseData();
        throw Poco::NotImplementedException("DataPocoLogger",
                "data type not implemented");
    }
}

DataPocoLogger::DataPocoLogger():
        DataLogger(name())
{
    // FIXME
    setLogger("DataLoggerDebug");
}
