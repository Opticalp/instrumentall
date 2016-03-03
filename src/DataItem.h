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

class OutPort;

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
        typeUndefined, /// to be used in empty ports
        typeInteger,
        typeFloat,
        typeString,
        typeCnt
    };

    DataItem(DataTypeEnum dataType = typeUndefined);
    virtual ~DataItem();

    /// get port data type as a character string
    static std::string dataTypeStr(DataTypeEnum dataType);

    DataAttribute getDataAttribute() { return attribute; }

    void setDataAttribute(DataAttribute attr) { attribute = attr; }

    template <typename T> bool tryGetDataToWrite(T*& pData)
    {
        // TODO: check type
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
     */
    template <typename T> T* getDataToRead()
    {
        // TODO: check type
        return reinterpret_cast<T*>(dataStore);
    }

    bool tryReadLock()
        { return dataLock.tryReadLock(); }

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

private:
    DataTypeEnum mDataType; ///< data type
    void* dataStore; ///< pointer to the data

    DataAttribute attribute; ///< data attribute

//    bool expired;

    Poco::RWLock dataLock; ///< lock to manage the access to the dataStore

    // TODO: separate volatile data (issue from a outport.push(data)) in UI
    // and other data, that do really have a parent port.

    OutPort* parentPort; ///< parent output data port.
};

//
// inlines
//
inline std::string DataItem::dataTypeStr(DataTypeEnum dataType)
{
    switch (dataType)
    {
    case typeUndefined:
        return "undefined";
    case typeInteger:
        return "integer";
    case typeFloat:
        return "floating point scalar";
    case typeString:
        return "character string";
    default:
        return "";
    }
}


#endif /* SRC_DATAITEM_H_ */
