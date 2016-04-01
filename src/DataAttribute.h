/**
 * @file	src/DataAttribute.h
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

#ifndef SRC_DATAATTRIBUTE_H_
#define SRC_DATAATTRIBUTE_H_

#include "Poco/Mutex.h"
#include "Poco/RWLock.h"

#include <set>

class InPort;

/**
 * DataAttribute
 *
 * Store informations about the data.
 * Those informations can be:
 *  - data index(es)
 *  - sequence information
 *  - ... (other?)
 *
 * Important operators are :
 *  - copy
 *  - merge (+): concatenate the lists
 *
 * Typical use cases:
 *  - in a module that process data from input data, get the attributes
 * from the input ports data, merge their attributes, then set it to the
 * output ports data (as DataAttributeOut)
 *
 * No need to lock the read/write since each attribute is supposed to be
 * used only in one DataItem, and the DataItem should be locked when accessed.
 * Then, the lock of the DataItem should be enough.
 */
class DataAttribute
{
public:
    /**
     * Standard constructor
     *
     * Create empty attributes. To be used at the DataItem creation,
     * when no data index is defined.
     */
    DataAttribute(): startCnt(0), contCnt(0), endCnt(0) { }

    /// Copy constructor
    DataAttribute(const DataAttribute& other);
    virtual ~DataAttribute();

    DataAttribute& operator =(const DataAttribute& other);

    DataAttribute& operator +=(const DataAttribute& rhs);

    // friends defined inside class body are inline and are hidden from non-ADL lookup
    friend DataAttribute operator+(DataAttribute lhs,  // passing lhs by value helps optimize chained a+b+c
                       const DataAttribute& rhs)       // otherwise, both parameters may be const references
    {
      lhs += rhs; // reuse compound assignment
      return lhs; // return the result by value (uses move constructor)
    }

    void appendStartSeqPortTarget(InPort* port)
        { startSequenceTargets.insert(port); }

    void appendContSeqPortTarget(InPort* port)
        { contSequenceTargets.insert(port); }

    void appendEndSeqPortTarget(InPort* port)
        { endSequenceTargets.insert(port); }

protected:
    /**
     * Swap content with another DataAttribute
     *
     * Used in operator = to avoid self-affectation issues.
     */
    void swap(DataAttribute& other);

    /**
     * Add a data index to the current list
     */
    void appendIndex(size_t index)
    	{ indexes.insert(index); }

    /**
     * Check if the given port is targeted by a startSequence
     *
     * Check if the InPort is in the startSequences list
     * and remove it if present
     */
    bool isStartSequence(InPort* port);

    /**
     * Check if the given port is targeted by a contSequence
     *
     * Check if the InPort is in the contSequences list
     * and remove it if present
     */
    bool isContinueSequence(InPort* port);

    /**
     * Check if the given port is targeted by a endSequence
     *
     * Check if the InPort is in the endSequences list
     * and remove it if present
     */
    bool isEndSequence(InPort* port);

    /// Increment startCnt
    void incStartSeq(size_t imbrication) { startCnt += imbrication; }
    /// Increment contCnt
    void incContSeq(size_t imbrication) { contCnt += imbrication; }
    /// Increment endCnt
    void incEndSeq(size_t imbrication) { endCnt += imbrication; }

private:
    std::set<size_t> indexes;
    std::set<InPort*> startSequenceTargets;
    std::set<InPort*> contSequenceTargets;
    std::set<InPort*> endSequenceTargets;

    size_t startCnt; ///< imbricated start
    size_t contCnt; ///< imbricated continue
    size_t endCnt; ///< imbricated stop
};

#endif /* SRC_DATAATTRIBUTE_H_ */
