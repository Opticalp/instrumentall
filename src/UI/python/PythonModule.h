/**
 * Definition of the Module python class
 * 
 * @file	src/UI/python/PythonModule.h
 * @date	jan. 2016
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


#ifndef SRC_PYTHONMODULE_H_
#define SRC_PYTHONMODULE_H_

#ifdef HAVE_PYTHON27

#include "PythonAPI.h"
#include "structmember.h"

#include "Poco/SharedPtr.h"

#include "core/Module.h"

// -----------------------------------------------------------------------
// Variables
// -----------------------------------------------------------------------

/// member variables for the python Module class
typedef struct
{
    PyObject_HEAD ///< a refcount and a pointer to a type object (convenience python/C API macro)
    PyObject* name;  ///< name attribute
    PyObject* internalName;  ///< internalName attribute
    PyObject* description;  ///< description attribute
    Poco::SharedPtr<Module*>* module; ///< pointer to the C++ internal Module object
} ModMembers;

/// Description of the ModMembers structure
static PyMemberDef pyModMembers[] =
{
    {
        const_cast<char *>("name"),
        T_OBJECT_EX,
        offsetof(ModMembers, name),
        READONLY,
        const_cast<char *>("Module name")
    },
    {
        const_cast<char *>("internalName"),
        T_OBJECT_EX,
        offsetof(ModMembers, internalName),
        READONLY,
        const_cast<char *>("Module internal name")
    },
    {
        const_cast<char *>("description"),
        T_OBJECT_EX,
        offsetof(ModMembers, description),
        READONLY,
        const_cast<char *>("Module description")
    },
    { NULL } // Sentinel
};

// -----------------------------------------------------------------------
// Methods
// -----------------------------------------------------------------------

/**
 * Initializer
 *
 * not implemented yet. Could be called with the name of the module (internal
 * or custom)
 */
extern "C" int pyModInit(ModMembers* self, PyObject *args, PyObject *kwds);

/// Module::destroy() python wrapper
extern "C" PyObject* pyModDestroy(ModMembers *self);

static PyMethodDef pyMethodModDestroy =
{
    "destroy",
    (PyCFunction)pyModDestroy,
    METH_NOARGS,
    "Cleanly self-delete this module. Future ref will drive to the empty module. "
};

/// Module::parent() python wrapper
extern "C" PyObject* pyModParent(ModMembers *self);

static PyMethodDef pyMethodModParent =
{
    "parent",
    (PyCFunction)pyModParent,
    METH_NOARGS,
    "Retrieve the parent factory"
};

/// Module::getInPorts() python wrapper
extern "C" PyObject* pyModInPorts(ModMembers *self);

static PyMethodDef pyMethodModInPorts =
{
    "inPorts",
    (PyCFunction)pyModInPorts,
    METH_NOARGS,
    "Retrieve the input ports"
};

/// Module::getOutPorts() python wrapper
extern "C" PyObject* pyModOutPorts(ModMembers *self);

static PyMethodDef pyMethodModOutPorts =
{
    "outPorts",
    (PyCFunction)pyModOutPorts,
    METH_NOARGS,
    "Retrieve the output ports"
};

/// Module::getInPort() python wrapper
extern "C" PyObject* pyModInPort(ModMembers *self, PyObject *args);

static PyMethodDef pyMethodModInPort =
{
    "inPort",
    (PyCFunction)pyModInPort,
    METH_VARARGS,
    "inPort(portName): Retrieve the input port given its name"
};

/// Module::getOutPort() python wrapper
extern "C" PyObject* pyModOutPort(ModMembers *self, PyObject *args);

static PyMethodDef pyMethodModOutPort =
{
    "outPort",
    (PyCFunction)pyModOutPort,
    METH_VARARGS,
    "outPort(portName): Retrieve the output port given its name"
};

/// Module::parameterSet() python wrapper
extern "C" PyObject* pyModGetParameterSet(ModMembers *self);

static PyMethodDef pyMethodModGetParameterSet =
{
    "getParameterSet",
    (PyCFunction)pyModGetParameterSet,
    METH_NOARGS,
    "Retrieve the parameter set"
};

/// Module::getParameterValue python wrapper
extern "C"
PyObject* pyModGetParameterValue(ModMembers *self, PyObject *args);

static PyMethodDef pyMethodModGetParameterValue =
{
    "getParameterValue",
    (PyCFunction)pyModGetParameterValue,
    METH_VARARGS,
    "getParameterValue(paramName): get the value of the given parameter"
};

/// Module::setParameterValue python wrapper
extern "C"
PyObject* pyModSetParameterValue(ModMembers *self, PyObject *args);

