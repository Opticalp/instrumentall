/**
 * @file	src/UI/GUI/PythonScriptRunner.h
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

#ifndef SRC_GUI_PYTHONSCRIPTRUNNER_H_
#define SRC_GUI_PYTHONSCRIPTRUNNER_H_

#ifdef HAVE_WXWIDGETS

#include "Poco/Path.h"
#include "Poco/Runnable.h"
#include "Poco/Mutex.h"

#include "core/ErrorReporter.h"

/**
 * PythonScriptRunner
 *
 * Runnable class to launch a python script from an object
 * having an error reporting facility
 *
 * The calling sequence to launch a script should be:
 *
 *
 *     PythonScriptRunner pyScriptRunner(&errReporter);
 *
 *     if (pyScriptRunner.setScriptAndLock(myScript, false))
 *     {
 *         try
 *         {
 *              myThread.start(pyScriptRunner);
 *         }
 *         catch (...)
 *         {
 *              pyScriptRunner.releaseStartLock();
 *              throw;
 *         }
 *         pyScriptRunner.releaseStartLock();
 *     }
 *
 *
 *
 */
class PythonScriptRunner: public Poco::Runnable
{
public:
    PythonScriptRunner(ErrorReporter* parentErrorReporter);
    virtual ~PythonScriptRunner() { }

    /// main function
    virtual void run();

    /**
     * Set the script to be launched
     *
     * lock the mutex (assume that "run" will be called next. )
     * @return false if the mutex can not be locked (another script is being launched)
     */
    bool setScriptAndLock(Poco::Path newScriptPath, bool repeat = false);

    std::string getScript() { return scriptPath.toString(); }
    bool getRepeat() { return loop; }

    void releaseStartLock() { mMutex.unlock(); }

    void cancel() { cancelRequested = true; }

private:
    /// run in loop
    void runLoop();

    /// check if a script is running
    Poco::Mutex mMutex;

    /// local storage of the script path
    Poco::Path scriptPath;

    bool loop; ///< repeat flag
    bool cancelRequested; ///< cancellation requested
    bool running; ///< duplicate thread.isRunning() info at the runnable level

    ErrorReporter* errReporter; ///< parent top frame
};

#endif /* HAVE_WXWIDGETS */
#endif /* SRC_GUI_PYTHONSCRIPTRUNNER_H_ */
