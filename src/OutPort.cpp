/**
 * @file	src/OutPort.cpp
 * @date	feb. 2016
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

#include "OutPort.h"

OutPort::OutPort(Module* parent,
        std::string name,
        std::string description,
        dataTypeEnum datatype,
        size_t index):
    Port(parent, name, description, datatype, index)
{
    // TODO Auto-generated constructor stub

}

OutPort::~OutPort()
{
    // TODO Auto-generated destructor stub
}

std::vector<SharedPtr<OutPort*> > OutPort::getTargetPorts()
{
    // TODO

    lock.readLock();
    // create a copy
    lock.unlock();
    // return the copy
    return std::vector<SharedPtr<OutPort*> >();
}

void OutPort::addTargetPort(InPort* port)
{
    // TODO
    // get shared ptr from the dispatcher
    // add it here

    lock.writeLock();
    // targetPorts.push_back( <shared ptr> );
    lock.unlock();
}

void OutPort::removeTargetPort(InPort* port)
{
    lock.writeLock();
    for (std::vector< SharedPtr<InPort*> >::iterator it=targetPorts.begin(), ite=targetPorts.end();
            it != ite; it++ )
    {
        if (**it==port)
        {
            targetPorts.erase(it);
            lock.unlock();
            return;
        }
    }

    lock.unlock();
}
