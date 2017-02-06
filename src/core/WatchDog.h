/**
 * @file	src/core/WatchDog.h
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

#ifndef SRC_CORE_WATCHDOG_H_
#define SRC_CORE_WATCHDOG_H_

#include "Poco/Runnable.h"
#include "Poco/Event.h"

class ThreadManager;

/**
 * Runnable to check that the task list is evoluting
 */
class WatchDog: public Poco::Runnable
{
public:
    WatchDog(ThreadManager* parent, long milliseconds):
        threadMan(parent), timeout(milliseconds),
        active (false), stopMe(false)
        {   }

    /**
     * Start the watchdog
     */
    void run();

    /**
     * Set timeout in milliseconds
     */
    void setTimeout(long milliseconds)
        { timeout = milliseconds; }

    /**
     * @return 1 if active, 0 else.
     */
    int isActive()
        { if (active) return 1; else return 0; }

    void stop();

private:
    ThreadManager* threadMan;

    long timeout; ///< timeout in milliseconds

    bool active; ///< activity flag
    Poco::Event stopMe; ///< stop request flag
};

#endif /* SRC_CORE_WATCHDOG_H_ */
