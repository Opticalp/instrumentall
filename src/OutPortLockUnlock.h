/**
 * @file	src/OutPortLockUnlock.h
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

#ifndef SRC_OUTPORTLOCKUNLOCK_H_
#define SRC_OUTPORTLOCKUNLOCK_H_

#include "OutPort.h"
#include "Poco/Exception.h"

/**
 * OutPortLockUnlock
 *
 *
 * Convenience class inspired from the scoped lock design pattern.
 *
 * On object destruction:
 * Release the output ports locks that were acquired via this class.
 */
class OutPortLockUnlock
{
public:
    OutPortLockUnlock(std::vector<OutPort*> outputPorts):
        outPorts(outputPorts)
    { lockFlags.assign(outputPorts.size(), false); }

    /**
     * Destructor: release the ports that were not unlocked
     */
    virtual ~OutPortLockUnlock()
    {
        for (size_t index = 0; index < outPorts.size(); index++)
        {
            if (lockFlags[index])
                outPorts[index]->releaseOnFailure();
        }
    }

    /**
     * Forward tryData for the given port
     */
    template <typename T>
    bool tryData(size_t portIndex, T*& pData);

    /**
     * Forward releaseData to the given port
     */
    void notifyReady(size_t portIndex, DataAttributeOut attribute);

private:
    std::vector<OutPort*> outPorts;

    /**
     * Flag array to store which output was locked using this class
     */
    std::vector<bool> lockFlags;
};

template<typename T>
inline bool OutPortLockUnlock::tryData(size_t portIndex, T*& pData)
{
    OutPort* outPort = outPorts.at(portIndex);

    if (lockFlags[portIndex])
        poco_bugcheck_msg("try to re-lock an input port that was already locked? ");


    bool retValue = outPort->tryData<T>(pData);

    if (retValue)
        lockFlags[portIndex] = true;

    return retValue;
}

inline void OutPortLockUnlock::notifyReady(size_t portIndex,
        DataAttributeOut attribute)
{
    if (!lockFlags[portIndex])
        poco_bugcheck_msg("try to unlock an output port "
                "that was not already locked? ");

    outPorts[portIndex]->notifyReady(attribute);

    lockFlags[portIndex] = false;
}

#endif /* SRC_OUTPORTLOCKUNLOCK_H_ */
