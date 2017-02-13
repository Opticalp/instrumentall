/**
 * @file	src/UI/GUI/CustomWxApp.h
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


#ifndef SRC_CUSTOMWXAPP_H_
#define SRC_CUSTOMWXAPP_H_

#ifdef HAVE_WXWIDGETS

// wxWidgets includes
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#  include "wx/wx.h"
#endif
#include "wx/xrc/xmlres.h"

class TopFrame;

#include "DepWxWidgets.h"

#include "Poco/Path.h"
#include "Poco/Util/Application.h"

class CustomWxApp: public wxApp
{
public:
    CustomWxApp():frame(NULL) { }
    ~CustomWxApp() { /* delete frame; // no: already deleted by wx */}

    virtual bool OnInit();

    TopFrame* getTopFrame() { return frame; }
private:
    TopFrame *frame;
};

wxDECLARE_APP(CustomWxApp);

#endif /* HAVE_WXWIDGETS */
#endif /* SRC_CUSTOMWXAPP_H_ */
