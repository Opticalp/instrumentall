/**
 * @file	src/DepWxWidgets.cpp
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

#include "DepWxWidgets.h"

// wxWidgets includes
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

//#include "wx/version.h"

DepWxWidgets::DepWxWidgets()
{
    if ( wxInitialize())
    {
        runtimeVer = wxGetLibraryVersionInfo().ToString().ToStdString();
        wxUninitialize();
    }
    else
    {
        runtimeVer = "Unknown";
    }

}

std::string DepWxWidgets::name()
{
    return "wxWidgets";
}

std::string DepWxWidgets::description()
{
    return "Cross-platform GUI Library";
}

std::string DepWxWidgets::URL()
{
    return "http://www.wxwidgets.org/";
}

std::string DepWxWidgets::license()
{
    return "Copyright (c) 1998-2005 Julian Smart, Robert Roebling et al \n"
           "wxWidgets is currently licenced under the \"wxWindows Library Licence\" "
           "pending approval of the \"wxWidgets Library Licence\" which will be identical "
           "apart from the name.";
}

std::string DepWxWidgets::buildTimeVersion()
{
    return (wxString(wxVERSION_STRING).ToStdString());
}

std::string DepWxWidgets::runTimeVersion()
{
    //return
    return runtimeVer;
}

#endif /* HAVE_WXWIDGETS */
