/**
 * @file	src/PythonInPort.cpp
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

#ifdef HAVE_PYTHON27

#include "InDataPort.h"
#include "Module.h"
#include "ModuleManager.h"
#include "PythonInPort.h"
#include "PythonModule.h"

extern "C" void pyInPortDealloc(InPortMembers* self)
{
    // "name" and "description" are python objects,
    // so we decrement the references on them,
    // instead of deleting it directly.
    Py_XDECREF(self->name);
    Py_XDECREF(self->description);
    delete self->inPort;

    self->ob_type->tp_free((PyObject*) self); // free the object’s memory
}

extern "C" PyObject* pyInPortNew(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    InPortMembers* self;

    self = (InPortMembers*) type->tp_alloc(type, 0);
    if (self != NULL)
      {
        self->name = PyString_FromString("");
        if (self->name == NULL)
          {
            Py_DECREF(self);
            return NULL;
          }

        self->description = PyString_FromString("");
        if (self->description == NULL)
          {
            Py_DECREF(self);
            return NULL;
          }

        self->inPort = new Poco::SharedPtr<InPort*>;
      }

    return (PyObject *) self;
}

extern "C" int pyInPortInit(InPortMembers* self, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(PyExc_NotImplementedError, "__init__ is not implemented");
    return -1;
}

PyObject* pyInPortParent(InPortMembers* self)
{
    Poco::SharedPtr<Module*> module;

    try
    {
        module = Poco::Util::Application::instance()
                          .getSubsystem<ModuleManager>()
                          .getModule( (**self->inPort)->parent() );
    }
    catch (Poco::Exception& e) // from getModule
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

    ModMembers* pyParent =
            (ModMembers*)(
                    pyModNew((PyTypeObject*)&PythonModule, NULL, NULL) );

    PyObject* tmp=NULL;

    // init
    // retrieve name, internal name, and description
    tmp = pyParent->name;
    pyParent->name = PyString_FromString((*module)->name().c_str());
    Py_XDECREF(tmp);

    tmp = pyParent->internalName;
    pyParent->internalName = PyString_FromString((*module)->internalName().c_str());
    Py_XDECREF(tmp);

    tmp = pyParent->description;
    pyParent->description = PyString_FromString((*module)->description().c_str());
    Py_XDECREF(tmp);

    // set ModuleFactory reference
    *(pyParent->module) = module;

    return (PyObject*)(pyParent);
}

#include "Dispatcher.h"
#include "OutPort.h"
#include "PythonOutPort.h"

PyObject* pyInPortGetSourcePort(InPortMembers* self)
{
    Poco::SharedPtr<OutPort*> sharedSource;

    sharedSource = (**self->inPort)->getSourcePort();

    // check if connected
    if ( *sharedSource == Poco::Util::Application::instance()
                            .getSubsystem<Dispatcher>()
                            .getEmptyOutPort() )
    {
        PyErr_SetString(PyExc_ReferenceError,
                "This input port has no source");
        return NULL;
    }

    // prepare OutPort python type
    if (PyType_Ready(&PythonOutPort) < 0)
    {
        PyErr_SetString(PyExc_ImportError,
                "Not able to create the OutPort Type");
        return NULL;
    }

    // create the python object
    OutPortMembers* pyPort =
        (OutPortMembers*)(pyOutPortNew((PyTypeObject*)&PythonOutPort, NULL, NULL) );

    PyObject* tmp=NULL;

    // init
    // retrieve name and description
    tmp = pyPort->name;
    pyPort->name = PyString_FromString((*sharedSource)->name().c_str());
    Py_XDECREF(tmp);

    tmp = pyPort->description;
    pyPort->description = PyString_FromString((*sharedSource)->description().c_str());
    Py_XDECREF(tmp);

    // set ModuleFactory reference
    *(pyPort->outPort) = sharedSource;

    return (PyObject*) pyPort;
}

PyObject* pyInPortGetSeqSourcePort(InPortMembers* self)
{
    if ((**self->inPort)->isTrig())
        Py_RETURN_NONE;

    Poco::SharedPtr<OutPort*> sharedSource;

    sharedSource = reinterpret_cast<InDataPort*>(**self->inPort)->getSeqSourcePort();

    // check if connected
    if ( *sharedSource == Poco::Util::Application::instance()
                            .getSubsystem<Dispatcher>()
                            .getEmptyOutPort() )
    {
        PyErr_SetString(PyExc_ReferenceError,
                "This input port has no source");
        return NULL;
    }

    // prepare OutPort python type
    if (PyType_Ready(&PythonOutPort) < 0)
    {
        PyErr_SetString(PyExc_ImportError,
                "Not able to create the OutPort Type");
        return NULL;
    }

    // create the python object
    OutPortMembers* pyPort =
        (OutPortMembers*)(pyOutPortNew((PyTypeObject*)&PythonOutPort, NULL, NULL) );

    PyObject* tmp=NULL;

    // init
    // retrieve name and description
    tmp = pyPort->name;
    pyPort->name = PyString_FromString((*sharedSource)->name().c_str());
    Py_XDECREF(tmp);

    tmp = pyPort->description;
    pyPort->description = PyString_FromString((*sharedSource)->description().c_str());
    Py_XDECREF(tmp);

    // set ModuleFactory reference
    *(pyPort->outPort) = sharedSource;

    return (PyObject*) pyPort;
}

PyObject* pyInPortHoldData(InPortMembers *self, PyObject* args)
{
    if ((**self->inPort)->isTrig())
    {
        PyErr_SetString(PyExc_RuntimeError,
                "This port is a trig port. Data can not be hold");
        return NULL;
    }

    char* commandChar;

    if (!PyArg_ParseTuple(args, "s:holdData", &commandChar))
        return NULL;

    std::string command(commandChar);

    if (command.compare("on") == 0)
        reinterpret_cast<InDataPort*>(**self->inPort)->hold(true);
    else if (command.compare("off") == 0)
        reinterpret_cast<InDataPort*>(**self->inPort)->hold(false);
    else
    {
        PyErr_SetString(PyExc_RuntimeError,
                "Please, pass \"on\" or \"off\" as function argument");
        return NULL;
    }

    Py_RETURN_NONE;
}

PyObject* pyInPortIsTrig(InPortMembers *self)
{
    if ((**self->inPort)->isTrig())
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

#endif /* HAVE_PYTHON27 */
