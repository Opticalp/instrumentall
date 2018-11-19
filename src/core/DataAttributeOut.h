/**
 * @file	src/core/DataAttributeOut.h
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

#ifndef DATAATTRIBUTEOUT_H_
#define DATAATTRIBUTEOUT_H_

#include "DataAttributeIn.h"

/**
 * DataAttributeOut
 *
 * DataAttribute used to be sent to the Dispatcher by any OutPort.
 * Manage data sequence generation.
 *
 * Typical use case in Module::process:
 *
 *     AttributeOut attr; // or got it from intput data attribute
 *
 *     attr.startSequence(); // start main seq
 *
 *     for (index1 < MAX_index1+1)
 *     {
 *       if (index1 == MAX_index1)
 *         attr.endSequence(); // end main seq
 *
 *       attr.startSequence(); // start included seq
 *
 *       for (index2 < MAX_index2+1)
 *       {
 *         // do things, prepare data to output
 *
 *         if (index2 == MAX_index2)
 *           attr.endSequence(); // end included seq
 *
 *         outPortsAccess.notifyReady(outPortA, attr);
 *         attr++; // increment attribute index, start becomes continue, stop becomes nothing.
 *       }
 *     }
 *
 */
class DataAttributeOut: public DataAttribute
{
public:
	/**
	 * Standard constructor
	 *
	 * Create a new DataAttribute with a never-used data index
	 */
	DataAttributeOut();

	/**
	 * Copy constructor
	 */
    DataAttributeOut(const DataAttributeOut& other);

    /**
     * Constructor importing a DataAttributeIn
     */
    DataAttributeOut(const DataAttributeIn& other);

    /**
     * Constructor importing a DataAttribute
     */
    DataAttributeOut(const DataAttribute& other);

	virtual ~DataAttributeOut() { }

    DataAttributeOut& operator =(const DataAttribute& other);
    DataAttributeOut& operator =(const DataAttributeIn& other);
	DataAttributeOut& operator =(const DataAttributeOut& other);

	DataAttributeOut& operator +=(const DataAttributeOut& rhs);

	// friends defined inside class body are inline and are hidden from non-ADL lookup
	friend DataAttributeOut operator+(DataAttributeOut lhs,  // passing lhs by value helps optimize chained a+b+c
		const DataAttributeOut& rhs)       // otherwise, both parameters may be const references
	{
		lhs += rhs; // reuse compound assignment
		return lhs; // return the result by value (uses move constructor)
	}

	/**
	 * Prefix ++ operator
	 */
	DataAttributeOut& operator ++();

	/**
	 * Postfix ++ operator
	 */
	DataAttributeOut operator ++(int);

	void startSequence();
	void endSequence();

	bool isSettingSequence() { return seqManaging != 0; }

private:
    static size_t nextToBeUsedIndex; ///< next available data index
    static size_t nextToBeUsedSeqIndex; ///< next available sequence index
    static Poco::Mutex lock; ///< lock to safely use and increment nextToBeUsedIndex

    size_t newIndex; ///< last generated index in the standard constructor
    size_t seqManaging;

    void swap (DataAttributeOut& other);

    void appendNewIndex();
};

#endif /* DATAATTRIBUTEOUT_H_ */
