/**
 * @file	src/UI/GUI/CustomWxApp.cpp
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

#include "CustomWxApp.h"

#include "TopFrame.h"

wxIMPLEMENT_APP_NO_MAIN(CustomWxApp);

bool CustomWxApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    DepWxWidgets::runtimeVer = wxGetLibraryVersionInfo().ToString().utf8_str();

    wxInitAllImageHandlers();

    wxXmlResource::Get()->InitAllHandlers();

    // load all xrc files from directory conf/rc
    Poco::Path rcDir(
            Poco::Util::Application::instance()
                .config()
                .getString("system.currentDir")  );

    // with poco >= 1.4.6, the 2 following lines can be merged
    rcDir.pushDirectory("conf");
    rcDir.pushDirectory("rc");

    wxXmlResource::Get()->LoadAllFiles(rcDir.toString());

    try
    {
        frame = new TopFrame();
        frame->Show(true);
    }
    catch (Poco::Exception& e)
    {
        wxMessageBox( e.displayText(),
                "TopFrame",
                wxOK | wxICON_ERROR );
        e.rethrow();
    }

    return true;
}
#endif /* HAVE_WXWIDGETS */
