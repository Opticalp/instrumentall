/**
 * Definition of the Factory python class
 * 
 * @file	src/UI/python/PythonModuleFactory.h
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


#ifndef SRC_PYTHONMODULEFACTORY_H_
#define SRC_PYTHONMODULEFACTORY_H_

#ifdef HAVE_PYTHON27

#include "PythonAPI.h"
#include "structmember.h"

#include "Poco/SharedPtr.h"

#include "core/ModuleFactory.h"

// -----------------------------------------------------------------------
// Variables
// -----------------------------------------------------------------------

/// member variables for the python Factory class
typedef struct
{
    PyObject_HEAD ///< a refcount and a pointer to a type object (convenience python/C API macro)
    PyObject* name;  ///< name attribute
    PyObject* description;  ///< description attribute
    Poco::SharedPtr<ModuleFactory*>* moduleFactory; ///< pointer to the C++ internal ModuleFactory object
} ModFactMembers;

/// Description of the ModuleFactoryMembers structure
static PyMemberDef pyModFactMembers[] =
{
    {
        const_cast<char *>("name"),
        T_OBJECT_EX,
        offsetof(ModFactMembers, name),
        READONLY,
        const_cast<char *>("Module factory name")
    },
    {
        const_cast<char *>("description"),
        T_OBJECT_EX,
        offsetof(ModFactMembers, description),
        READONLY,
        const_cast<char *>("Module factory description")
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
extern "C" int pyModFactInit(ModFactMembers* self, PyObject *args, PyObject *kwds);

/// ModuleFactory::select python wrapper
extern "C" PyObject* pyModFactSelect(ModFactMembers *self, PyObject *args);

static PyMethodDef pyMethodModFactSelect =
{
    "select",
    (PyCFunction)pyModFactSelect,
    METH_VARARGS,
    "Refine factory settings to return a new factory"
};

/// ModuleFactory::selectDescription python wrapper
extern "C" PyObject* pyModFactSelectDescription(ModFactMembers *self);

static PyMethodDef pyMethodModFactSelectDescription =
{
    "selectDescription",
    (PyCFunction)pyModFactSelectDescription,
    METH_NOARGS,
    "select() usage help"
};

/// ModuleFactory::selectValueList python wrapper
extern "C" PyObject* pyModFactSelectValueList(ModFactMembers *self);

static PyMethodDef pyMethodModFactSelectValueList =
{
    "selectValueList",
    (PyCFunction)pyModFactSelectValueList,
    METH_NOARGS,
    "List select() possible values. "
    "An empty string means that the choice is open"
};

/// ModuleFactoryBranch::getSelector python wrapper
extern "C" PyObject* pyModFactGetSelector(ModFactMembers *self);

static PyMethodDef pyMethodModFactGetSelector =
{
    "getSelector",
    (PyCFunction)pyModFactGetSelector,
    METH_NOARGS,
    "Get the selector that built this factory. "
    "A root factory returns NONE"
};

/// ModuleFactoryBranch::parent python wrapper
extern "C" PyObject* pyModFactParent(ModFactMembers *self);

static PyMethodDef pyMethodModFactParent =
{
    "parent",
    (PyCFunction)pyModFactParent,
    METH_NOARGS,
    "Get the parent factory that built this factory. "
    "A root factory returns NONE"
};

/// ModuleFactory::countRemain python wrapper
extern "C" PyObject* pyModFactCountRemain(ModFactMembers *self);

static PyMethodDef pyMethodModFactCountRemain =
{
    "countRemain",
    (PyCFunction)pyModFactCountRemain,
    METH_NOARGS,
    "Count how many times create() can be called. "
};

/// ModuleFactory::isLeaf python wrapper
extern "C" PyObject* pyModFactIsLeaf(ModFactMembers *self);

static PyMethodDef pyMethodModFactIsLeaf =
{
    "isLeaf",
    (PyCFunction)pyModFactIsLeaf,
    METH_NOARGS,
    "Check if the current factory is a leaf factory. "
};

/// ModuleFactory::create python wrapper
extern "C" PyObject* pyModFactCreate(ModFactMembers *self, PyObject *args);

static PyMethodDef pyMethodModFactCreate =
{
    "create",
    (PyCFunction)pyModFactCreate,
    METH_VARARGS,
    "Create a new Module. "
};

/// ModuleFactory::getChildModules python wrapper
extern "C" PyObject* pyModFactGetChildModules(ModFactMembers *self);

static PyMethodDef pyMethodModFactGetChildModules =
{
    "getChildModules",
    (PyCFunction)pyModFactGetChildModules,
    METH_NOARGS,
    "Retrieve all the child modules: self children if the factory is a leaf,"
    "or children of child factories. "
};

/// ModuleFactory::setVerbosity python wrapper
extern "C"
PyObject* pyModFactSetVerbosity(ModFactMembers *self, PyObject *args);

static PyMethodDef pyMethodModFactSetVerbosity =
{
    "setVerbosity",
    (PyCFunction)pyModFactSetVerbosity,
    METH_VARARGS,
    "getVerbosity: set the module factory logger verbosity (with integer value)"
};

/// ModuleFactory::getVerbosity python wrapper
extern "C"
PyObject* pyModFactGetVerbosity(ModFactMembers *self, PyObject *args);

static PyMethodDef pyMethodModFactGetVerbosity =
{
    "getVerbosity",
    (PyCFunction)pyModFactGetVerbosity,
    METH_NOARGS,
    "getVerbosity: set the module factory logger verbosity"
};

/// exported methods
static PyMethodDef pyModFactMethods[] = {

        pyMethodModFactSelect,
        pyMethodModFactSelectDescription,
        pyMethodModFactSelectValueList,

		pyMethodModFactGetSelector,
		pyMethodModFactParent,

        pyMethodModFactCountRemain,
        pyMethodModFactIsLeaf,
        pyMethodModFactCreate,
        pyMethodModFactGetChildModules,

		pyMethodModFactSetVerbosity,
		pyMethodModFactGetVerbosity,

        {NULL} // sentinel
};

// -----------------------------------------------------------------------
// General
// -----------------------------------------------------------------------

/**
 * Deallocator
 */
