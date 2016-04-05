/**
 * @file	src/InDataPort.cpp
 * @date	fev. 2016
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

#include "InDataPort.h"

#include "ModuleManager.h"
#include "Dispatcher.h"
#include "OutPort.h"

InDataPort::InDataPort(Module* parent,
        std::string name,
        std::string description,
        int datatype,
        size_t index):
    Port(parent, name, description, datatype, index),
    used(true), held(false)
{
    mSourcePort = SharedPtr<OutPort*>(
            new (OutPort*)( Poco::Util::Application::instance()
                                    .getSubsystem<Dispatcher>()
                                    .getEmptyOutPort()       ) );
    mSeqSourcePort = mSourcePort;
}

InDataPort::InDataPort(OutPort* emptySourcePort):
        Port(Poco::Util::Application::instance()
                    .getSubsystem<ModuleManager>()
                    .getEmptyModule(),
                "emptyIn", "replace an expired port",
                DataItem::typeUndefined, 0),
        used(true), held(false)
{
    mSourcePort = SharedPtr<OutPort*>( new (OutPort*)(emptySourcePort) );
    mSeqSourcePort = mSourcePort;
}

void InDataPort::setSourcePort(SharedPtr<OutPort*> port)
{
    (*mSourcePort)->removeTargetPort(this);
    mSourcePort = port;
    try
    {
        (*mSourcePort)->addTargetPort(this);
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

void InDataPort::releaseSourcePort()
{
    (*mSourcePort)->removeTargetPort(this);
    mSourcePort = SharedPtr<OutPort*>(
            new (OutPort*)( Poco::Util::Application::instance()
                                    .getSubsystem<Dispatcher>()
                                    .getEmptyOutPort()       ) );
}

SharedPtr<OutPort*> InDataPort::getSourcePort()
{
    return mSourcePort;
}

void InDataPort::setSeqSourcePort(SharedPtr<OutPort*> port)
{
    (*mSeqSourcePort)->removeSeqTargetPort(this);
    mSeqSourcePort = port;
    try
    {
        (*mSeqSourcePort)->addSeqTargetPort(this);
    }
    catch (DispatcherException& e)
    {
        mSeqSourcePort = SharedPtr<OutPort*>(
                new (OutPort*)( Poco::Util::Application::instance()
                                        .getSubsystem<Dispatcher>()
                                        .getEmptyOutPort()       ) );
        e.rethrow();
    }
}

void InDataPort::releaseSeqSourcePort()
{
    (*mSeqSourcePort)->removeSeqTargetPort(this);
    mSeqSourcePort = SharedPtr<OutPort*>(
            new (OutPort*)( Poco::Util::Application::instance()
                                    .getSubsystem<Dispatcher>()
                                    .getEmptyOutPort()       ) );
}

SharedPtr<OutPort*> InDataPort::getSeqSourcePort()
{
    return mSeqSourcePort;
}

void InDataPort::releaseData()
{
    setNew(false);
    (*getSourcePort())->dataItem()->releaseData();

    // TODO: update expiration information?
}

void InDataPort::setNew(bool value)
{
    if (value)
    {
        // poco_information(Poco::Logger::get("portDebug"), name() + " has new data. ");
        (*getSourcePort())->dataItem()->readLock();
    }

    used = !value;
}

bool InDataPort::isUpToDate()
{
	// TODO: should check if the input data is in a sequence?
	// inside a sequence, the holding should be impossible?
	if (!held)
		return false;
	else
		return (!(*getSourcePort())->dataItem()->isExpired());
}
