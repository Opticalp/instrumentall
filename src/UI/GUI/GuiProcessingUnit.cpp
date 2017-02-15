/**
 * @file	src/UI/GUI/GuiProcessingUnit.cpp
 * @date	Apr. 2016
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


#ifdef HAVE_WXWIDGETS

#include "GuiProcessingUnit.h"

#include "UI/PythonManager.h"
#include "UI/GuiManager.h"
//#include "core/Dispatcher.h"
#include "core/ThreadManager.h"
//#include "core/ModuleManager.h"
//#include "core/Module.h"

#include "Poco/Util/Application.h"
#include "Poco/NumberFormatter.h"
#include "Poco/Format.h"

//#include "ImagePanel.h"
#include "TopFrame.h"

GuiProcessingUnit::GuiProcessingUnit(TopFrame* parent):
    topFrame(parent), // imagePanel(NULL),
    pyRunner(parent),
	stopRequest(false)
{
    setLogger("GuiProcessingUnit");

    guiScript = Poco::Util::Application::instance()
                    .getSubsystem<GuiManager>().getScript();
}

GuiProcessingUnit::~GuiProcessingUnit()
{

}

void GuiProcessingUnit::runPyScript(Poco::Path scriptPath)
{
    if (isRunning())
    {
        topFrame->reportError("a script is already running: " + pyRunner.getScript());
        return;
    }


    // TODO: lock?
    stopRequest = false;
    pyRunner.setRepeat(false);
    pyRunner.setScript(scriptPath);
    pyThread.start(pyRunner);
    // waitAll()

    //    if (breakOnError)
    //    	poco_warning(logger(),
    //    			"The fab thread ended on error: " + errMsg);
    //    else if (stopRequest)
    //    	poco_warning(logger(),
    //    			"processFab ended on stop request");
    //    else
    //    	poco_information(logger(),
    //    			"The fab thread ended successfully. ");

    //    runningScript = false;
        stopRequest = false;
    //    topFrame->stBarText("", 0);
    //    topFrame->updateDisplay();
}

void GuiProcessingUnit::runPyScript()
{
	runPyScript(guiScript);
}

void GuiProcessingUnit::runLoopPyScript()
{
    if (isRunning())
    {
        topFrame->reportError("a script is already running: " + pyRunner.getScript());
        return;
    }

    // TODO: lock?
    stopRequest = false;
    pyRunner.setRepeat(true);
    pyRunner.setScript(guiScript);
    pyThread.start(pyRunner);

//    if (breakOnError)
//    	poco_warning(logger(),
//    			"The fab thread ended on error: " + errMsg);
//    else if (stopRequest)
//    	poco_warning(logger(),
//    			"processFab ended on stop request");
//    else
//    	poco_information(logger(),
//    			"The fab thread ended successfully. ");

//    runningScript = false;
    stopRequest = false;
//    topFrame->stBarText("", 0);
//    topFrame->updateDisplay();
}

void GuiProcessingUnit::stop()
{
    if (isRunning())
    	stopRequest = true;

    Poco::Util::Application::instance().getSubsystem<ThreadManager>().cancelAll();
}

bool GuiProcessingUnit::isRunning()
{
	// TODO: fix it using threads etc. see runPyScript and runLoopPyScript
	return pyThread.isRunning();
}

//#ifdef HAVE_OPENCV
//void GuiProcessingUnit::showImage(cv::Mat img)
//{
//    imagePanel->setImage(img);
//}
//
//#endif
//
//void GuiProcessingUnit::newImage()
//{
//    try
//    {
//        (*findAfCam())->runModule();
//    }
//    catch (Poco::Exception& e)
//    {
//        poco_error(logger(), e.displayText());
//    }
//}

#endif /* HAVE_WXWIDGETS */
