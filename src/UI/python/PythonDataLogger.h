/**
 * Definition of the DataLogger python class
 * 
 * @file	src/UI/python/PythonDataLogger.h
 * @date	Mar 2016
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


#ifndef SRC_PYTHONDATALOGGER_H_
#define SRC_PYTHONDATALOGGER_H_

#ifdef HAVE_PYTHON27

#include "PythonAPI.h"
#include "structmember.h"

#include "core/DataLogger.h"

#include "Poco/AutoPtr.h"

// -----------------------------------------------------------------------
// Variables
// -----------------------------------------------------------------------

/// member variables for the python DataLogger class
typedef struct
{
    PyObject_HEAD ///< a refcount and a pointer to a type object (convenience python/C API macro)
    PyObject* name;  ///< name attribute
    PyObject* description;  ///< description attribute
    Poco::AutoPtr<DataLogger>* logger; ///< pointer to the C++ internal DataLogger object
} DataLoggerMembers;

/// Description of the DataLoggerMembers structure
static PyMemberDef pyDataLoggerMembers[] =
{
    {
        const_cast<char *>("name"),
        T_OBJECT_EX,
        offsetof(DataLoggerMembers, name),
        READONLY,
        const_cast<char *>("DataLogger class name")
    },
    {
        const_cast<char *>("description"),
        T_OBJECT_EX,
        offsetof(DataLoggerMembers, description),
        READONLY,
        const_cast<char *>("DataLogger description")
    },
    { NULL } // Sentinel
};

// -----------------------------------------------------------------------
// Methods
// -----------------------------------------------------------------------

/**
 * Initializer
 *
 * The python version of this initializer has to be called with the
 * name of the class as argument to create a new logger
 */
extern "C" int pyDataLoggerInit(DataLoggerMembers* self, PyObject *args, PyObject *kwds);

/// DataLogger::parent python wrapper
extern "C" PyObject* pyDataLoggerSource(DataLoggerMembers *self);

static PyMethodDef pyMethodDataLoggerSource =
{
    "portSource",
    (PyCFunction)pyDataLoggerSource,
    METH_NOARGS,
    "Retrieve the source Port object"
};

/// python wrapper to get the target ports of the current output port
extern "C" PyObject* pyDataLoggerDetach(DataLoggerMembers *self);

static PyMethodDef pyMethodDataLoggerDetach =
{
    "detach",
    (PyCFunction)pyDataLoggerDetach,
    METH_NOARGS,
    "Detach the logger from its data source"
};

/// DataLogger::parameterSet() python wrapper
extern "C" PyObject* pyDataLoggerGetParameterSet(DataLoggerMembers *self);

static PyMethodDef pyMethodDataLoggerGetParameterSet =
{
    "getParameterSet",
    (PyCFunction)pyDataLoggerGetParameterSet,
    METH_NOARGS,
    "Retrieve the parameter set"
};

/// DataLogger::getParameterValue python wrapper
extern "C"
PyObject* pyDataLoggerGetParameterValue(DataLoggerMembers *self, PyObject *args);

static PyMethodDef pyMethodDataLoggerGetParameterValue =
{
    "getParameterValue",
    (PyCFunction)pyDataLoggerGetParameterValue,
    METH_VARARGS,
    "getParameterValue(paramName): get the value of the given parameter"
};

/// DataLogger::setParameterValue python wrapper
extern "C"
PyObject* pyDataLoggerSetParameterValue(DataLoggerMembers *self, PyObject *args);

static PyMethodDef pyMethodDataLoggerSetParameterValue =
{
    "setParameterValue",
    (PyCFunction)pyDataLoggerSetParameterValue,
    METH_VARARGS,
    "setParameterValue(paramName, value): set the value of the given parameter"
};

/// DataLogger::setVerbosity python wrapper
extern "C"
PyObject* pyDataLoggerSetVerbosity(DataLoggerMembers *self, PyObject *args);

static PyMethodDef pyMethodDataLoggerSetVerbosity =
{
    "setVerbosity",
    (PyCFunction)pyDataLoggerSetVerbosity,
    METH_VARARGS,
    "getVerbosity: set the dataLogger logger verbosity (with integer value)"
};

/// DataLogger::getVerbosity python wrapper
extern "C"
PyObject* pyDataLoggerGetVerbosity(DataLoggerMembers *self, PyObject *args);

static PyMethodDef pyMethodDataLoggerGetVerbosity =
{
    "getVerbosity",
    (PyCFunction)pyDataLoggerGetVerbosity,
    METH_NOARGS,
    "getVerbosity: set the dataLogger logger verbosity"
};

/// exported methods
static PyMethodDef pyDataLoggerMethods[] = {
        pyMethodDataLoggerSource,
        pyMethodDataLoggerDetach,

        pyMethodDataLoggerGetParameterSet,
        pyMethodDataLoggerGetParameterValue,
        pyMethodDataLoggerSetParameterValue,

        pyMethodDataLoggerSetVerbosity,
        pyMethodDataLoggerGetVerbosity,

        {NULL} // sentinel
};

// -----------------------------------------------------------------------
// General
// -----------------------------------------------------------------------

/**
 * Deallocator
 */
extern "C" void pyDataLoggerDealloc(DataLoggerMembers* self);

/**
 * Allocator
 *
 * Initialize "name" and "description" to empty strings instead of
 * NULL because they are python objects.
 */
extern "C" PyObject* pyDataLoggerNew(PyTypeObject* type, PyObject* args, PyObject* kwds);


/// Definition of the PyTypeObject
static PyTypeObject PythonDataLogger = {                  // SPECIFIC
    PyObject_HEAD_INIT(NULL)
    0,                          /*ob_size*/
    "instru.DataLogger",            /*tp_name*/           // SPECIFIC
    sizeof(DataLoggerMembers),      /*tp_basicsize*/      // SPECIFIC
    0,                          /*tp_itemsize*/
    (destructor)pyDataLoggerDealloc,/*tp_dealloc*/        // SPECIFIC
    0,                          /*tp_print*/
    0,                          /*tp_getattr*/
    0,                          /*tp_setattr*/
    0,                          /*tp_compare*/
    0,                          /*tp_repr*/
    0,                          /*tp_as_number*/
    0,                          /*tp_as_sequence*/
    0,                          /*tp_as_mapping*/
    0,                          /*tp_hash */
    0,                          /*tp_call*/
    0,                          /*tp_str*/
    0,                          /*tp_getattro*/
    0,                          /*tp_setattro*/
    0,                          /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,    /*tp_flags*/
    "DataLogger to automatically"
    "log newly created data ",  /* tp_doc */          // SPECIFIC
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    0,                          /* tp_richcompare */
    0,                          /* tp_weaklistoffset */
    0,                          /* tp_iter */
    0,                          /* tp_iternext */
    pyDataLoggerMethods,            /* tp_methods */    // SPECIFIC
    pyDataLoggerMembers,            /* tp_members */    // SPECIFIC
    0,                          /* tp_getset */
    0,                          /* tp_base */       // SPECIFIC
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
    (initproc)pyDataLoggerInit,        /* tp_init */       // SPECIFIC
    0,                          /* tp_alloc */
    pyDataLoggerNew,                /* tp_new */        // SPECIFIC
};

#endif /* HAVE_PYTHON27 */
#endif /* SRC_PYTHONDATALOGGER_H_ */
