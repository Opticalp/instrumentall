/**
 * @file	src/ModuleTask.cpp
 * @date	31 mai 2016
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

#include "ModuleTask.h"

#include "Module.h"
#include "InPort.h"

#include "Poco/NumberFormatter.h"

ModuleTask::ModuleTask(Module* module, InPort* inPort):
	coreModule(module),
	runState(NotAvailableRunningState),
	mTriggingPort(inPort),
	doneEvent(false), // manual reset
	unregisterRunner(this)
{
	// commented: registered when queued by the dispatcher
	// coreModule->registerTask(this);

	mName = coreModule->name() + "-t" + Poco::NumberFormatter::format(id());
}

ModuleTask::ModuleTask():
	coreModule(NULL),
	runState(NotAvailableRunningState),
	mTriggingPort(NULL),
	doneEvent(false), // manual reset
	unregisterRunner(this)
{
}

ModuleTask::~ModuleTask()
{
	poco_information(coreModule->logger(), "erasing task: " + name());

	// there should be nothing to do in the managers since AutoPtr should be used
	// - in the thread manager
	// - in the task manager
}

ModuleTask::RunningStates ModuleTask::getRunningState()
{
	if (getState() == TASK_RUNNING)
		return runState;
	else
		return NotAvailableRunningState;
}

void ModuleTask::setRunningState(RunningStates state)
{
	runState = state;
}

void ModuleTask::prepareTask()
{
    coreModule->prepareTaskStart(this);

    try
    {
        MergeableTask::prepareTask();
    }
    catch (...)
    {
        poco_warning(coreModule->logger(), name() + " starting failed (prepareTask)");
        coreModule->taskStartFailure();

        if ((getState() == TASK_FINISHED) && isSlave())
        {
            poco_warning(coreModule->logger(), name() + " was merged and is finished");
            throw TaskMergedException("Finished slave task. "
                    "Can not run");
        }

        throw;
    }
}

void ModuleTask::runTask()
{
	coreModule->run(this);
}

void ModuleTask::leaveTask()
{			
    if (coreModule == NULL)
		return;

    poco_information(coreModule->logger(), name() + " leaving task. "
            "Check the queue. ");

	// enqueue tasks until it works.
	while (true)
	{
		try
		{
			coreModule->popTaskSync();
		}
		catch (...)
		{
			continue;
		}
		break;
	}

}

void ModuleTask::cancel()
{
    switch(getState())
    {
    case TASK_IDLE:
    case TASK_STARTING:
    case TASK_RUNNING:
    case TASK_MERGED:
        break;
    case TASK_FALSE_START:
        poco_warning(coreModule->logger(), name()
                + " cancel not relevant: "
                "task state is FALSE_START. ");
        return;
    case TASK_CANCELLING:
        poco_warning(coreModule->logger(), name()
                + " cancel not relevant: "
                "task state is CANCELLING. ");
        return;
    case TASK_FINISHED:
        poco_warning(coreModule->logger(), name()
                + " cancel not relevant: "
                "task state is FINISHED. ");
        return;
    default:
        poco_error(coreModule->logger(), name()
                + " cancel not possible. "
                "Task state is unknown. ");
        return;
    }

    ModuleTask* pTask = coreModule->getRunningTask();
    coreModule->setRunningTask(this);
	moduleCancel();
	coreModule->setRunningTask(pTask);
	MergeableTask::cancel();
}

#include "Dispatcher.h"
#include "Poco/Util/Application.h"

void ModuleTask::moduleCancel()
{
	Poco::Util::Application::instance()
			.getSubsystem<Dispatcher>().cancel(coreModule);
}

#include "ThreadManager.h"

void ModuleTask::taskFinished()
{
    doneEvent.set();

    poco_information(coreModule->logger(), name() + " finished, "
            "trying to remove it from Module::allLaunchedTasks");

	if (!coreModule->tryUnregisterTask(this))
	{
		// try once, then, do it the async way
        poco_information(coreModule->logger(), name() + ": "
                "removing it (async) from Module::allLaunchedTasks");
		Poco::Util::Application::instance()
				.getSubsystem<ThreadManager>()
				.startRunnable(unregisterRunner);
	}
}

