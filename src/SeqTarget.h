/**
 * @file	/src/SeqTarget.h
 * @date	Jul. 2016
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

#ifndef SRC_SEQTARGET_H_
#define SRC_SEQTARGET_H_

#include "DataTarget.h"

class SeqSource;
class DataAttributeIn;

/**
 * SeqTarget
 *
 * Data sequence target.
 * Inherited from DataTarget since it can not be a seqTarget
 * if it is not a DataTarget
 */
class SeqTarget: public DataTarget
{
public:
	SeqTarget(): seqSource(NULL) { }
	virtual ~SeqTarget();

	/**
	 * Retrieve the data sequence source port
	 */
	SeqSource* getSeqSource();

    /**
     * Read the data attribute of the incoming data
     *
     * The port shall have been previously locked using
     * tryLock, with return value == true.
     */
	void readDataAttribute(DataAttributeIn* pAttr);

protected:
	/**
	 * Register a new data sequence source
	 *
	 * The data sequence source has to be unique.
	 * Setting a new data sequence source frees the
	 * previous data sequence source.
	 *
	 * Call SeqSource::AddSeqTarget
	 */
	void setSeqSource(SeqSource* source);

	/**
	 * Release the seq source
	 *
	 * and replace it by NULL
	 */
	void releaseSeqSource();

private:
    SeqSource* seqSource;
    Poco::FastMutex seqSourceLock; ///< lock for the seqSource operations

    friend class Dispatcher;
};

#endif /* SRC_SEQTARGET_H_ */
