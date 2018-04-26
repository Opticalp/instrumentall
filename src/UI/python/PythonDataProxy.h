/**
 * Definition of the DataProxy python class
 * 
 * @file	src/UI/python/PythonDataProxy.h
 * @date	Jul 2016
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


#ifndef SRC_PYTHONDATAPROXY_H_
#define SRC_PYTHONDATAPROXY_H_

#ifdef HAVE_PYTHON27

#include "PythonAPI.h"
#include "structmember.h"

#include "core/DataProxy.h"

#include "Poco/AutoPtr.h"

// -----------------------------------------------------------------------
// Variables
// -----------------------------------------------------------------------

/// member variables for the python DataProxy class
typedef struct
{
    PyObject_HEAD ///< a refcount and a pointer to a type object (convenience python/C API macro)
    PyObject* name;  ///< name attribute
    PyObject* description;  ///< description attribute
    Poco::AutoPtr<DataProxy>* proxy; ///< pointer to the C++ internal DataProxy object
} DataProxyMembers;

/// Description of the DataProxyMembers structure
static PyMemberDef pyDataProxyMembers[] =
{
    {
        const_cast<char *>("name"),
        T_OBJECT_EX,
        offsetof(DataProxyMembers, name),
        READONLY,
        const_cast<char *>("DataProxy class name")
    },
    {
        const_cast<char *>("description"),
        T_OBJECT_EX,
        offsetof(DataProxyMembers, description),
        READONLY,
        const_cast<char *>("DataProxy description")
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
 * name of the class as argument to create a new proxy
 */
extern "C" int pyDataProxyInit(DataProxyMembers* self, PyObject *args, PyObject *kwds);

extern "C" PyObject* pyDataProxySource(DataProxyMembers *self);

static PyMethodDef pyMethodDataProxySource =
{
    "source",
    (PyCFunction)pyDataProxySource,
    METH_NOARGS,
    "Retrieve the source Port object"
};


//extern "C" PyObject* pyDataProxyTargets(DataProxyMembers *self);
//
//static PyMethodDef pyMethodDataProxyTargets =
//{
//    "targets",
//    (PyCFunction)pyDataProxyTargets,
//    METH_NOARGS,
//    "Retrieve the target Port objects"
//};

extern "C" PyObject* pyDataProxyDetach(DataProxyMembers *self);

static PyMethodDef pyMethodDataProxyDetach =
{
    "detach",
    (PyCFunction)pyDataProxyDetach,
    METH_NOARGS,
    "Detach the proxy from its source and target"
};

/// python wrapper to set the name of the dataLogger
extern "C"
PyObject* pyDataProxySetName(DataProxyMembers *self, PyObject *args);

static PyMethodDef pyMethodDataProxySetName =
{
    "setName",
    (PyCFunction)pyDataProxySetName,
    METH_VARARGS,
    "setName(newName): set a new name for the data logger, "
    "change the parameters prefix key for the conf file, "
    "load the parameters from the conf file if any. "
};


/// DataProxy::parameterSet() python wrapper
extern "C" PyObject* pyDataProxyGetParameterSet(DataProxyMembers *self);

static PyMethodDef pyMethodDataProxyGetParameterSet =
{
    "getParameterSet",
    (PyCFunction)pyDataProxyGetParameterSet,
    METH_NOARGS,
    "Retrieve the parameter set"
};

/// DataProxy::getParameterValue python wrapper
extern "C"
PyObject* pyDataProxyGetParameterValue(DataProxyMembers *self, PyObject *args);

static PyMethodDef pyMethodDataProxyGetParameterValue =
{
    "getParameterValue",
    (PyCFunction)pyDataProxyGetParameterValue,
    METH_VARARGS,
    "getParameterValue(paramName): get the value of the given parameter"
};

/// DataProxy::setParameterValue python wrapper
extern "C"
PyObject* pyDataProxySetParameterValue(DataProxyMembers *self, PyObject *args);

