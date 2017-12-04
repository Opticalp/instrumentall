/**
 * @file	src/UI/GUI/TopFrame.h
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

#ifndef SRC_TOPFRAME_H_
#define SRC_TOPFRAME_H_

#ifdef HAVE_WXWIDGETS

// to be defined if the GUI script has to be called in loop.
//#define GUI_SCRIPT_LOOP

#include "wx/frame.h"

#include "core/ErrorReporter.h"
#include "GuiProcessingUnit.h"

#ifdef HAVE_OPENCV
#include "ImagePanel.h"

#include "opencv2/core/core.hpp"
#endif

/**
 * TopFrame
 *
 * wxFrame derived class defining the app main window using a XRC resource
 * file.
 */
class TopFrame: public wxFrame, public ErrorReporter
{
public:
    TopFrame(wxWindow* parent=(wxWindow *)NULL);
    virtual ~TopFrame() { }

#ifdef HAVE_OPENCV
    /**
     * Display an image on an image panel
     *
     * Do not emit a warning if the requested image panel (see `pos` parameter)
     * is not available.
     *
     * @param img opencv image to display
     * @param index image panel index in case of multiple image panels
     */
    void setImage(cv::Mat img, int pos = 0);
#endif

    /**
     * Refresh the display with new values
     *
     * Send a RefreshEvent
     */
    void updateDisplay();

    GuiProcessingUnit* getGuiProcUnit() { return &core; }

    void setStatusBarTxt(std::string msg, int pos=0);
    std::string getStatusBarTxt(int pos);

    void setTextCtrlTxt(std::string txt);
    std::string getTextCtrlTxt();

    void reportError(std::string errorMsg);
    void reportStatus(std::string statusMsg);

private:
    GuiProcessingUnit core;

    bool isRunning() { return core.isScriptRunning(); }

    wxStatusBar* stBar;

    // Event handlers (these functions should _not_ be virtual)
    /// General exit function
    void onExit(wxCommandEvent& event);
    void onClose(wxCloseEvent& event);

    /// run script menu event handler
    void onRunScriptMenu(wxCommandEvent& event);

    /// gui script menu event handler
    void onGuiScriptMenu(wxCommandEvent& event);

    /// about... menu event handler
    void onWxAbout(wxCommandEvent& event);

    /// Start button event handler
    void onStart(wxCommandEvent& event);
    /// stop button event handler
    void onStop(wxCommandEvent& event);

#ifdef HAVE_OPENCV
    /// zoom in button handler
    void onZoomIn(wxCommandEvent& event);
    /// zoom out button handler
    void onZoomOut(wxCommandEvent& event);
    /// zoom fit button handler
    void onZoomFit(wxCommandEvent& event);

    std::vector<ImagePanel*> imgPanels;
#endif

#ifdef MANAGE_USERS
    void addUserManagementMenuEntry();

    /// open login window
    void onLogin(wxCommandEvent& event);
    /// user logout
    void onLogout(wxCommandEvent& event);
#endif

    /**
     * Listener in the main thread
     *
     * Is able to refresh the GUI.
     * Trigged by a RefreshEvent
     */
    void forceRefresh(wxCommandEvent& event);

    /**
     * Display an error message using a pop-up dialog
     *
     */
    void emitErrorMessage(wxCommandEvent& event);

    // Any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();
};

#endif /* HAVE_WXWIDGETS */
#endif /* SRC_TOPFRAME_H_ */
