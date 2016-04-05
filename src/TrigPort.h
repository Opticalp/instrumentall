/**
 * detailed description
 * 
 * @file	TrigPort.h
 * @brief	brief description
 * @date	5 avr. 2016
 * @author	PhRG - opticalp.fr
 *
 * $Id$
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

#ifndef SRC_TRIGPORT_H_
#define SRC_TRIGPORT_H_

#include "InPort.h"

class DataAttribute;

/**
 * TrigPort
 *
 * InPort that accepts any type of data. This kind of port is used to
 * trig the runTask (see Dispatcher::setOutPortDataReady)
 */
class TrigPort: public InPort
{
public:
    TrigPort(Module* parent,
            std::string name,
            std::string description,
            size_t index);

    /**
     * Special constructor for the empty TrigPort
     */
    TrigPort(OutPort* emptySourcePort);

    virtual ~TrigPort() { }

    bool tryDataAttribute(DataAttribute* pAttr);
};

#endif /* SRC_TRIGPORT_H_ */