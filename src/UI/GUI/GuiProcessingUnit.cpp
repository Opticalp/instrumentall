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
    pyRunner(parent)
{
    setLogger("GuiProcessingUnit");

    guiScript = Poco::Util::Application::instance()
                    .getSubsystem<GuiManager>().getScript();
}

GuiProcessingUnit::~GuiProcessingUnit()
{

}

void GuiProcessingUnit::runPyScript(Poco::Path scriptPath, bool repeat)
{
    if (isScriptRunning())
    {
        topFrame->reportError("a script is already running: " + pyRunner.getScript());
        return;
    }

    if (pyRunner.setScriptAndLock(scriptPath, repeat))
    {
        try
        {
            pyThread.start(pyRunner);
        }
        catch (...)
        {
            pyRunner.releaseStartLock();
            throw;
        }

        pyRunner.releaseStartLock();
    }
}

void GuiProcessingUnit::runPyScript()
{
	runPyScript(guiScript);
}

void GuiProcessingUnit::runLoopPyScript()
{
    runPyScript(guiScript, true);
}

void GuiProcessingUnit::stop()
{
    if (isScriptRunning())
    	pyRunner.cancel();

    Poco::Util::Application::instance().getSubsystem<ThreadManager>().cancelAll();
}

bool GuiProcessingUnit::isScriptRunning()
{
	return pyThread.isRunning();
}

std::string GuiProcessingUnit::getStatusBarTxt(int field)
{
    return topFrame->getStatusBarTxt(field);
}

void GuiProcessingUnit::setStatusBarTxt(std::string txt, int field)
{
    topFrame->setStatusBarTxt(txt, field);
    topFrame->updateDisplay();
}

std::string GuiProcessingUnit::getTextCtrlTxt()
{
    return topFrame->getTextCtrlTxt();
}

void GuiProcessingUnit::init()
{
	if (Poco::Util::Application::instance()
	                    .getSubsystem<GuiManager>().autostart())
	{
#ifdef GUI_SCRIPT_LOOP
    	runLoopPyScript();
#else
    	runPyScript();
#endif
	}
}

void GuiProcessingUnit::setTextCtrlTxt(std::string txt)
{
    topFrame->setTextCtrlTxt(txt);
    topFrame->updateDisplay();
}

#ifdef HAVE_OPENCV
void GuiProcessingUnit::showImage(cv::Mat img, int panelIndex)
{
    topFrame->setImage(img, panelIndex);
}

#endif

#ifdef MANAGE_USERS

#include "core/UserManager.h"

void GuiProcessingUnit::forwardGuiUserToPython()
{
	Poco::Util::Application::instance()
			.getSubsystem<PythonManager>().setUser(
					Poco::Util::Application::instance()
						.getSubsystem<GuiManager>().getUser());
}

bool GuiProcessingUnit::tryLogin(std::string userName, std::string passwd)
{
	if ( Poco::Util::Application::instance()
		                    .getSubsystem<UserManager>()
							.authenticate(userName, passwd,
									Poco::Util::Application::instance()
	                    				.getSubsystem<GuiManager>()
										.getUser()) )
	{
		forwardGuiUserToPython();
		return true;
	}
	else
		return false;
}

void GuiProcessingUnit::logout()
{
	Poco::Util::Application::instance()
			                    .getSubsystem<UserManager>()
								.disconnectUser(
										Poco::Util::Application::instance()
		                    				.getSubsystem<GuiManager>()
											.getUser());
	forwardGuiUserToPython();
}

#endif /* MANAGE_USERS */

#endif /* HAVE_WXWIDGETS */
