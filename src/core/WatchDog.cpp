/**
 * @file	src/core/WatchDog.cpp
 * @date	Feb. 2017
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

#include "WatchDog.h"

#include "ThreadManager.h"

#include "Poco/Thread.h"

void WatchDog::run()
{
    stopMe.reset();

    if (active)
        return;

    active = true;

    // init taskListFrozen query
    threadMan->taskListFrozen(true);
    threadMan->threadCountNotChanged(true);

    // loop: check that the tasks change.
    // if not, check that the thread count change.
    // else, cancelAll. (if tasks count != 0 of thread count != self)
    while (!stopMe.tryWait(timeout))
    {
        if (threadMan->taskListFrozen() && threadMan->threadCountNotChanged())
        {
            threadMan->cancelAllFromWatchDog();
            break;
        }
    }

    active = false;
}

void WatchDog::stop()
{
    stopMe.set();
    Poco::Thread::yield();
}
