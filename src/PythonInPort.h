/**
 * Definition of the InPort python class
 * 
 * @file	src/PythonInPort.h
 * @date	feb. 2016
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


#ifndef SRC_PYTHONINPORT_H_
#define SRC_PYTHONINPORT_H_

#ifdef HAVE_PYTHON27

#include "PythonAPI.h"
#include "structmember.h"

#include "Poco/SharedPtr.h"

#include "InDataPort.h"

// -----------------------------------------------------------------------
// Variables
// -----------------------------------------------------------------------

/// member variables for the python InPort class
typedef struct
{
    PyObject_HEAD ///< a refcount and a pointer to a type object (convenience python/C API macro)
    PyObject* name;  ///< name attribute
    PyObject* description;  ///< description attribute
    Poco::SharedPtr<InDataPort*>* inPort; ///< pointer to the C++ internal InPort object
} InPortMembers;

/// Description of the InPortMembers structure
static PyMemberDef pyInPortMembers[] =
{
    {
        const_cast<char *>("name"),
        T_OBJECT_EX,
        offsetof(InPortMembers, name),
        READONLY,
        const_cast<char *>("Input Port name")
    },
    {
        const_cast<char *>("description"),
        T_OBJECT_EX,
        offsetof(InPortMembers, description),
        READONLY,
        const_cast<char *>("Input Port description")
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
 * name of the factory as argument.
 */
extern "C" int pyInPortInit(InPortMembers* self, PyObject *args, PyObject *kwds);

/// InPort::parent python wrapper
extern "C" PyObject* pyInPortParent(InPortMembers *self);

static PyMethodDef pyMethodInPortParent =
{
    "parent",
    (PyCFunction)pyInPortParent,
    METH_NOARGS,
    "Retrieve the parent module"
};

/// python wrapper to get the source port of the current input port
extern "C" PyObject* pyInPortGetSourcePort(InPortMembers *self);

static PyMethodDef pyMethodInPortGetSourcePort =
{
    "getSourcePort",
    (PyCFunction)pyInPortGetSourcePort,
    METH_NOARGS,
    "Retrieve the source port"
};

/// python wrapper to get the source port of the current input port
extern "C" PyObject* pyInPortGetSeqSourcePort(InPortMembers *self);

static PyMethodDef pyMethodInPortGetSeqSourcePort =
{
    "getSeqSourcePort",
    (PyCFunction)pyInPortGetSeqSourcePort,
    METH_NOARGS,
    "Retrieve the data sequence source port"
};

/// python wrapper to hold the data coming from this port
extern "C" PyObject* pyInPortHoldData(InPortMembers *self, PyObject* args);

static PyMethodDef pyMethodInPortHoldData =
{
    "holdData",
    (PyCFunction)pyInPortHoldData,
    METH_VARARGS,
    "holdData(\"on\"): re-use the same data if not expired. "
    "holdData(\"off\"): release the data holding. "
};

/// exported methods
static PyMethodDef pyInPortMethods[] = {
        pyMethodInPortParent,

        pyMethodInPortGetSourcePort,
        pyMethodInPortGetSeqSourcePort,

		pyMethodInPortHoldData,

        {NULL} // sentinel
};

// -----------------------------------------------------------------------
// General
// -----------------------------------------------------------------------

/**
 * Deallocator
 */
extern "C" void pyInPortDealloc(InPortMembers* self);

/**
 * Allocator
 *
 * Initialize "name" and "description" to empty strings instead of
 * NULL because they are python objects.
 */
extern "C" PyObject* pyInPortNew(PyTypeObject* type, PyObject* args, PyObject* kwds);


/// Definition of the PyTypeObject
static PyTypeObject PythonInPort = {                  // SPECIFIC
    PyObject_HEAD_INIT(NULL)
    0,                          /*ob_size*/
    "instru.InPort",            /*tp_name*/           // SPECIFIC
    sizeof(InPortMembers),      /*tp_basicsize*/      // SPECIFIC
    0,                          /*tp_itemsize*/
    (destructor)pyInPortDealloc,/*tp_dealloc*/        // SPECIFIC
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
    "Input port entity to "
    "receive data from another "
    "module port (source) ",    /* tp_doc */          // SPECIFIC
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    0,                          /* tp_richcompare */
    0,                          /* tp_weaklistoffset */
    0,                          /* tp_iter */
    0,                          /* tp_iternext */
    pyInPortMethods,            /* tp_methods */    // SPECIFIC
    pyInPortMembers,            /* tp_members */    // SPECIFIC
    0,                          /* tp_getset */
    0,                          /* tp_base */       // SPECIFIC
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
    (initproc)pyInPortInit,        /* tp_init */       // SPECIFIC
    0,                          /* tp_alloc */
    pyInPortNew,                /* tp_new */        // SPECIFIC
};

#endif /* HAVE_PYTHON27 */
#endif /* SRC_PYTHONINPORT_H_ */
