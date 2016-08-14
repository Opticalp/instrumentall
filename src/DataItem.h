/**
 * @file	DataItem.h
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

#ifndef SRC_DATAITEM_H_
#define SRC_DATAITEM_H_

#include "DataAttribute.h"

#include "Poco/RWLock.h"
#include "Poco/SharedPtr.h"

#include <iostream>

using Poco::RWLock;
using Poco::SharedPtr;

class OutPort;
class DataLogger;

/**
 * DataItem
 *
 * Define data objects that will be used in the data manager.
 */
class DataItem
{
public:
    /**
     * Data type descriptor
     */
    enum DataTypeEnum
    {
        /// Undefined data type.
        /// To be combined with contScalar container.
        /// To be used in empty ports
        typeUndefined,

        // integer types
        typeInt32,
        typeInteger = typeInt32,
        typeUInt32,
        typeInt64,
        typeUInt64,

        // float types
        typeFloat,
        typeDblFloat,

        // character string
        typeString,

        typeCnt // count the types
    };

    /**
     * Mask to apply on the data type enum
     *
     * to get the complete data type description
     */
    enum DataContainerEnum
    {
        contScalar = 0, ///< no mask
        contVector = 0x8000
    };

    DataItem(int datatype = typeUndefined, OutPort* parent = NULL);
    virtual ~DataItem();

    /**
     * Get dataType as a character string
     *
     * To get the data type of a data item, use
     *
     *     dataTypeStr(item.dataType())
     *
     */
    static std::string dataTypeStr(int datatype);

    /**
     * Get dataType as a character string -- short version
     *
     * The string is "short" and contains no space
     */
    static std::string dataTypeShortStr(int datatype);

    /**
     * Counterpart of dataTypeShortStr
     */
    static int getTypeFromShortStr(std::string typeName);

    /**
     * Check if the given datatype container is a vector
     */
    static bool isVector(int datatype)
        { return (datatype & contVector) != 0 ; }

    static DataTypeEnum noContainerDataType(int datatype)
        { return static_cast<DataTypeEnum>(datatype & ~contVector); }

    /**
     * @throw Poco::DataFormatException is the type T
     * does not fit mDataType
     */
    template <typename T> void checkType();

    DataAttribute getDataAttribute() { return attribute; }

    void setDataAttribute(DataAttribute attr) { attribute = attr; }

    /**
     * Locking part of tryGetDataToWrite
     *
     * @see getDataToWrite
     */
    bool tryLockToWrite()
    {
    	if (dataLock.tryWriteLock())
    	{
            std::cout << "tryLockToWrite succeeded: " << std::endl;
    		writeLockCnt++;
    		lockCntLogger();
    		return true;
    	}
    	else
    		return false;
    }

    /**
     * Retrieving pointer part of tryGetDataToWrite
     *
     * @see tryLockToWrite
     */
    template<typename T> void getDataToWrite(T*& pData)
    {
        checkType<T>();
        pData = reinterpret_cast<T*>(dataStore);
    }

    /**
     * Retrieve a write reference on the data
     *
     * Lock the data
     * @throw Poco::DataFormatException forwarded from checkType()
     */
    template <typename T> bool tryGetDataToWrite(T*& pData)
    {
        checkType<T>();

        if (dataLock.tryWriteLock())
        {
            std::cout << "tryGetDataToWrite succeeded: " << std::endl;
        	writeLockCnt++;
        	lockCntLogger();
            pData = reinterpret_cast<T*>(dataStore);
            return true;
        }
        else
        {
            return false;
        }
    }

    /**
     * Retrieve a pointer on the data to read it
     *
     * @warning The lock has to have been previously acquired
     * @throw Poco::DataFormatException forwarded from checkType()
     */
    template <typename T> T* getDataToRead()
    {
        checkType<T>();

        return reinterpret_cast<T*>(dataStore);
    }

