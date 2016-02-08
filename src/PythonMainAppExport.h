/**
 * @file	src/PythonMainAppExport.h
 * @date	dec. 2015
 * @author	PhRG - opticalp.fr
 * 
 * @brief Python exposition of methods of MainApplication class
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


#ifndef SRC_PYTHONMAINAPPEXPORT_H_
#define SRC_PYTHONMAINAPPEXPORT_H_

#ifdef HAVE_PYTHON27

#include "PythonAPI.h"

/**
 * @brief Python wrapper to retrieve the "about" information
 *
 * Call MainApplication::about() method
 *
 */
extern "C" PyObject*
pythonMainAppAbout(PyObject *self, PyObject *args);

static PyMethodDef pyMethodMainAppAbout =
{
    "about",
    pythonMainAppAbout,
    METH_NOARGS,
    "Retrieve general verbose information about the current application"
};

/**
 * @brief Python wrapper to retrieve the version information
 *
 * Call MainApplication::version() method
 *
 */
extern "C" PyObject*
pythonMainAppVersion(PyObject *self, PyObject *args);

static PyMethodDef pyMethodMainAppVersion =
{
    "version",
    pythonMainAppVersion,
    METH_NOARGS,
    "Retrieve version information of the current application"
};


// ----------------------------------------------------------------
//     Module Manager
// ----------------------------------------------------------------

/**
 * @brief Python wrapper to retrieve the list of root factories
 *
 * Call ModuleManager::getRootFactories() method
 *
 */
extern "C" PyObject*
pythonModManGetRootFact(PyObject *self, PyObject *args);

static PyMethodDef pyMethodModManGetRootFact =
{
    "getRootFactories",
    pythonModManGetRootFact,
    METH_NOARGS,
    "Retrieve the list of root factories"
};

// ----------------------------------------------------------------
//     Dispatcher
// ----------------------------------------------------------------

/**
 * @brief Python wrapper to bind ports
 *
 * Call Dispatcher::bind(portA, portB) method
 *
 */
extern "C" PyObject*
pythonDispatchBind(PyObject *self, PyObject *args);

static PyMethodDef pyMethodDispatchBind =
{
    "bind",
    pythonDispatchBind,
    METH_VARARGS,
    "bind(portA, portB): Bind a data output source port portA "
    "to a data input target port portB"
};

/**
 * @brief Python wrapper to unbind ports
 *
 * Call Dispatcher::unbind(inPortA) method
 *
 */
extern "C" PyObject*
pythonDispatchUnbind(PyObject *self, PyObject *args);

static PyMethodDef pyMethodDispatchUnbind =
{
    "unbind",
    pythonDispatchUnbind,
    METH_VARARGS,
    "unbind(inPortA): unbind a data input target port inPortA "
    "from its source"
};

#endif /* HAVE_PYTHON27 */
#endif /* SRC_PYTHONMAINAPPEXPORT_H_ */
