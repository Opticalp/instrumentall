/**
 * @file	src/TypeNeutralData.h
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

#ifndef SRC_TYPENEUTRALDATA_H_
#define SRC_TYPENEUTRALDATA_H_

#include "Poco/Exception.h"

/**
 * TypeNeutralData
 *
 * Encapsulate any data transfered in the workflow.
 */
class TypeNeutralData
{
public:
	// TODO ============================================
	// use Poco::Any

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

	TypeNeutralData(int datatype = typeUndefined);
	virtual ~TypeNeutralData();

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
     * @throw Poco::DataFormatException if the type T
     * does not fit mDataType
     */
    template <typename T> void checkType();

    /**
     * Get the data item data type
     */
    int dataType() { return mDataType; }

protected:
    void* dataStore; ///< pointer to the data

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
};

inline std::string TypeNeutralData::dataTypeStr(int datatype)
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

#include "TypeNeutralData.ipp"

#endif /* SRC_TYPENEUTRALDATA_H_ */