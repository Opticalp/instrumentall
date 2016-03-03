/**
 * Definition of the OutPort python class
 * 
 * @file	src/PythonOutPort.h
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


#ifndef SRC_PYTHONOUTPORT_H_
#define SRC_PYTHONOUTPORT_H_

#ifdef HAVE_PYTHON27

#include "PythonAPI.h"
#include "structmember.h"

#include "Poco/SharedPtr.h"

#include "OutPort.h"

// -----------------------------------------------------------------------
// Variables
// -----------------------------------------------------------------------

/// member variables for the python OutPort class
typedef struct
{
    PyObject_HEAD ///< a refcount and a pointer to a type object (convenience python/C API macro)
    PyObject* name;  ///< name attribute
    PyObject* description;  ///< description attribute
    Poco::SharedPtr<OutPort*>* outPort; ///< pointer to the C++ internal OutPort object
} OutPortMembers;

/// Description of the OutPortMembers structure
static PyMemberDef pyOutPortMembers[] =
{
    {
        const_cast<char *>("name"),
        T_OBJECT_EX,
        offsetof(OutPortMembers, name),
        READONLY,
        const_cast<char *>("Output Port name")
    },
    {
        const_cast<char *>("description"),
        T_OBJECT_EX,
        offsetof(OutPortMembers, description),
        READONLY,
        const_cast<char *>("Output Port description")
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
extern "C" int pyOutPortInit(OutPortMembers* self, PyObject *args, PyObject *kwds);

/// OutPort::parent python wrapper
extern "C" PyObject* pyOutPortParent(OutPortMembers *self);

static PyMethodDef pyMethodOutPortParent =
{
    "parent",
    (PyCFunction)pyOutPortParent,
    METH_NOARGS,
    "Retrieve the parent module"
};

/// python wrapper to get the target ports of the current output port
extern "C" PyObject* pyOutPortGetTargetPorts(OutPortMembers *self);

static PyMethodDef pyMethodOutPortGetTargetPorts =
{
    "getTargetPorts",
    (PyCFunction)pyOutPortGetTargetPorts,
    METH_NOARGS,
    "Retrieve the target ports"
};

/// python wrapper to get the target ports of the current output port
extern "C" PyObject* pyOutPortGetSeqTargetPorts(OutPortMembers *self);

static PyMethodDef pyMethodOutPortGetSeqTargetPorts =
{
    "getSeqTargetPorts",
    (PyCFunction)pyOutPortGetSeqTargetPorts,
    METH_NOARGS,
    "Retrieve the data sequence target ports"
};

/// exported methods
static PyMethodDef pyOutPortMethods[] = {
        pyMethodOutPortParent,

        pyMethodOutPortGetTargetPorts,
        pyMethodOutPortGetSeqTargetPorts,

        {NULL} // sentinel
};

// -----------------------------------------------------------------------
// General
// -----------------------------------------------------------------------

/**
 * Deallocator
 */
extern "C" void pyOutPortDealloc(OutPortMembers* self);

/**
 * Allocator
 *
 * Initialize "name" and "description" to empty strings instead of
 * NULL because they are python objects.
 */
extern "C" PyObject* pyOutPortNew(PyTypeObject* type, PyObject* args, PyObject* kwds);


/// Definition of the PyTypeObject
static PyTypeObject PythonOutPort = {                  // SPECIFIC
    PyObject_HEAD_INIT(NULL)
    0,                          /*ob_size*/
    "instru.OutPort",            /*tp_name*/           // SPECIFIC
    sizeof(OutPortMembers),      /*tp_basicsize*/      // SPECIFIC
    0,                          /*tp_itemsize*/
    (destructor)pyOutPortDealloc,/*tp_dealloc*/        // SPECIFIC
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
    "Output port entity to "
    "send data to another "
    "module port (target) ",    /* tp_doc */          // SPECIFIC
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    0,                          /* tp_richcompare */
    0,                          /* tp_weaklistoffset */
    0,                          /* tp_iter */
    0,                          /* tp_iternext */
    pyOutPortMethods,            /* tp_methods */    // SPECIFIC
    pyOutPortMembers,            /* tp_members */    // SPECIFIC
    0,                          /* tp_getset */
    0,                          /* tp_base */       // SPECIFIC
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
    (initproc)pyOutPortInit,        /* tp_init */       // SPECIFIC
    0,                          /* tp_alloc */
    pyOutPortNew,                /* tp_new */        // SPECIFIC
};

#endif /* HAVE_PYTHON27 */
#endif /* SRC_PYTHONOUTPORT_H_ */