    /**
     * Forward the tryReadLock() call to the data RWLock
     */
    bool tryReadLock()
	{
		if ( dataLock.tryReadLock())
		{
	    	std::cout << "tryReadLock succeeded: " << std::endl;
			readLockCnt++;
			lockCntLogger();
			return true;
		}
		else
			return false;
	}

    /**
     * Forward the readLock() call to the data RWLock
     */
    void readLock()
	{
    	std::cout << "entering readLock" << std::endl;
    	lockCntLogger();

		dataLock.readLock();
		readLockCnt++;
		lockCntLogger();
	}

    /**
     * Release newly created data
     *
     *  - Release the lock
     *  - Notify the data manager that will acquire a read lock if necessary
     */
    void releaseNewData();

    /**
     * Release data that failed to be created
     *
     *  - Release the lock
     *  - expire the data
     */
    void releaseBrokenData();

    /**
     * Unlock the data
     *
     * unlock the data that was previously locked using tryGetDataToWrite
     * or tryReadLock()
     */
    void releaseData()
	{
    	if (writeLockCnt == 1)
    		writeLockCnt--;
    	else if (readLockCnt > 0)
    		readLockCnt--;
    	else
    		poco_bugcheck_msg("DataItem: impossible lock count while releasing");

    	std::cout << "release" << std::endl;
    	lockCntLogger();

    	dataLock.unlock();
	}

    /**
     * Get the parent port
     *
     * @warning the parent port can be NULL
     */
    OutPort* parentPort() { return mParentPort; }

    /**
     * Get the data item data type
     */
    int dataType() { return mDataType; }

    /**
     * Retrieve the data loggers
     *
     * This function calls the DataManager::getDataLogger
     * to retrieve the shared pointers
     */
    std::set< SharedPtr<DataLogger*> > loggers();

    /**
     * Check if loggers are registered
     */
    bool hasLoggers() { return (allLoggers.size()>0); }

    void expire() { expired = true; }
    bool isExpired() { return expired; }

private:
    /**
     * Data type
     *
     * The data type is a combination (logical OR) of
     * a DataTypeEnum value and a DataContainerEnum value
     */
    int mDataType;

    void checkContScalar()
    {
        if (mDataType & contVector)
            throw Poco::DataFormatException("DataItem::checkType",
                    "The data should be contained in a vector");
    }

    void checkContVector()
    {
        if ((mDataType & contVector) == 0)
            throw Poco::DataFormatException("DataItem::checkType",
                    "The data should be contained in a scalar");
    }

    /**
     * @throw Poco::DataFormatException if datatype (without
     * container) does not fit mDataType
     */
    void checkDataType(int datatype);

    void* dataStore; ///< pointer to the data

    DataAttribute attribute; ///< data attribute

    bool expired;

    RWLock dataLock; ///< lock to manage the access to the dataStore
    size_t readLockCnt;
    size_t writeLockCnt;

    void lockCntLogger();

    OutPort* mParentPort; ///< parent output data port.

    friend class DataLogger;

    void registerLogger(DataLogger* logger);
    void detachLogger(DataLogger* logger);

    std::set<DataLogger*> allLoggers;
    RWLock loggersLock;
};

//
// inlines
//
inline std::string DataItem::dataTypeStr(int datatype)
{
    if (datatype == typeUndefined)
        return "undefined";

    std::string desc;

    if (datatype & contVector)
        desc = "vector of ";

    switch (datatype & ~contVector)
    {
    case typeInt32:
        desc += "32-bit integer";
        break;
    case typeUInt32:
        desc += "32-bit unsigned integer";
        break;
    case typeInt64:
        desc += "64-bit integer";
        break;
    case typeUInt64:
        desc += "64-bit unsigned integer";
        break;
    case typeFloat:
        desc += "floating point scalar";
        break;
    case typeDblFloat:
        desc += "double precision floating point scalar";
        break;
    case typeString:
        desc += "character string";
        break;
    default:
        return "";
    }

    return desc;
}

#include "DataItem.ipp"

#endif /* SRC_DATAITEM_H_ */
