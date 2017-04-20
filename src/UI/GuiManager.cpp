/**
 * @file    src/UI/GuiManager.cpp
 * @date    Apr. 2014
 * @author  PhRG - opticalp.fr
 */

/*
Copyright (c) 2014 Ph. Renaud-Goud / Opticalp

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

// wxWidgets includes
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "GuiManager.h"

#include "GUI/CustomWxApp.h"
#include "GUI/TopFrame.h"

/// @name Custom config keys
///@{
#define CONF_KEY_GUI_SCRIPT         "python.guiScript"
///@}

using Poco::Util::Application;

GuiManager::GuiManager() :
    VerboseEntity(name()),
    guiRun(false),
	autostartFlag(false)
{

}

GuiManager::~GuiManager()
{
    // uninitialize(); // should be already called by the system.
}

void GuiManager::initialize(Application& app)
{
    setLogger(name());
}

void GuiManager::uninitialize()
{
    poco_information(logger(), "GuiManager::uninitialized");
}

int GuiManager::main(Application& app)
{
	if ( app.config().hasProperty(CONF_KEY_GUI_SCRIPT))
	{
		scriptPath = Poco::Path(app.config().getString(CONF_KEY_GUI_SCRIPT));
	}

    char *empty = const_cast<char*>("instrumentAll");
    int argc = 1;
    guiRun = true;
    wxEntry(argc, &empty);

    return Application::EXIT_OK;
}

GuiProcessingUnit* GuiManager::getGuiProcUnit()
{
    if (guiRun)
        return wxGetApp().getTopFrame()->getGuiProcUnit();
    else
        return NULL;
}

using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::OptionCallback;

void GuiManager::defineOptions(OptionSet & options)
{
    options.addOption(
        Option(
                "guiscript", "g",
#ifdef GUI_SCRIPT_LOOP
				"script called (in loop) at the Graphical User Interface"
#else
				"script called at the Graphical User Interface"
#endif
				" on start button. " )
            .required(false)
            .repeatable(false)
			.argument("SCRIPT")
            .binding(CONF_KEY_GUI_SCRIPT)
            .group("interface"));

    options.addOption(
        Option(
                "autostart", "a",
                "directly start the gui script at launching" )
            .required(false)
            .repeatable(false)
            .callback(OptionCallback<GuiManager>(
                                this, &GuiManager::handleAutostart) ));
}

void GuiManager::handleAutostart(const std::string& name,
		const std::string& value)
{
	// check if a gui script is defined
	if ( ! Poco::Util::Application::instance().config().hasProperty(CONF_KEY_GUI_SCRIPT))
		poco_error(logger(), "The autostart option can not be set if the gui script is not defined first");
	else
	{
		autostartFlag = true;
		poco_information(logger(), "autostart flag set.");
	}
}

#endif /* HAVE_WXWIDGETS */
