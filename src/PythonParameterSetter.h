/**
 * @file	src/PythonParameterSetter.h
 * @date	Aug. 2016
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

#ifndef SRC_PYTHONPARAMETERSETTER_H_
#define SRC_PYTHONPARAMETERSETTER_H_



#ifdef HAVE_PYTHON27

#include "PythonAPI.h"
#include "structmember.h"

#include "ParameterSetter.h"

#include "Poco/AutoPtr.h"

// -----------------------------------------------------------------------
// Variables
// -----------------------------------------------------------------------

/// member variables for the python DataProxy class
typedef struct
{
    PyObject_HEAD ///< a refcount and a pointer to a type object (convenience python/C API macro)
    Poco::AutoPtr<ParameterSetter>* setter; ///< pointer to the C++ internal ParameterSetter object
} ParameterSetterMembers;

/// Description of the ParameterSetterMembers structure
static PyMemberDef pyParameterSetterMembers[] =
{
    { NULL } // Sentinel
};

// -----------------------------------------------------------------------
// Methods
// -----------------------------------------------------------------------

/**
 * Initializer
 *
 * The python version of this initializer should not be directly called.
 * Use Module.buildParameterSetter instead
 */
extern "C" int pyParameterSetterInit(ParameterSetterMembers* self, PyObject *args, PyObject *kwds);

extern "C" PyObject* pyParameterSetterParameterName(ParameterSetterMembers *self);

static PyMethodDef pyMethodParameterSetterParameterName =
{
    "parameterName",
    (PyCFunction)pyParameterSetterParameterName,
    METH_NOARGS,
    "Retrieve the name of the parent parameter as: moduleName.paramName"
};


/// exported methods
static PyMethodDef pyParameterSetterMethods[] = {
        pyMethodParameterSetterParameterName,

        {NULL} // sentinel
};

// -----------------------------------------------------------------------
// General
// -----------------------------------------------------------------------

/**
 * Deallocator
 */
extern "C" void pyParameterSetterDealloc(ParameterSetterMembers* self);

/**
 * Allocator
 */
extern "C" PyObject* pyParameterSetterNew(PyTypeObject* type, PyObject* args, PyObject* kwds);


/// Definition of the PyTypeObject
static PyTypeObject PythonParameterSetter = {                  // SPECIFIC
    PyObject_HEAD_INIT(NULL)
    0,                          /*ob_size*/
    "instru.ParameterSetter",            /*tp_name*/           // SPECIFIC
    sizeof(ParameterSetterMembers),      /*tp_basicsize*/      // SPECIFIC
    0,                          /*tp_itemsize*/
    (destructor)pyParameterSetterDealloc,/*tp_dealloc*/        // SPECIFIC
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
    "ParameterSetter to set "
    "a parameter value "
    "from a data source",       /* tp_doc */          // SPECIFIC
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    0,                          /* tp_richcompare */
    0,                          /* tp_weaklistoffset */
    0,                          /* tp_iter */
    0,                          /* tp_iternext */
    pyParameterSetterMethods,            /* tp_methods */    // SPECIFIC
    pyParameterSetterMembers,            /* tp_members */    // SPECIFIC
    0,                          /* tp_getset */
    0,                          /* tp_base */       // SPECIFIC
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
    (initproc)pyParameterSetterInit,        /* tp_init */       // SPECIFIC
    0,                          /* tp_alloc */
    pyParameterSetterNew,                /* tp_new */        // SPECIFIC
};

#endif /* HAVE_PYTHON27 */


#endif /* SRC_PYTHONPARAMETERSETTER_H_ */
