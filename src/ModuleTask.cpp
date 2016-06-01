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
	coreModule(module), triggingPort(inPort)
{
	// commented: registered when queued by the dispatcher
	// coreModule->registerTask(this);

	mName = coreModule->name() + "-t" + Poco::NumberFormatter::format(id());
}

ModuleTask::ModuleTask():
	coreModule(NULL), triggingPort(NULL)
{
}

ModuleTask::~ModuleTask()
{
	coreModule->unregisterTask(this);

	// there should be nothing to do in the managers since AutoPtr should be used
	// - in the thread manager
	// - in the task manager
}

void ModuleTask::runTask()
{
	if (coreModule == NULL)
		throw Poco::NullPointerException("no more module bound to " + name());

	coreModule->setRunningTask(this);
	coreModule->run();
}
