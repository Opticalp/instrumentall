/**
 * @file	src/UI/GUI/LoginDialog.cpp
 * @date	Nov. 2017
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

#include "LoginDialog.h"
#ifdef HAVE_WXWIDGETS
#ifdef MANAGE_USERS

#include "GuiProcessingUnit.h"

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

wxBEGIN_EVENT_TABLE(LoginDialog, wxDialog)
  EVT_BUTTON(wxID_OK, LoginDialog::onOk)
  EVT_BUTTON(wxID_CANCEL, LoginDialog::onCancel)

wxEND_EVENT_TABLE()

LoginDialog::LoginDialog(wxWindow* parent, GuiProcessingUnit& guiProc):
	wxDialog(parent, -1, "User login"),
	core(guiProc)
{
    wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer *hbox1 = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* lblTxt = new wxStaticText(this, wxID_ANY, wxT("Username: "), wxDefaultPosition);
    wxSize lblSize = lblTxt->GetBestSize();
    lblSize.x += 10;
    lblTxt->SetInitialSize(lblSize);
    hbox1->Add(lblTxt, 0);

    usernameTxtCtrl = new wxTextCtrl(this, wxID_ANY);
    hbox1->Add(usernameTxtCtrl, 1);
    vbox->Add(hbox1, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);

    wxBoxSizer *hbox2 = new wxBoxSizer(wxHORIZONTAL);
    hbox2->Add(new wxStaticText(this, wxID_ANY, wxT("Password: "), wxDefaultPosition, lblSize), 0);

    passwordTxtCtrl = new wxTextCtrl(this, wxID_ANY, wxString(""),
        wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
    hbox2->Add(passwordTxtCtrl, 1);
    vbox->Add(hbox2, 1, wxEXPAND | wxLEFT | wxTOP | wxRIGHT, 10);

    wxSizer *hSizer3 = CreateButtonSizer(wxOK | wxCANCEL);
    if (hSizer3)
    	vbox->Add(hSizer3, 0, wxEXPAND | wxLEFT | wxTOP | wxRIGHT, 10);

    SetSizerAndFit(vbox);
}

void LoginDialog::onOk(wxCommandEvent& event)
{
	if (core.tryLogin(usernameTxtCtrl->GetValue().ToStdString(),
			passwordTxtCtrl->GetValue().ToStdString()))
		event.Skip();
	else
		wxMessageBox(wxT("Login failed, please retry. "),
				wxT("User login"), wxICON_ERROR);
}

void LoginDialog::onCancel(wxCommandEvent& event)
{
	wxMessageBox("cancel pressed");
	event.Skip();
}

#endif /* MANAGE_USERS */
#endif /* HAVE_WXWIDGETS */
