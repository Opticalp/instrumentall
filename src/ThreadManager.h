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

#include "ModuleTask.h"
#include "TaskManager.h"
#include "TaskNotification.h"

#include "Poco/ThreadPool.h"
#include "Poco/Runnable.h"
#include "Poco/TaskNotification.h"
#include "Poco/Util/Subsystem.h"
#include "Poco/RWLock.h"
#include "Poco/AutoPtr.h"

#include <set>

class DataLogger;

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
     void initialize(Poco::Util::Application& app)
         { setLogger(name()); }

     // void reinitialize(Application & app); // not needed. By default: uninit, then init.

     /**
      *
      *
      *
      */
     void uninitialize()
         { poco_information(logger(), "ThreadManager::uninitialize()"); }
     ///@}

    void onStarted(TaskStartedNotification* pNf);
//    void onProgress(TaskProgressNotification* pNf);
    void onFailed(TaskFailedNotification* pNf);
    void onFailedOnCancellation(TaskFailedNotification* pNf);
    void onFinished(TaskFinishedNotification* pNf);
    void onEnslaved(TaskEnslavedNotification* pNf);

    /**
     * Count the active tasks
     *
     * It does include all the pending tasks.
     * @see pendingModTasks
     */
    size_t count();

    /**
     * Cancel all the tasks
     *
     * Cancel module tasks whenever possible
     */
    void cancelAll();

    /**
     * Start a runnable in a thread of the thread pool
     */
    void startDataLogger(DataLogger* dataLogger);

    /**
     * Start a task
     *
     * Start a ModuleTask.
     * Do not take ownership of the task.
     */
    void startModuleTask(ModuleTask* task);

    /**
     * Start a task in the current thread
     *
     * ModuleTask specific.
     * Do not take ownership of the task.
     */
    void startSyncModuleTask(ModuleTask* task);

    /**
     * Wait for all tasks to be terminated
     */
    void waitAll();

    /**
     * Register a new task
     *
     * and take ownership of it
     */
    void registerNewModuleTask(ModuleTask* pTask);

    /**
     * Unregister a task
     */
    void unregisterModuleTask(ModuleTask* pTask);

    /**
     * @see Dispatcher::cancel
     */
    void startModuleCancellationListener(Poco::Runnable& runnable);

private:
    TaskManager taskManager;
    Poco::ThreadPool threadPool;

    /// Store all non-terminated tasks: idle, or active.
    std::set< Poco::AutoPtr<ModuleTask> > pendingModTasks;
    Poco::RWLock  taskListLock;

    bool cancellingAll;
};

#endif /* SRC_THREADMANAGER_H_ */
