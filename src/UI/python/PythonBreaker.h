/**
 * Definition of the Breaker python class
 *
 * @file	src/PythonBreaker.h
 * @date	Jul. 2016
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

#ifndef SRC_PYTHONBREAKER_H_
#define SRC_PYTHONBREAKER_H_

#ifdef HAVE_PYTHON27

#include "PythonAPI.h"
#include "structmember.h"

#include "core/Breaker.h"

// -----------------------------------------------------------------------
// Variables
// -----------------------------------------------------------------------

/// member variables for the python Breaker class
typedef struct
{
    PyObject_HEAD ///< a refcount and a pointer to a type object (convenience python/C API macro)
    Breaker* breaker; ///< pointer to the C++ internal Breaker object
} BreakerMembers;

/// Description of the BreakerMembers structure
static PyMemberDef pyBreakerMembers[] =
{
    { NULL } // Sentinel
};

// -----------------------------------------------------------------------
// Methods
// -----------------------------------------------------------------------

/**
 * Initializer
 *
 * The python version of this initializer does not support arguments
 */
extern "C" int pyBreakerInit(BreakerMembers* self, PyObject* args, PyObject *kwds);

/// python wrapper to break all the connections from a source
extern "C" PyObject* pyBreakerBreakAllTargets(BreakerMembers *self, PyObject* args);

static PyMethodDef pyMethodBreakerBreakAllTargets =
{
    "breakAllTargets",
    (PyCFunction)pyBreakerBreakAllTargets,
    METH_VARARGS,
    "breakAllTargets(source): break all the connections issued from the given source"
};

/// python wrapper to break the connection to a target
extern "C" PyObject* pyBreakerBreakSource(BreakerMembers *self, PyObject* args);

static PyMethodDef pyMethodBreakerBreakSource =
{
    "breakSource",
    (PyCFunction)pyBreakerBreakSource,
    METH_VARARGS,
    "breakSource(target): break the connection to the given target"
};

/// python wrapper to release the breaker, i.e. re-build the bindings
extern "C" PyObject* pyBreakerRelease(BreakerMembers *self);

static PyMethodDef pyMethodBreakerRelease =
{
    "release",
    (PyCFunction)pyBreakerRelease,
    METH_NOARGS,
    "Release the breaks hold by this breaker"
};

/// exported methods
static PyMethodDef pyBreakerMethods[] = {
		pyMethodBreakerBreakAllTargets,
		pyMethodBreakerBreakSource,
		pyMethodBreakerRelease,

        {NULL} // sentinel
};

// -----------------------------------------------------------------------
// General
// -----------------------------------------------------------------------

/**
 * Deallocator
 */
extern "C" void pyBreakerDealloc(BreakerMembers* self);

/**
 * Allocator
 *
 * Nothing to do at the python side...
 */
extern "C" PyObject* pyBreakerNew(PyTypeObject* type, PyObject* args, PyObject* kwds);


/// Definition of the PyTypeObject
static PyTypeObject PythonBreaker = {                  // SPECIFIC
    PyObject_HEAD_INIT(NULL)
    0,                          /*ob_size*/
    "instru.Breaker",            /*tp_name*/           // SPECIFIC
    sizeof(BreakerMembers),      /*tp_basicsize*/      // SPECIFIC
    0,                          /*tp_itemsize*/
    (destructor)pyBreakerDealloc,/*tp_dealloc*/        // SPECIFIC
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
    "Breaker entity to "
    "temporary cut "
    "the data flow ",    /* tp_doc */          // SPECIFIC
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    0,                          /* tp_richcompare */
    0,                          /* tp_weaklistoffset */
    0,                          /* tp_iter */
    0,                          /* tp_iternext */
    pyBreakerMethods,            /* tp_methods */    // SPECIFIC
    pyBreakerMembers,            /* tp_members */    // SPECIFIC
    0,                          /* tp_getset */
    0,                          /* tp_base */       // SPECIFIC
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
    (initproc)pyBreakerInit,        /* tp_init */       // SPECIFIC
    0,                          /* tp_alloc */
    pyBreakerNew,                /* tp_new */        // SPECIFIC
};

#endif /* HAVE_PYTHON27 */
#endif /* SRC_PYTHONBREAKER_H_ */
