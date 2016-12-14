/**
 * @file	src/UI/python/PythonTask.cpp
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

#ifdef HAVE_PYTHON27

#include "core/Module.h"
#include "core/ModuleManager.h"
#include "PythonModuleFactory.h"
#include "PythonModule.h"
#include "PythonTask.h"

extern "C" void pyTaskDealloc(TaskMembers* self)
{
    // "name" is a python object,
    // so we decrement the reference on it,
    // instead of deleting it directly.
    Py_XDECREF(self->name);
    delete self->task;

    self->ob_type->tp_free((PyObject*) self); // free the object’s memory
}

extern "C" PyObject* pyTaskNew(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    TaskMembers* self;

    self = (TaskMembers*) type->tp_alloc(type, 0);
    if (self != NULL)
      {
        self->name = PyString_FromString("");
        if (self->name == NULL)
          {
            Py_DECREF(self);
            return NULL;
          }

        self->task = new Poco::AutoPtr<ModuleTask>;
      }

    return (PyObject *) self;
}

extern "C" int pyTaskInit(TaskMembers* self, PyObject *args, PyObject *kwds)
{
    PyObject* tmp=NULL;

    char* charName;
    if (!PyArg_ParseTuple(args, "s:__init__", &charName))
    {
        return -1;
    }

    std::string name(charName);

    // retrieve task
    try
    {
        //*self->task = ;

    	Poco::NotImplementedException("taskInit(str) is not implemented");

    }
    catch (Poco::Exception& e)
    {
        std::string errMsg = "Not able to initialize the task: "
                + e.displayText();
        PyErr_SetString(PyExc_RuntimeError, errMsg.c_str());
        return -1;
    }

    // retrieve name and description
    tmp = self->name;
    self->name = PyString_FromString((*self->task)->name().c_str());
    Py_XDECREF(tmp);

    return 0;
}

PyObject* pyTaskModule(TaskMembers* self)
{
    Poco::SharedPtr<Module*> module;

    try
    {
        module = Poco::Util::Application::instance()
                          .getSubsystem<ModuleManager>()
                          .getModule( (*self->task)->module() );
    }
    catch (Poco::Exception& e)
    {
        PyErr_SetString(PyExc_RuntimeError,
                e.displayText().c_str());
        return NULL;
    }

    // alloc
    if (PyType_Ready(&PythonModule) < 0)
    {
        PyErr_SetString(PyExc_ImportError,
                "Not able to create the Module Type");
        return NULL;
    }

    ModMembers* pyModule =
            (ModMembers*)(pyModNew((PyTypeObject*)&PythonModule, NULL, NULL) );

    PyObject* tmp=NULL;

    // init
    // retrieve name and description
    tmp = pyModule->name;
    pyModule->name = PyString_FromString((*module)->name().c_str());
    Py_XDECREF(tmp);

    tmp = pyModule->internalName;
    pyModule->internalName = PyString_FromString((*module)->internalName().c_str());
    Py_XDECREF(tmp);

    tmp = pyModule->description;
    pyModule->description = PyString_FromString((*module)->description().c_str());
    Py_XDECREF(tmp);

    // set Module reference
    *(pyModule->module) = module;

    return (PyObject*)(pyModule);
}

PyObject* pyTaskWaitDone(TaskMembers* self)
{
    try
    {
        (*self->task)->waitTaskDone();
    }
    catch (Poco::Exception& e)
    {
        PyErr_SetString(PyExc_RuntimeError,
                e.displayText().c_str());
        return NULL;
    }

    Py_RETURN_NONE;
}

//PyObject* pyTaskCancel(TaskMembers* self)
//{
//    try
//    {
//        (*self->task)->cancel();
//    }
//    catch (Poco::Exception& e)
//    {
//        PyErr_SetString(PyExc_RuntimeError,
//                e.displayText().c_str());
//        return NULL;
//    }
//
//    Py_RETURN_NONE;
//}

PyObject* pyTaskState(TaskMembers* self)
{
	std::string state;
    try
    {
        switch ((*self->task)->getState())
        {
        case MergeableTask::TASK_IDLE:
        	state = "idle";
        	break;
        case MergeableTask::TASK_FALSE_START:
        	state = "false start";
        	break;
        case MergeableTask::TASK_STARTING:
        	state = "starting";
        	break;
        case MergeableTask::TASK_CANCELLING:
        	state = "canceling";
        	break;
        case MergeableTask::TASK_FINISHED:
        	state = "finished";
        	break;
        case MergeableTask::TASK_RUNNING:
        	switch ((*self->task)->getRunningState())
        	{
        	case ModuleTask::applyingParameters:
        		state = "retrieving and applying parameters";
        		break;
        	case ModuleTask::retrievingInDataLocks:
        		state = "retrieving input locks";
        		break;
        	case ModuleTask::retrievingOutDataLocks:
        		state = "retrieving output locks";
        		break;
        	case ModuleTask::processing:
        		state = "processing";
        		break;
        	case ModuleTask::NotAvailableRunningState:
        		state = "not available";
        		PyErr_WarnEx(PyExc_RuntimeWarning,
        				"The task state changed during the query", 0);
        		break;
        	default:
            	poco_bugcheck_msg("unknown task running state");
            	throw Poco::BugcheckException();
        	}
        	break;
        default:
        	poco_bugcheck_msg("unknown task state");
        	throw Poco::BugcheckException();
        }
    }
    catch (Poco::Exception& e)
    {
        PyErr_SetString(PyExc_RuntimeError,
                e.displayText().c_str());
        return NULL;
    }

    return PyString_FromString(state.c_str());
}

#endif /* HAVE_PYTHON27 */
