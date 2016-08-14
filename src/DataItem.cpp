/**
 * @file	src/DataItem.cpp
 * @date	Feb. 2016
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

#include "DataItem.h"
#include "DataLogger.h"
#include "DataManager.h"

#include "OutPort.h"
#include "Module.h"

#include "Poco/Types.h"
#include "Poco/Util/Application.h"

DataItem::DataItem(int dataType, OutPort* parent):
        mDataType(dataType),
        mParentPort(parent),
		expired(true),
		readLockCnt(0),
		writeLockCnt(0)
{
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

    // others
    case typeUndefined:
        break;
    default:
        poco_bugcheck_msg("DataItem::DataItem: unknown requested data type");
        throw Poco::BugcheckException();
    }
}

DataItem::~DataItem()
{
    // The loggers are detached by DataManager::removeOutPort()

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

    // others
    case typeUndefined:
    default:
        break;
    }
}

void DataItem::releaseNewData()
{
	expired = false;
    releaseData();

    Poco::Util::Application::instance()
            .getSubsystem<DataManager>()
            .newData(this);
}

void DataItem::releaseBrokenData()
{
    expired = true;
    releaseData();
}

void DataItem::registerLogger(DataLogger* logger)
{
    loggersLock.writeLock();
    allLoggers.insert(logger);
    loggersLock.unlock();
}

std::set<SharedPtr<DataLogger*> > DataItem::loggers()
{
    std::set<SharedPtr<DataLogger*> > tmpList;

    loggersLock.readLock();

    for (std::set< DataLogger* >::iterator it = allLoggers.begin(),
            ite = allLoggers.end(); it != ite; it++ )
    {
        tmpList.insert( Poco::Util::Application::instance()
                            .getSubsystem<DataManager>()
                            .getDataLogger(*it) );
    }

    loggersLock.unlock();

    return tmpList;
}

void DataItem::lockCntLogger()
{
	if (mParentPort == NULL)
		return;

	std::cout << mParentPort->name() << " of " << mParentPort->parent()->name() << ": "
	          << " readLock/writeLock == " << readLockCnt << "/" << writeLockCnt << std::endl;
}

void DataItem::detachLogger(DataLogger* logger)
{
    loggersLock.writeLock();
    allLoggers.erase(logger);
    loggersLock.unlock();
}

void DataItem::checkDataType(int datatype)
{
    if ((datatype & ~contVector) == (mDataType & ~contVector))
        return;

    throw Poco::DataFormatException(dataTypeStr(mDataType) + " is expected");
}

std::string DataItem::dataTypeShortStr(int datatype)
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
    default:
        return "";
    }

    if (datatype & contVector)
        desc += "Vect";

    return desc;
}

int DataItem::getTypeFromShortStr(std::string typeName)
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

    return retType;
}

