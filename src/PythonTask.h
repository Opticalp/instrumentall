/**
 * Definition of the Task python class
 * 
 * @file	src/PythonTask.h
 * @date	june 2016
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


#ifndef SRC_PYTHONTASK_H_
#define SRC_PYTHONTASK_H_

#ifdef HAVE_PYTHON27

#include "PythonAPI.h"
#include "structmember.h"

#include "Poco/AutoPtr.h"

#include "ModuleTask.h"

// -----------------------------------------------------------------------
// Variables
// -----------------------------------------------------------------------

/// member variables for the python Task class
typedef struct
{
    PyObject_HEAD ///< a refcount and a pointer to a type object (convenience python/C API macro)
    PyObject* name;  ///< name attribute
    Poco::AutoPtr<ModuleTask>* task; ///< pointer to the C++ internal ModuleTask object
} TaskMembers;

/// Description of the TaskMembers structure
static PyMemberDef pyTaskMembers[] =
{
    {
        const_cast<char *>("name"),
        T_OBJECT_EX,
        offsetof(TaskMembers, name),
        READONLY,
        const_cast<char *>("Task name")
    },
    { NULL } // Sentinel
};

// -----------------------------------------------------------------------
// Methods
// -----------------------------------------------------------------------

/**
 * Initializer
 *
 * not implemented yet. Could be called with the name of the task (internal
 * or custom)
 */
extern "C" int pyTaskInit(TaskMembers* self, PyObject *args, PyObject *kwds);

/// Task::module() python wrapper
extern "C" PyObject* pyTaskModule(TaskMembers *self);

static PyMethodDef pyMethodTaskModule =
{
    "module",
    (PyCFunction)pyTaskModule,
    METH_NOARGS,
    "Retrieve the bound module"
};

/// Task::waitTaskDone() python wrapper
extern "C" PyObject* pyTaskWaitDone(TaskMembers *self);

static PyMethodDef pyMethodTaskWaitDone =
{
    "wait",
    (PyCFunction)pyTaskWaitDone,
    METH_NOARGS,
    "Wait until the task is done. "
};

///// Task::cancel() python wrapper
//extern "C" PyObject* pyTaskCancel(TaskMembers *self);
//
//static PyMethodDef pyMethodTaskCancel =
//{
//    "cancel",
//    (PyCFunction)pyTaskCancel,
//    METH_NOARGS,
//    "Cancel the task"
//};

/// Task::getState() and Task::getRunningState python wrapper
extern "C" PyObject* pyTaskState(TaskMembers *self);

static PyMethodDef pyMethodTaskState =
{
    "state",
    (PyCFunction)pyTaskState,
    METH_NOARGS,
    "Retrieve the task state"
};

/// exported methods
static PyMethodDef pyTaskMethods[] = {
	pyMethodTaskModule,
	pyMethodTaskWaitDone,
	pyMethodTaskState,

	{NULL} // sentinel
};

// -----------------------------------------------------------------------
// General
// -----------------------------------------------------------------------

/**
 * Deallocator
 */
extern "C" void pyTaskDealloc(TaskMembers* self);

/**
 * Allocator
 *
 * Initialize "name", "internalName" and "description" to empty strings instead of
 * NULL because they are python objects.
 */
extern "C" PyObject* pyTaskNew(PyTypeObject* type, PyObject* args, PyObject* kwds);


/// Definition of the PyTypeObject
static PyTypeObject PythonTask = {             // SPECIFIC
    PyObject_HEAD_INIT(NULL)
    0,                          /*ob_size*/
    "instru.Task",            /*tp_name*/             // SPECIFIC
    sizeof(TaskMembers),         /*tp_basicsize*/  // SPECIFIC
    0,                          /*tp_itemsize*/
    (destructor)pyTaskDealloc,   /*tp_dealloc*/        // SPECIFIC
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
    "Task that controls the execution "
    "of a module",              /* tp_doc */        // SPECIFIC
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    0,                          /* tp_richcompare */
    0,                          /* tp_weaklistoffset */
    0,                          /* tp_iter */
    0,                          /* tp_iternext */
    pyTaskMethods,               /* tp_methods */    // SPECIFIC
    pyTaskMembers,               /* tp_members */    // SPECIFIC
    0,                          /* tp_getset */
    0,                          /* tp_base */       // SPECIFIC
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
    (initproc)pyTaskInit,        /* tp_init */       // SPECIFIC
    0,                          /* tp_alloc */
    pyTaskNew,                   /* tp_new */        // SPECIFIC
};

#endif /* HAVE_PYTHON27 */
#endif /* SRC_PYTHONTASK_H_ */
