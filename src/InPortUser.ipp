/**
 * @file	src/InPortUser.ipp
 * @date	June 2016
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

#include "InPortUser.h"

#include "InDataPort.h"

template<typename T>
inline void InPortUser::readInPortData(size_t portIndex, T*& pData)
{
    if (inPorts.at(portIndex)->isTrig())
        poco_bugcheck_msg("The port at the given index is a trig port");

    InDataPort* inPort = reinterpret_cast<InDataPort*>(inPorts[portIndex]);

    if (!isInPortCaught(portIndex))
    	poco_bugcheck_msg("try to read an input port that was not previously caught");

    if (!isInPortLocked(portIndex))
    	poco_bugcheck_msg("try to read an input port that was not previously locked");

    inPort->readData<T>(pData);
}

