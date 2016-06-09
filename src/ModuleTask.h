/**
 * @file	src/ModuleTask.h
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

#ifndef SRC_MODULETASK_H_
#define SRC_MODULETASK_H_

#include "MergeableTask.h"

#include "Poco/Event.h"

class Module;
class InPort;

/**
 * Class inherited from MergeableTask to control the execution of a Module.
 *
 * The runTask method shall only be called by Module::popTask or Module::popTaskSync
 */
class ModuleTask: public MergeableTask
{
public:
	ModuleTask(Module* module, InPort* inPort = NULL);
	virtual ~ModuleTask();

	/// Unique name of the task. Defined at creation.
	std::string name() { return mName; }

	/**
	 * Execute the attached module.
	 *
	 * To be called by the module itself.
	 * @see Module::popTask
	 * @see Module::popTaskSync
	 */
	void runTask();

	/**
	 * Retrieve the attached module
	 *
	 * The returned value should be transformed into a sharedPtr via
	 * ModuleManager::getModule() to be used.
	 */
	Module* module() { return coreModule; }

	/**
	 * Retrieve the triggering input port
	 *
	 * The returned value should be transformed into a sharedPtr via
	 * Dispatcher::getInPort() to be used.
	 */
	InPort* triggingPort() { return mTriggingPort; }

	/**
	 * Possible states during execution
	 */
	enum RunningStates
	{
		retrievingInDataLocks,
		retrievingOutDataLocks,
		processing
	};

	RunningStates getRunningState() { return runState; }

	/**
	 * Wait the given time that the task finishes
	 *
	 * @return false if the task did not finish in the given time
	 */
	bool tryWaitTaskDone(long milliseconds)
		{ return doneEvent.tryWait(milliseconds); }

	/**
	 * Wait that the task finishes
	 *
	 * Infinite wait.
	 */
	void waitTaskDone()
		{ doneEvent.wait(); }

protected:
	/**
	 * To be called by Module::run and Module::process
	 * to give information about the current running state of the task
	 */
	void setRunningState(RunningStates state) { runState = state; }

private:
	ModuleTask();
	Module* coreModule;
	InPort* mTriggingPort;

	std::string mName;

	RunningStates runState;

	Poco::Event doneEvent; /// event signaled when the task is finished.

	friend class Module; // access to sleep, setProgress, ...
};

#endif /* SRC_MODULETASK_H_ */