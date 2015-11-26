/**
 * @file	src/PythonRedirection.h
 * @date	nov. 2015
 * @author	PhRG - opticalp.fr
 * 
 * Python standard error redirection tools
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

#ifndef SRC_PYTHONREDIRECTION_H_
#define SRC_PYTHONREDIRECTION_H_

#include "Python.h"

typedef void (*forwardingFctPtr) (const char *);

/// Python standard error forwarder
void setPyStderrForwarder(forwardingFctPtr forwarder);

/// Python stderr.write overloading candidate
extern "C" PyObject * stderr_write(PyObject *self, PyObject *args);

/// structure used to expose the stderr_write method
static PyMethodDef stderr_methods[] = {
    {"write", stderr_write, METH_VARARGS, "Write some error."},
    {NULL, NULL, 0, NULL}
};


#endif /* SRC_PYTHONREDIRECTION_H_ */
