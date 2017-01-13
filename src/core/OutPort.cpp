/**
 * @file	src/core/OutPort.cpp
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

#include "Module.h"

#include "Dispatcher.h"
#include "DataManager.h"
#include "DataLogger.h"

#include "Poco/Util/Application.h"

OutPort::OutPort(Module* parent,
        std::string name,
        std::string description,
        int datatype,
        size_t index):
    Port(parent, name, description, index),
	SeqSource(datatype)
{
//    // notify the DataManager of the creation
//    Poco::Util::Application::instance()
//                            .getSubsystem<DataManager>()
//                            .addOutPort(this);
}

OutPort::OutPort():
        Port("emptyOut", "replace an expired port")
{
    // nothing to do
}

void OutPort::sourceCancel()
{
	parent()->immediateCancel();
}

void OutPort::sourceWaitCancelled()
{
	parent()->waitCancelled();
}

void OutPort::sourceReset()
{
	parent()->moduleReset();
}
