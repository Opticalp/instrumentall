/**
 * @file	src/UI/GUI/DepWxWidgets.cpp
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

std::string DepWxWidgets::runtimeVer = "unknown";

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

#include "wx/version.h"
#include "wx/string.h"

std::string DepWxWidgets::buildTimeVersion()
{
    return std::string(wxString(wxVERSION_STRING).utf8_str());
}

std::string DepWxWidgets::runTimeVersion()
{
    return runtimeVer;
}

#endif /* HAVE_WXWIDGETS */
