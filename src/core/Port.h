/**
 * @file	src/core/Port.h
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

#include "DataItem.h"
#include <string>

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

    /// Constructor
    Port(Module* parent,
            std::string name,
            std::string description,
            size_t index):
                pParent(parent),
                mName(name), mDescription(description),
                mIndex(index) { }

    /**
     * Constructor to be used by empty ports
     */
    Port(std::string name, std::string description);

    /// Destructor
    virtual ~Port() { }

    /// Get the parent module
    Module* parent() { return pParent; }
    /// get port name
    std::string name() { return mName; }
    /// get port description
    std::string description() { return mDescription; }
    size_t index() { return mIndex; }

private:
    Module* pParent; ///< parent module reference
    std::string mName; ///< port name
    std::string mDescription; ///< port description
    size_t mIndex; ///< index in the module port list
};

#endif /* SRC_PORT_H_ */
