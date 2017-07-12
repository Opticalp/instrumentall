/**
 * @file	src/modules/control/SeqAccumulator.ipp
 * @date	Jul. 2017
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2017 Ph. Renaud-Goud / Opticalp

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

#include "SeqAccumulator.h"

template <typename T>
void SeqAccumulator::sendData(std::vector<T>& input, DataAttributeIn attr)
{
	DataAttributeOut attrOut(attr);

	size_t inSize = input.size();

	if (inSize == 0)
	{
		poco_warning(logger(), "The input array is empty. "
				"Can not send empty data");
		return;
	}

    reserveOutPort(dataOutPort);
    poco_information(logger(),"out port reserved");

    T* pData;
    getDataToWrite<T>(dataOutPort, pData);
	*pData = input[0];

    if (inSize == 1)
    {
    	poco_information(logger(), "only one element to be sent. No sequence then. ");
    	notifyOutPortReady(dataOutPort, attrOut);
    	return;
    }

	attrOut.startSequence();
	for (size_t ind = 1; ind < inSize; ind++)
	{
    	notifyOutPortReady(dataOutPort, attrOut);
    	attrOut++;
        reserveOutPort(dataOutPort);
        getDataToWrite<T>(dataOutPort, pData);
    	*pData = input[ind];
	}

	attrOut.endSequence();
	notifyOutPortReady(dataOutPort, attrOut);
}
