/**
 * @file    src/modules/UI/GuiProperties.cpp
 * @date    Feb. 2017
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2017 Ph. Renaud-Goud / Opticalp

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

#include "GuiProperties.h"

#include "UI/GuiManager.h"
#include "UI/GUI/GuiProcessingUnit.h"
#include "Poco/Util/Application.h"

GuiProperties::GuiProperties(ModuleFactory* parent, std::string customName):
    Module(parent, customName)
{
    setInternalName("GuiProperties");
    setCustomName(customName);
    setLogger("module." + name());

    // parameters
    setParameterCount(paramCnt);
    addParameter(paramStatusBar0, "statusBar0", "statusBar, field #0", ParamItem::typeString);
//    addParameter(paramStatusBar1, "statusBar1", "statusBar, field #1", ParamItem::typeString);
    addParameter(paramTextCtrl, "textCtrl", "main text control", ParamItem::typeString);

    // init wxWidgets share
    guiProc = Poco::Util::Application::instance().getSubsystem<GuiManager>().getGuiProcUnit();

    notifyCreation();
}

std::string GuiProperties::getStrParameterValue(size_t paramIndex)
{
    if (guiProc == NULL)
    {
        poco_warning(logger(), "trying to get a GUI value, "
                "but the GUI is not loaded");
        return "GUI not loaded";
    }

    switch (paramIndex)
    {
    case paramStatusBar0:
        return guiProc->getStatusBarTxt(0);
    //case paramStatusBar1:
    //    return guiProc->getStatusBarTxt(1);
    case paramTextCtrl:
        return guiProc->getTextCtrlTxt();
    default:
        poco_bugcheck_msg("impossible parameter index");
        throw Poco::BugcheckException();
    }
}

void GuiProperties::setStrParameterValue(size_t paramIndex, std::string value)
{
    if (guiProc == NULL)
    {
        poco_warning(logger(), "trying to set a GUI value, "
                "but the GUI is not loaded");
        return;
    }

    switch (paramIndex)
    {
    case paramStatusBar0:
        guiProc->setStatusBarTxt(value, 0);
        break;
    //case paramStatusBar1:
    //    guiProc->setStatusBarTxt(value, 1);
    //    break;
    case paramTextCtrl:
        guiProc->setTextCtrlTxt(value);
        break;
    default:
        poco_bugcheck_msg("impossible parameter index");
        throw Poco::BugcheckException();
    }
}

#endif /* HAVE_WXWIDGETS */
