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
	doneEvent(false) // manual reset
{
	// commented: registered when queued by the dispatcher
	// coreModule->registerTask(this);

	mName = coreModule->name() + "-t" + Poco::NumberFormatter::format(id());
}

ModuleTask::ModuleTask():
	coreModule(NULL),
	runState(NotAvailableRunningState),
	mTriggingPort(NULL),
	doneEvent(false) // manual reset
{
}

ModuleTask::~ModuleTask()
{
	poco_information(coreModule->logger(), "erasing task: " + name());
	coreModule->unregisterTask(this);

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
	if (isCancelled())
		throw Poco::RuntimeException(name() +
				": can not change running state, "
				"the task is cancelling");

	runState = state;
}

void ModuleTask::runTask()
{
	if (coreModule == NULL)
		throw Poco::NullPointerException("no more module bound to " + name());

	try
	{
		coreModule->run(this);
	}
	catch (...)
	{
		doneEvent.set();
		throw;
	}

	doneEvent.set();
}

void ModuleTask::leaveTask()
{			
	if (coreModule == NULL)
		return;

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
	if (coreModule)
		coreModule->condCancel();

	if (!isCancelled())
		MergeableTask::cancel();
}

void ModuleTask::resetModule()
{
	if (coreModule)
		coreModule->resetWithTargets();
}

