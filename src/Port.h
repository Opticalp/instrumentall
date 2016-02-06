/**
 * @file	src/Port.h
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

#ifndef SRC_PORT_H_
#define SRC_PORT_H_

class Module;

/**
 * Port
 *
 * Class representing a module data port, either a data input port, or
 * a data output port.
 */
class Port
{
public:
    /**
     * Data type descriptor
     */
    enum dataTypeEnum
    {
        typeInteger,
        typeFloat,
        typeString,
        typeCnt
    };

    /// Constructor
    Port(Module* parent,
            std::string name,
            std::string description,
            dataTypeEnum datatype,
            size_t index):
                pParent(parent),
                mName(name), mDescription(description),
                mType(datatype), mIndex(index) { }
    /// Destructor
    virtual ~Port() { }

    /// Get the parent module
    Module* parent() { return pParent; }
    /// get port name
    std::string name() { return mName; }
    /// get port description
    std::string description() { return mDescription; }
    /// get port data type
    dataTypeEnum dataType() { return mType; }
    /// get port data type as a character string
    std::string dataTypeStr();
    /// get the index in the port list
    size_t index();

private:
    Module* pParent; ///< parent module reference
    std::string mName; ///< port name
    std::string mDescription; ///< port description
    dataTypeEnum mType; ///< port data type
    size_t mIndex; ///< index in the module port list
};


inline std::string Port::dataTypeStr()
{
    switch (mType)
    {
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

#endif /* SRC_PORT_H_ */
