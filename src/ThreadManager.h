/**
 * @file	src/ThreadManager.h
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

#ifndef SRC_THREADMANAGER_H_
#define SRC_THREADMANAGER_H_

#include "VerboseEntity.h"

#include "Poco/TaskManager.h"
#include "Poco/TaskNotification.h"
#include "Poco/Util/Subsystem.h"

/**
 * ThreadManager
 *
 * Contain the task manager of the system that is used to launch each
 * {{{Module::runTask}}} and the data logger tasks
 */
class ThreadManager: public Poco::Util::Subsystem, VerboseEntity
{
public:
    /**
     * Constructor
     *
     * Add the observers
     */
    ThreadManager();
    virtual ~ThreadManager() { }

    /**
      * Subsystem name
      *
      * Shall not contain spaces: see setLogger() call in initialize().
      */
     const char * name() const { return "ThreadManager"; }

     /// @name un/re/initialization methods
     ///@{
     /**
      * Initialize
      *
      */
     void initialize(Poco::Util::Application& app) { }

     // void reinitialize(Application & app); // not needed. By default: uninit, then init.

     /**
      *
      *
      *
      */
     void uninitialize() { }
     ///@}

    void onStarted(Poco::TaskStartedNotification* pNf);
//    void onProgress(Poco::TaskProgressNotification* pNf);
    void onFailed(Poco::TaskFailedNotification* pNf);
    void onFinished(Poco::TaskFinishedNotification* pNf);

    /**
     * Count the active tasks
     */
    int count() { return taskManager.count(); }

    /**
     * Cancel all the tasks
     *
     * Cancel module tasks and logging tasks whenever possible
     */
    void cancelAll() { taskManager.cancelAll(); }

    /**
     * Start a task
     *
     * Duplicate the task before launching it in order that
     * the task manager does not delete it after execution
     */
    void start(Poco::Task* task);

    /**
     * Wait for all tasks to be terminated
     */
    void waitAll();

private:
    Poco::TaskManager taskManager;
};

#endif /* SRC_THREADMANAGER_H_ */
