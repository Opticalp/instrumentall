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
#include "core/ThreadManager.h"

#include "PythonScriptRunner.h"

#define WAIT_FOR_UNLOCK_TIMEOUT_MS 500

PythonScriptRunner::PythonScriptRunner(ErrorReporter* parentErrorReporter):
    errReporter(parentErrorReporter),
	loop(false), cancelRequested(false),
	running(false)
{

}

void PythonScriptRunner::run()
{
    running = true;
    Poco::Mutex::ScopedLock lock(mMutex);

    try
    {
        if (!loop)
        {
            Poco::Util::Application::instance()
                .getSubsystem<PythonManager>()
                .runScript(scriptPath);
        }
        else
        {
            while (!cancelRequested)
            {
                Poco::Util::Application::instance()
                    .getSubsystem<PythonManager>()
                    .runScript(scriptPath);
                Poco::Util::Application::instance()
                    .getSubsystem<ThreadManager>().waitAll();
            }
        }
    }
    catch (Poco::Exception& e)
    {
        if (cancelRequested)
            errReporter->reportError("PythonScript "
                + scriptPath.toString() + " error: "
                + e.displayText());
    }
    catch (...) // anyhow: running in a thread, the info would not be forwarded
    {
        errReporter->reportError("PythonScript "
            + scriptPath.toString() + " error: **UNKNOWN**");
    }

    running = false;
}

bool PythonScriptRunner::setScriptAndLock(Poco::Path newScriptPath, bool repeat)
{
    if (!running && mMutex.tryLock())
    {
        scriptPath = newScriptPath;
        loop = repeat;
        cancelRequested = false;
        return true;
    }
    else
    {
        return false;
    }
}

#endif /* HAVE_WXWIDGETS */