static PyMethodDef pyMethodDataProxySetParameterValue =
{
    "setParameterValue",
    (PyCFunction)pyDataProxySetParameterValue,
    METH_VARARGS,
    "setParameterValue(paramName, value): set the value of the given parameter"
};

/// Bind to setParameterValue and applyParameters
extern "C"
PyObject* pyDataProxySetParameterValues(DataProxyMembers *self, PyObject *args);

static PyMethodDef pyMethodDataProxySetParameterValues =
{
    "setParameterValues",
    (PyCFunction)pyDataProxySetParameterValues,
    METH_VARARGS,
    "setParameterValues(dict): set the values of the given parameters. "
    "The dict keys are the parameter names, "
    "the corresponding values are the parameter values. "
};

/// DataProxy::setVerbosity python wrapper
extern "C"
PyObject* pyDataProxySetVerbosity(DataProxyMembers *self, PyObject *args);

static PyMethodDef pyMethodDataProxySetVerbosity =
{
    "setVerbosity",
    (PyCFunction)pyDataProxySetVerbosity,
    METH_VARARGS,
    "getVerbosity: set the dataLogger logger verbosity (with integer value)"
};

/// DataProxy::getVerbosity python wrapper
extern "C"
PyObject* pyDataProxyGetVerbosity(DataProxyMembers *self, PyObject *args);

static PyMethodDef pyMethodDataProxyGetVerbosity =
{
    "getVerbosity",
    (PyCFunction)pyDataProxyGetVerbosity,
    METH_NOARGS,
    "getVerbosity: set the dataLogger logger verbosity"
};

/// exported methods
static PyMethodDef pyDataProxyMethods[] = {
        pyMethodDataProxySource,
		//pyMethodDataProxyTargets,
        pyMethodDataProxyDetach,

        pyMethodDataProxySetName,

        pyMethodDataProxyGetParameterSet,
        pyMethodDataProxyGetParameterValue,
        pyMethodDataProxySetParameterValue,
        pyMethodDataProxySetParameterValues,

        pyMethodDataProxySetVerbosity,
        pyMethodDataProxyGetVerbosity,

        {NULL} // sentinel
};

// -----------------------------------------------------------------------
// General
// -----------------------------------------------------------------------

/**
 * Deallocator
 */
extern "C" void pyDataProxyDealloc(DataProxyMembers* self);

/**
 * Allocator
 *
 * Initialize "name" and "description" to empty strings instead of
 * NULL because they are python objects.
 */
extern "C" PyObject* pyDataProxyNew(PyTypeObject* type, PyObject* args, PyObject* kwds);


/// Definition of the PyTypeObject
static PyTypeObject PythonDataProxy = {                  // SPECIFIC
    PyObject_HEAD_INIT(NULL)
    0,                          /*ob_size*/
    "instru.DataProxy",            /*tp_name*/           // SPECIFIC
    sizeof(DataProxyMembers),      /*tp_basicsize*/      // SPECIFIC
    0,                          /*tp_itemsize*/
    (destructor)pyDataProxyDealloc,/*tp_dealloc*/        // SPECIFIC
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
    "DataProxy to convert"
    "data on-line",             /* tp_doc */          // SPECIFIC
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    0,                          /* tp_richcompare */
    0,                          /* tp_weaklistoffset */
    0,                          /* tp_iter */
    0,                          /* tp_iternext */
    pyDataProxyMethods,            /* tp_methods */    // SPECIFIC
    pyDataProxyMembers,            /* tp_members */    // SPECIFIC
    0,                          /* tp_getset */
    0,                          /* tp_base */       // SPECIFIC
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
    (initproc)pyDataProxyInit,        /* tp_init */       // SPECIFIC
    0,                          /* tp_alloc */
    pyDataProxyNew,                /* tp_new */        // SPECIFIC
};

#endif /* HAVE_PYTHON27 */
#endif /* SRC_PYTHONDATAPROXY_H_ */
