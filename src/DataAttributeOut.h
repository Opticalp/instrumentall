/**
 * @file	src/DataAttributeOut.h
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

#include "DataAttribute.h"

/**
 * DataAttributeOut
 *
 * DataAttribute used to be sent to the Dispatcher by any OutPort.
 * It can notify the Dispatcher that it has to generate a start sequence.
 */
class DataAttributeOut: public DataAttribute
{
public:
	DataAttributeOut():seqInfo(undefSeqInfo) { }
	virtual ~DataAttributeOut();

    DataAttributeOut(const DataAttribute& other):
    	DataAttribute(other), seqInfo(undefSeqInfo) { }
    DataAttributeOut(const DataAttributeOut& other):
    	DataAttribute(other), seqInfo(other.seqInfo) { }

    DataAttributeOut& operator =(const DataAttribute& other);
	DataAttributeOut& operator =(const DataAttributeOut& other);

	void startSequence() { seqInfo = startSeqInfo; }
    void continueSequence() { seqInfo = contSeqInfo; }
	void endSequence() { seqInfo = endSeqInfo; }

	bool isStartSequence() { return (seqInfo == startSeqInfo); }
    bool isContinueSequence() { return (seqInfo == contSeqInfo); }
	bool isEndSequence() { return (seqInfo == endSeqInfo); }

    static DataAttributeOut newDataAttribute();

private:
    static size_t nextToBeUsedIndex; ///< next available data index
    static Poco::Mutex lock; ///< lock to safely use and increment nextToBeUsedIndex

enum SeqInfoEnum
	{
		undefSeqInfo, // no sequence info
		startSeqInfo, // start sequence
		contSeqInfo, // continue sequence
		endSeqInfo, // end sequence
		seqInfoCnt
	};

	SeqInfoEnum seqInfo;
};

#endif /* DATAATTRIBUTEOUT_H_ */
