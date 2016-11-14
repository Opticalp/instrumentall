/**
 * @file	src/UI/python/PythonAPI.h
 * @date	dec. 2015
 * @author	PhRG - opticalp.fr
 * 
 * Fix a #pragma including python27_d.lib in Python.h (in pyconfig.h exactly)
 * when in debug mode.
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


#ifndef SRC_PYTHONAPI_H_
#define SRC_PYTHONAPI_H_

#ifdef HAVE_PYTHON27


// inspired from boost python wrapper.
#ifdef _DEBUG
#  ifdef _MSC_VER
    // VC8.0 will complain if system headers are #included both with
    // and without _DEBUG defined, so we have to #include all the
    // system headers used by pyconfig.h right here.
#   include <stddef.h>
#   include <stdarg.h>
#   include <stdio.h>
#   include <stdlib.h>
#   include <assert.h>
#   include <errno.h>
#   include <ctype.h>
#   include <wchar.h>
#   include <basetsd.h>
#   include <io.h>
#   include <limits.h>
#   include <float.h>
#   include <string.h>
#   include <math.h>
#   include <time.h>
#  endif
#  undef _DEBUG // Don't let Python force the debug library just because we're debugging.
#  define DEBUG_UNDEF_BY_PYTHONAPI_H
#endif

#include "Python.h"

#ifdef DEBUG_UNDEF_BY_PYTHONAPI_H
#  define _DEBUG
#endif


#endif /* HAVE_PYTHON27 */
#endif /* SRC_PYTHONAPI_H_ */
