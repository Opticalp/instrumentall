/**
 * @file	src/UI/Python/PythonScriptRunner.cpp
 * @date	Mar. 2015
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2015 Ph. Renaud-Goud / Opticalp

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

#ifdef HAVE_WXWIDGETS

#include "Poco/Util/Application.h"
#include "UI/PythonManager.h"

#include "PythonScriptRunner.h"

#define WAIT_FOR_UNLOCK_TIMEOUT_MS 5000

PythonScriptRunner::PythonScriptRunner(ErrorReporter* parentErrorReporter):
    errReporter(parentErrorReporter),
	loop(false), cancelRequested(false)
{

}

void PythonScriptRunner::run()
{
	_mutex.lock(WAIT_FOR_UNLOCK_TIMEOUT_MS);

	try
	{
		if (!loop)
		{
		    try
		    {
                Poco::Util::Application::instance()
                    .getSubsystem<PythonManager>()
                    .runScript(scriptPath);
		    }
		    catch (Poco::Exception& e)
		    {
		        errReporter->reportError("PythonScript "
		                + scriptPath.toString() + " error: "
		                + e.displayText());
		    }
		}
		else
		{
			cancelRequested = false;
			runLoop();
		}
	}
	catch (Poco::Exception&)
	{
		_mutex.unlock();
		throw;
	}

	_mutex.unlock();
}

bool PythonScriptRunner::setScript(Poco::Path newScriptPath)
{
    // the lock isn't required since we check if the thread is running
    // before doing any action...
    if (_mutex.tryLock(WAIT_FOR_UNLOCK_TIMEOUT_MS))
    {
        scriptPath = newScriptPath;
        _mutex.unlock();
        return true;
    }
    else
    {
        return false;
    }
}

void PythonScriptRunner::runLoop()
{
	while (true)
	{
		if (!cancelRequested)
		{
            try
            {
                Poco::Util::Application::instance()
                    .getSubsystem<PythonManager>()
                    .runScript(scriptPath);

                // TODO:
                //waitAll
            }
            catch (Poco::Exception& e)
            {
                errReporter->reportError("PythonScript "
                        + scriptPath.toString() + " error: "
                        + e.displayText());
                break;
            }
		}
		else
		{
		    break;
		}
	}
}

#endif /* HAVE_WXWIDGETS */
