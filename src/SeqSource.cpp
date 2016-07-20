/**
 * detailed comment
 * 
 * @file	/Instrumentall-Debug@instru-git-debug/[Source directory]/src/SeqSource.cpp
 * @brief	short comment
 * @date	15 juil. 2016
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

#include "SeqSource.h"

#include "Dispatcher.h"

#include "Poco/Util/Application.h"

std::vector<SharedPtr<InPort*> > SeqSource::getSeqTargetPorts()
{
    std::vector<SharedPtr<InPort*> > list;

    seqTargetPortsLock.readLock();
    list = seqTargetPorts;
    seqTargetPortsLock.unlock();

    return list;
}

void SeqSource::addSeqTargetPort(InDataPort* port)
{
    Poco::ScopedRWLock lock(seqTargetPortsLock, true);

    SharedPtr<InPort*> sharedPort =
        Poco::Util::Application::instance()
                    .getSubsystem<Dispatcher>()
                    .getInPort(port);
        seqTargetPorts.push_back(sharedPort);
}

void SeqSource::removeSeqTargetPort(InDataPort* port)
{
    Poco::ScopedRWLock lock(seqTargetPortsLock, true);

    for (std::vector< SharedPtr<InPort*> >::iterator it=seqTargetPorts.begin(),
            ite=seqTargetPorts.end(); it != ite; it++ )
    {
        if (**it==port)
        {
            seqTargetPorts.erase(it);
            return;
        }
    }
}
