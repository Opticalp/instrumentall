/**
 * @file	src/InPortLockUnlock.h
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

#ifndef SRC_INPORTLOCKUNLOCK_H_
#define SRC_INPORTLOCKUNLOCK_H_

#include "InPort.h"
#include "Poco/Exception.h"

/**
 * InPortLockUnlock
 *
 * Convenience class inspired from the scoped lock design pattern.
 *
 * On object destruction:
 * Release the input ports locks that were acquired via this class.
 */
class InPortLockUnlock
{
public:
    InPortLockUnlock(std::vector<InPort*> inputPorts):
        inPorts(inputPorts)
    { lockFlags.assign(inputPorts.size(), false); }

    /**
     * Destructor: release the ports that were not unlocked
     */
    virtual ~InPortLockUnlock()
    {
        for (size_t index = 0; index < inPorts.size(); index++)
            releaseData(index);
    }

    /**
     * Forward tryData for the given port
     */
    template <typename T>
    bool tryData(size_t portIndex, T*& pData, DataAttributeIn* pAttr);

    /**
     * Forward releaseData to the given port
     */
    void releaseData(size_t portIndex);

private:
    std::vector<InPort*> inPorts;

    /**
     * Flag array to store which input was locked using this class
     */
    std::vector<bool> lockFlags;
};

template<typename T>
inline bool InPortLockUnlock::tryData(size_t portIndex, T*& pData,
        DataAttributeIn* pAttr)
{
    InPort* inPort = inPorts.at(portIndex);

    if (lockFlags[portIndex])
        poco_bugcheck_msg("try to re-lock an input port that was already locked? ");


    bool retValue = inPort->tryData<T>(pData, pAttr);

    if (retValue)
        lockFlags[portIndex] = true;

    return retValue;
}

inline void InPortLockUnlock::releaseData(size_t portIndex)
{
    if (lockFlags[portIndex])
        inPorts[portIndex]->releaseData();

    lockFlags[portIndex] = false;
}

#endif /* SRC_INPORTLOCKUNLOCK_H_ */
