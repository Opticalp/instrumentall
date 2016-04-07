/**
 * @file	src/InPort.cpp
 * @date	Apr. 2016
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

#include "InPort.h"

#include "OutPort.h"
#include "Dispatcher.h"
#include "ModuleManager.h"

#include "Poco/Util/Application.h"

InPort::InPort(Module* parent, std::string name, std::string description,
        int datatype, size_t index, bool trig):
        Port(parent, name, description, datatype, index),
        used(true), plugged(false),
        isTrigFlag(trig)
{
    mSourcePort = SharedPtr<OutPort*>(
            new (OutPort*)( Poco::Util::Application::instance()
                                    .getSubsystem<Dispatcher>()
                                    .getEmptyOutPort()       ) );
}

InPort::InPort(OutPort* emptySourcePort, std::string name,
        std::string description, bool trig):
                Port(Poco::Util::Application::instance()
                    .getSubsystem<ModuleManager>()
                    .getEmptyModule(), name, description,
                    DataItem::typeUndefined, 0),
                used(false), plugged(false),
                isTrigFlag(trig)
{
    mSourcePort = SharedPtr<OutPort*>( new (OutPort*)(emptySourcePort) );
}

void InPort::setNew(bool value)
{
    if (value)
        (*getSourcePort())->dataItem()->readLock();

    used = !value;
}

void InPort::release()
{
    setNew(false);
    (*getSourcePort())->dataItem()->releaseData();
}

void InPort::setSourcePort(SharedPtr<OutPort*> port)
{
    if (!plugged)
        (*mSourcePort)->removeTargetPort(this);

    mSourcePort = port;
    try
    {
        (*mSourcePort)->addTargetPort(this);
        plugged = true;
    }
    catch (DispatcherException& e)
    {
        mSourcePort = SharedPtr<OutPort*>(
                new (OutPort*)( Poco::Util::Application::instance()
                                        .getSubsystem<Dispatcher>()
                                        .getEmptyOutPort()       ) );
        e.rethrow();
    }
}

void InPort::releaseSourcePort()
{
    if (!plugged)
        return;

    (*mSourcePort)->removeTargetPort(this);
    mSourcePort = SharedPtr<OutPort*>(
            new (OutPort*)( Poco::Util::Application::instance()
                                    .getSubsystem<Dispatcher>()
                                    .getEmptyOutPort()       ) );
    plugged = false;
}