extern "C" void pyModFactDealloc(ModFactMembers* self);

/**
 * Allocator
 *
 * Initialize "name" and "description" to empty strings instead of
 * NULL because they are python objects.
 */
extern "C" PyObject* pyModFactNew(PyTypeObject* type, PyObject* args, PyObject* kwds);


/// Definition of the PyTypeObject
static PyTypeObject PythonModuleFactory = {             // SPECIFIC
    PyObject_HEAD_INIT(NULL)
    0,                          /*ob_size*/
    "instru.Factory",           /*tp_name*/             // SPECIFIC
    sizeof(ModFactMembers),     /*tp_basicsize*/  // SPECIFIC
    0,                          /*tp_itemsize*/
    (destructor)pyModFactDealloc, /*tp_dealloc*/        // SPECIFIC
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
    "Module factory to create modules",   /* tp_doc */     // SPECIFIC
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    0,                          /* tp_richcompare */
    0,                          /* tp_weaklistoffset */
    0,                          /* tp_iter */
    0,                          /* tp_iternext */
    pyModFactMethods,        /* tp_methods */    // SPECIFIC
    pyModFactMembers,        /* tp_members */    // SPECIFIC
    0,                          /* tp_getset */
    0,                          /* tp_base */       // SPECIFIC
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
    (initproc)pyModFactInit, /* tp_init */       // SPECIFIC
    0,                          /* tp_alloc */
    pyModFactNew,          /* tp_new */        // SPECIFIC
};


#endif /* HAVE_PYTHON27 */
#endif /* SRC_PYTHONMODULEFACTORY_H_ */
