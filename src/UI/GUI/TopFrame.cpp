/**
 * @file	src/UI/GUI/TopFrame.cpp
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

// wxWidgets includes
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/xrc/xmlres.h"      // XRC XML resources

//#include "wx/textctrl.h"
//#include "wx/colour.h"
//#include "wx/time.h" // wxMilliSleep
//#include "wx/tglbtn.h"
//#include "wx/spinctrl.h"
//#include "wx/valtext.h" // validator
#include "wx/statusbr.h"

#include "Poco/Exception.h"
#include "Poco/NumberFormatter.h"
#include "Poco/NumberParser.h"

#include "TopFrame.h"

wxDECLARE_EVENT(RefreshEvent, wxCommandEvent);
wxDEFINE_EVENT(RefreshEvent, wxCommandEvent);

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------

// The event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
// The reason why the menuitems and tools are given the same name in the
// XRC file, is that both a tool (a toolbar item) and a menuitem are designed
// to fire the same kind of event (an EVT_MENU) and thus I give them the same
// ID name to help new users emphasize this point which is often overlooked
// when starting out with wxWidgets.

wxBEGIN_EVENT_TABLE(TopFrame, wxFrame)
    EVT_MENU(wxID_EXIT, TopFrame::onExit)
    EVT_MENU(XRCID("runScriptMenu"), TopFrame::onRunScriptMenu)
    EVT_MENU(XRCID("guiScriptMenu"), TopFrame::onGuiScriptMenu)
    EVT_MENU(XRCID("wxAbout"), TopFrame::onWxAbout)

    EVT_BUTTON(XRCID("startBtn"), TopFrame::onStart)
    EVT_BUTTON(wxID_STOP, TopFrame::onStop)
    EVT_BUTTON(wxID_EXIT, TopFrame::onExit)

//    EVT_BUTTON(wxID_ZOOM_IN, TopFrame::onZoomIn)
//    EVT_BUTTON(wxID_ZOOM_OUT, TopFrame::onZoomOut)
//    EVT_BUTTON(wxID_ZOOM_FIT, TopFrame::onZoomHome)

    EVT_COMMAND(wxID_ANY, RefreshEvent, TopFrame::forceRefresh)
wxEND_EVENT_TABLE()


TopFrame::TopFrame(wxWindow* parent):
    core(this)
{
    // Load up this frame from XRC. [Note, instead of making a class's
    // constructor take a wxWindow* parent with a default value of NULL,
    // we could have just had designed MyFrame class with an empty
    // constructor and then written here:
    // wxXmlResource::Get()->LoadFrame(this, (wxWindow* )NULL, "main_frame");
    // since this frame will always be the top window, and thus parentless.
    // However, the current approach has source code that can be recycled
    // for other frames that aren't the top level window.]
    wxXmlResource::Get()->LoadFrame(this, parent, wxT("topFrame"));

    // go maximized
//    Maximize(true);

//    _imagePanel = XRCCTRL(*this, "imagePanel", ImagePanel);

//    // link to the interfacing processor
//    GuiProperties::initWxFrame(this);

    stBar = XRCCTRL(*this,"topFrameStatusbar", wxStatusBar);

//    stBar->SetStatusText("my name is joe");
//    stBar->SetStatusText("my name is jack",1);
}

TopFrame::~TopFrame()
{
}

void TopFrame::onWxAbout(wxCommandEvent& event)
{
    wxInfoMessageBox(this);
}

void TopFrame::onExit(wxCommandEvent& event)
{
    core.stop();

    Close(true);
}

void TopFrame::onStart(wxCommandEvent& event)
{
    if (isRunning())
    {
        wxMessageBox(
                "A script is already running...",
                "Start GUI script",
                wxOK | wxICON_ERROR );
        return;
    }

    try
    {
#ifdef GUI_SCRIPT_LOOP
    	core.runLoopPyScript();
#else
    	core.runPyScript();
#endif

//        XRCCTRL(*this,"pauseButton",wxToggleButton)->SetValue(false);
    }
    catch (Poco::Exception& e)
    {
    	wxLogError("Not able to start the GUI script");
	}
}

void TopFrame::onStop(wxCommandEvent& event)
{
    core.stop();

    //XRCCTRL(*this,"pauseButton",wxToggleButton)->SetValue(false);
}


void TopFrame::onRunScriptMenu(wxCommandEvent& event)
{
    if (isRunning())
    {
        wxMessageBox(
                "A script is already running...",
                "Run script",
                wxOK | wxICON_ERROR );
        return;
    }

    wxFileDialog fileDlg(this,
            "Open python script", "", "",
            "Python scripts (*.py)|*.py",
            wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR);

    if (fileDlg.ShowModal() == wxID_CANCEL)
        return; // the user changed idea...

    try
    {
        core.runPyScript(Poco::Path(fileDlg.GetPath().utf8_str()));
    }
    catch (Poco::Exception& e)
    {
        wxLogError(e.displayText().c_str());
    }
}

void TopFrame::onGuiScriptMenu(wxCommandEvent& event)
{
//    if (isRunning())
//        return;

    wxFileDialog fileDlg(this,
            "Select GUI python script", "", "",
            "Python scripts (*.py)|*.py",
            wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR);

    if (fileDlg.ShowModal() == wxID_CANCEL)
        return; // the user changed idea...

    core.setPyGuiScript(Poco::Path(fileDlg.GetPath().utf8_str()));
}


void TopFrame::forceRefresh(wxCommandEvent& event)
{
    Refresh();
}

void TopFrame::updateDisplay()
{
    // TODO:
    // update values before refreshing
//    XRCCTRL(*this, "sourceFile", wxStaticText)
//        ->SetLabelText(core.getSourceFile().getFileName());

    // do not call refresh directly! since we can be in a worker thread...
    wxCommandEvent* evt = new wxCommandEvent(RefreshEvent,GetId());
    evt->SetEventObject(this);
    QueueEvent( evt );
}

//void TopFrame::onImageDlg(wxCommandEvent& event)
//{
//#ifdef WIN32
//	imageDlg->Show();
//#else
//    imageDlg->Raise();
//#endif
//}

void TopFrame::setStatusBarTxt(std::string msg, int pos)
{
	if (pos<0 || pos>1)
		throw Poco::RangeException("statusBar position");

	stBar->SetStatusText(msg, pos);
}

std::string TopFrame::getStatusBarTxt(int pos)
{
    if (pos<0 || pos>1)
        throw Poco::RangeException("statusBar position");

    return std::string(stBar->GetStatusText(pos));
}

void TopFrame::setTextCtrlTxt(std::string txt)
{
    XRCCTRL(*this,"textCtrl",wxTextCtrl)->SetValue(txt.c_str());
}

std::string TopFrame::getTextCtrlTxt()
{
    return std::string(XRCCTRL(*this,"textCtrl",wxTextCtrl)->GetValue());
}


void TopFrame::reportError(std::string errorMsg)
{
    wxLogError(errorMsg.c_str());
}

#endif /* HAVE_WXWIDGETS */
