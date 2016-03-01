/**
 * @file	src/DataAttributeIn.h
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

#ifndef SRC_DATAATTRIBUTEIN_H_
#define SRC_DATAATTRIBUTEIN_H_

#include "DataAttribute.h"

/**
 * DataAttributeIn
 *
 * DataAttribute specialized by the input port in the tryData query
 * to leave the ability to this DataAttribute to query itself about
 * the startSequence and endSequence, comparing its internal lists.
 */
class DataAttributeIn: public DataAttribute
{
public:
    DataAttributeIn(DataAttribute attribute, InPort* parent):
        DataAttribute(attribute), mParent(parent) { }

    DataAttributeIn(): mParent(NULL) { }

    virtual ~DataAttributeIn() { }

    DataAttributeIn& operator =(const DataAttributeIn& other);

    /**
     * Check if the InPort is concerned by a startSequence
     *
     * and then remove the InPort from the list
     */
    bool isStartSequence()
        { return DataAttribute::isStartSequence(mParent); }

    bool isEndSequence()
        { return DataAttribute::isEndSequence(mParent); }

private:
    InPort* mParent;
};

#endif /* SRC_DATAATTRIBUTEIN_H_ */
