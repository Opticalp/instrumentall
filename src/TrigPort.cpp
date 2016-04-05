/**
 * detailed description
 * 
 * @file	TrigPort.cpp
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

#include "TrigPort.h"
#include "ModuleManager.h"

TrigPort::TrigPort(Module* parent, std::string name, std::string description,
        size_t index):
        InPort(parent, name, description, DataItem::typeUndefined, index, true)
{
}

TrigPort::TrigPort(OutPort* emptySourcePort):
                InPort(emptySourcePort,
                       "emptyTrig", "replace an expired port",
                       true)
{
}

bool TrigPort::tryDataAttribute(DataAttribute* pAttr)
{
    if (!isNew())
        return false;

    *pAttr = (*getSourcePort())->dataItem()->getDataAttribute();
    return true;
}
