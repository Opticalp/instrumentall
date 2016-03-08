/**
 * Definition of the Data python class
 * 
 * @file	src/PythonData.h
 * @date	mar. 2016
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


#ifndef SRC_PYTHONDATA_H_
#define SRC_PYTHONDATA_H_

#ifdef HAVE_PYTHON27

#include "PythonAPI.h"
#include "structmember.h"

#include "Poco/SharedPtr.h"

class DataItem;

// -----------------------------------------------------------------------
// Variables
// -----------------------------------------------------------------------

/// member variables for the python Data class
typedef struct
{
    PyObject_HEAD ///< a refcount and a pointer to a type object (convenience python/C API macro)
    Poco::SharedPtr<DataItem*>* data; ///< pointer to the C++ internal DataItem object
} DataMembers;

/// Description of the DataMembers structure
static PyMemberDef pyDataMembers[] =
{
    { NULL } // Sentinel
};

// -----------------------------------------------------------------------
// Methods
// -----------------------------------------------------------------------

/**
 * Initializer
 *
 * The Data direct initializing is not supported
 */
extern "C" int pyDataInit(DataMembers* self, PyObject *args, PyObject *kwds);

/// Data::parent python wrapper
extern "C" PyObject* pyDataParent(DataMembers *self);

static PyMethodDef pyMethodDataParent =
{
    "parent",
    (PyCFunction)pyDataParent,
    METH_NOARGS,
    "Retrieve the parent OutPort"
};

/// python wrapper to get the current value of the data item
extern "C" PyObject* pyDataGetValue(DataMembers *self);

static PyMethodDef pyMethodDataGetValue =
{
    "getValue",
    (PyCFunction)pyDataGetValue,
    METH_NOARGS,
    "Retrieve the current value"
};

/// python wrapper to register a logger to the data item
extern "C" PyObject* pyDataRegister(DataMembers *self, PyObject* args);

static PyMethodDef pyMethodDataRegister =
{
    "register",
    (PyCFunction)pyDataRegister,
    METH_VARARGS,
    "register(logger): Register the DataLogger \"logger\" to log the data"
};

/// exported methods
static PyMethodDef pyDataMethods[] = {
        pyMethodDataParent,
        pyMethodDataGetValue,
        pyMethodDataRegister,

        {NULL} // sentinel
};

// -----------------------------------------------------------------------
// General
// -----------------------------------------------------------------------

/**
 * Deallocator
 */
extern "C" void pyDataDealloc(DataMembers* self);

/**
 * Allocator
 *
 * Initialize "name" and "description" to empty strings instead of
 * NULL because they are python objects.
 */
extern "C" PyObject* pyDataNew(PyTypeObject* type, PyObject* args, PyObject* kwds);


/// Definition of the PyTypeObject
static PyTypeObject PythonData = {                  // SPECIFIC
    PyObject_HEAD_INIT(NULL)
    0,                          /*ob_size*/
    "instru.Data",            /*tp_name*/           // SPECIFIC
    sizeof(DataMembers),      /*tp_basicsize*/      // SPECIFIC
    0,                          /*tp_itemsize*/
    (destructor)pyDataDealloc,/*tp_dealloc*/        // SPECIFIC
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
    "data entity attached to "
    "an output port ",          /* tp_doc */          // SPECIFIC
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    0,                          /* tp_richcompare */
    0,                          /* tp_weaklistoffset */
    0,                          /* tp_iter */
    0,                          /* tp_iternext */
    pyDataMethods,            /* tp_methods */    // SPECIFIC
    pyDataMembers,            /* tp_members */    // SPECIFIC
    0,                          /* tp_getset */
    0,                          /* tp_base */       // SPECIFIC
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
    (initproc)pyDataInit,        /* tp_init */       // SPECIFIC
    0,                          /* tp_alloc */
    pyDataNew,                /* tp_new */        // SPECIFIC
};

#endif /* HAVE_PYTHON27 */
#endif /* SRC_PYTHONDATA_H_ */
