/**
 * @file	src/ThreadManager.cpp
 * @date	Mar 2016
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

#include "ThreadManager.h"

#include "Poco/Observer.h"
#include "Poco/NumberFormatter.h"

using Poco::Observer;

ThreadManager::ThreadManager():
        VerboseEntity(name()),
        noMoreThread(false)
{
    taskManager.addObserver(
            Observer<ThreadManager, Poco::TaskStartedNotification>(
                    *this, &ThreadManager::onStarted ) );
    taskManager.addObserver(
            Observer<ThreadManager, Poco::TaskFailedNotification>(
                    *this, &ThreadManager::onFailed ) );
    taskManager.addObserver(
                Observer<ThreadManager, Poco::TaskFinishedNotification>(
                        *this, &ThreadManager::onFinished ) );
}

void ThreadManager::onStarted(Poco::TaskStartedNotification* pNf)
{
    poco_debug(logger(), pNf->task()->name() + " was started");

    noMoreThread.reset();
}

void ThreadManager::onFailed(Poco::TaskFailedNotification* pNf)
{
    Poco::Exception e(pNf->reason());
    poco_error(logger(), e.displayText());
}

void ThreadManager::onFinished(Poco::TaskFinishedNotification* pNf)
{
    poco_debug(logger(), pNf->task()->name() + " has stopped");

    // It seems that we are in the thread that sent the notification,
    // then, it is running...
    if (count() <= 1)
    {
        noMoreThread.set();
        poco_debug(logger(), "No more thread is running. "
                "Setting the corresponding event. ");
    }
}

void ThreadManager::start(Poco::Task* task)
{
    task->duplicate();
    taskManager.start(task);
}

void ThreadManager::waitAll()
{
    // joinAll does not work here,
    // since it seems that it locks the recursive creation of new threads...
    // We use an event instead.
    if (count())
    {
        poco_information(logger(), Poco::NumberFormatter::format(count())
            + " threads are running. Waiting for them to finish. ");

        noMoreThread.wait();
    }
}
