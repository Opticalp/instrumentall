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
        typeString
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
     * @throw Poco::DataFormatException is the type T
     * does not fit mDataType
     */
    template <typename T> void checkType();

    DataAttribute getDataAttribute() { return attribute; }

    void setDataAttribute(DataAttribute attr) { attribute = attr; }

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
        { return dataLock.tryReadLock(); }

    /**
     * Forward the readLock() call to the data RWLock
     */
    void readLock()
        { dataLock.readLock(); }

    /**
     * Release newly created data
     *
     *  - Release the lock
     *  - Notify the data manager that will acquire a read lock if necessary
     */
    void releaseNewData();

    /**
     * Unlock the data
     *
     * unlock the data that was previously locked using tryGetDataToWrite
     * or tryReadLock()
     */
    void releaseData()
        { dataLock.unlock(); }

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

//    bool expired;

    RWLock dataLock; ///< lock to manage the access to the dataStore

    // TODO: separate volatile data (issue from a outport.push(data)) in UI
    // and other data, that do really have a parent port.

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
        return "32-bit integer";
    case typeUInt32:
        return "32-bit unsigned integer";
    case typeInt64:
        return "64-bit integer";
    case typeUInt64:
        return "64-bit unsigned integer";
    case typeFloat:
        return "floating point scalar";
    case typeDblFloat:
        return "double precision floating point scalar";
    case typeString:
        return "character string";
    default:
        return "";
    }
}

#include "DataItem.ipp"

#endif /* SRC_DATAITEM_H_ */
