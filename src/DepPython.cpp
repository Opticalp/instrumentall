/**
 * @file	DepPython.cpp
 * @date	20 nov. 2015
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2015 Ph. Renaud-Goud / Opticalp

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

#ifdef HAVE_PYTHON27

#include "PythonAPI.h"
// #include "patchlevel.h"

#include "DepPython.h"

std::string DepPython::name()
{
    return "Python";
}

std::string DepPython::description()
{
    return "Python is a programming language that lets you work quickly "
            "and integrate systems more effectively.";
}

std::string DepPython::URL()
{
    return "http://www.python.org/";
}

std::string DepPython::license()
{
    return std::string(Py_GetCopyright());
}

std::string DepPython::buildTimeVersion()
{
    return PY_VERSION;
}

std::string DepPython::runTimeVersion()
{
    return std::string(Py_GetVersion());
}

#endif /* HAVE_PYTHON27 */
