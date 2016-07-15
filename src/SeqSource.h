/**
 * @file	src/SeqSource.h
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

#ifndef SRC_SEQSOURCE_H_
#define SRC_SEQSOURCE_H_

#include "DataSource.h"

#include "Poco/RWLock.h"
#include "Poco/SharedPtr.h"

#include <vector>

using Poco::RWLock;
using Poco::SharedPtr;

class InDataPort;
class InPort;

/**
 * SeqSource
 *
 * Data sequence source.
 * Inherited from DataSource since it can not be a SeqSource if it
 * is not a DataSource
 */
class SeqSource: public DataSource
{
public:
	SeqSource(int datatype): DataSource(datatype) { }
	SeqSource() { }
	virtual ~SeqSource() { }

    /**
     * Retrieve the data sequence target ports
     */
    std::vector< SharedPtr<InPort*> > getSeqTargetPorts();

protected:
    /**
     * Add a sequence re-combiner target port
     *
     * This function should only be called by the seq target InDataPort.
     *
     * The Dispatcher is requested to get the shared pointer
     * on the InDataPort.
     */
    void addSeqTargetPort(InDataPort* port);

    /**
     * Remove a sequence re-combiner target port
     *
     * Should not throw an exception if the port is not present
     * in the seqTargetPorts
     */
    void removeSeqTargetPort(InDataPort* port);

    /// list of target ports for data sequence re-combination
    std::vector< SharedPtr<InPort*> > seqTargetPorts;
    RWLock seqTargetPortsLock; ///< lock for seqTargetPorts operations

    friend class InDataPort;
};

#endif /* SRC_SEQSOURCE_H_ */