static PyMethodDef pyMethodModSetParameterValue =
{
    "setParameterValue",
    (PyCFunction)pyModSetParameterValue,
    METH_VARARGS,
    "setParameterValue(paramName, value): set the value of the given parameter"
};

/// Module::setVerbosity python wrapper
extern "C"
PyObject* pyModSetVerbosity(ModMembers *self, PyObject *args);

static PyMethodDef pyMethodModSetVerbosity =
{
    "setVerbosity",
    (PyCFunction)pyModSetVerbosity,
    METH_VARARGS,
    "getVerbosity: set the module logger verbosity (with integer value)"
};

/// Module::getVerbosity python wrapper
extern "C"
PyObject* pyModGetVerbosity(ModMembers *self, PyObject *args);

static PyMethodDef pyMethodModGetVerbosity =
{
    "getVerbosity",
    (PyCFunction)pyModGetVerbosity,
    METH_NOARGS,
    "getVerbosity: set the module logger verbosity"
};

/// Module::buildParameterGetter python wrapper
extern "C"
PyObject* pyModBuildParamGetter(ModMembers *self, PyObject *args);

static PyMethodDef pyMethodModBuildParamGetter =
{
    "buildParameterGetter",
    (PyCFunction)pyModBuildParamGetter,
    METH_VARARGS,
    "buildParameterGetter(paramName): "
    "build a parameter getter for the given parameter"
};

/// Module::buildParameterSetter python wrapper
extern "C"
PyObject* pyModBuildParamSetter(ModMembers *self, PyObject *args);

static PyMethodDef pyMethodModBuildParamSetter =
{
    "buildParameterSetter",
    (PyCFunction)pyModBuildParamSetter,
    METH_VARARGS,
    "buildParameterSetter(paramName): "
    "build a parameter setter for the given parameter"
};

/// Module::getParameterSetters python wrapper
extern "C"
PyObject* pyModGetParamSetters(ModMembers *self, PyObject *args);

static PyMethodDef pyMethodModGetParamSetters =
{
    "getParameterSetters",
    (PyCFunction)pyModGetParamSetters,
    METH_NOARGS,
    "Retrieve the parameterSetters of the module"
};

/// exported methods
static PyMethodDef pyModMethods[] = {
	pyMethodModDestroy,

    pyMethodModParent,

	pyMethodModInPorts,
	pyMethodModInPort,
	pyMethodModOutPorts,
	pyMethodModOutPort,

	pyMethodModGetParameterSet,
	pyMethodModGetParameterValue,
	pyMethodModSetParameterValue,

	pyMethodModSetVerbosity,
	pyMethodModGetVerbosity,

	pyMethodModBuildParamGetter,
	pyMethodModBuildParamSetter,
	pyMethodModGetParamSetters,

	{NULL} // sentinel
};

// -----------------------------------------------------------------------
// General
// -----------------------------------------------------------------------

/**
 * Deallocator
 */
extern "C" void pyModDealloc(ModMembers* self);

/**
 * Allocator
 *
 * Initialize "name", "internalName" and "description" to empty strings instead of
 * NULL because they are python objects.
 */
extern "C" PyObject* pyModNew(PyTypeObject* type, PyObject* args, PyObject* kwds);


/// Definition of the PyTypeObject
static PyTypeObject PythonModule = {             // SPECIFIC
    PyObject_HEAD_INIT(NULL)
    0,                          /*ob_size*/
    "instru.Module",            /*tp_name*/             // SPECIFIC
    sizeof(ModMembers),         /*tp_basicsize*/  // SPECIFIC
    0,                          /*tp_itemsize*/
    (destructor)pyModDealloc,   /*tp_dealloc*/        // SPECIFIC
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
    "Module entity like "
    "hardware device interface, "
    "processing unit, ...",     /* tp_doc */        // SPECIFIC
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    0,                          /* tp_richcompare */
    0,                          /* tp_weaklistoffset */
    0,                          /* tp_iter */
    0,                          /* tp_iternext */
    pyModMethods,               /* tp_methods */    // SPECIFIC
    pyModMembers,               /* tp_members */    // SPECIFIC
    0,                          /* tp_getset */
    0,                          /* tp_base */       // SPECIFIC
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
    (initproc)pyModInit,        /* tp_init */       // SPECIFIC
    0,                          /* tp_alloc */
    pyModNew,                   /* tp_new */        // SPECIFIC
};

#endif /* HAVE_PYTHON27 */
#endif /* SRC_PYTHONMODULE_H_ */
