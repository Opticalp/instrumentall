/**
 * @file	src/PythonModule.cpp
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

#ifdef HAVE_PYTHON27

#include "Module.h"
#include "ModuleManager.h"
#include "PythonModuleFactory.h"
#include "PythonModule.h"

extern "C" void pyModDealloc(ModMembers* self)
{
    // "name" and "description" are python objects,
    // so we decrement the references on them,
    // instead of deleting it directly.
    Py_XDECREF(self->name);
    Py_XDECREF(self->internalName);
    Py_XDECREF(self->description);
    delete self->module;

    self->ob_type->tp_free((PyObject*) self); // free the object’s memory
}

extern "C" PyObject* pyModNew(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    ModMembers* self;

    self = (ModMembers*) type->tp_alloc(type, 0);
    if (self != NULL)
      {
        self->name = PyString_FromString("");
        if (self->name == NULL)
          {
            Py_DECREF(self);
            return NULL;
          }

        self->internalName = PyString_FromString("");
        if (self->internalName == NULL)
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

        self->module = new Poco::SharedPtr<Module*>;
      }

    return (PyObject *) self;
}

extern "C" int pyModInit(ModMembers* self, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(PyExc_NotImplementedError, "__init__ is not implemented");
    return -1;
}

PyObject* pyModParent(ModMembers* self)
{
    Poco::SharedPtr<ModuleFactory*> factory;

    try
    {
        factory = Poco::Util::Application::instance()
                          .getSubsystem<ModuleManager>()
                          .getFactory( (**self->module)->parent() );
    }
    catch (ModuleFactoryException& e)
    {
        PyErr_SetString(PyExc_RuntimeError,
                e.displayText().c_str());
        return NULL;
    }
    catch (ModuleException& e)
    {
        PyErr_SetString(PyExc_RuntimeError,
                e.displayText().c_str());
        return NULL;
    }

    // alloc
    if (PyType_Ready(&PythonModuleFactory) < 0)
        return NULL;

    ModFactMembers* pyParent =
            (ModFactMembers*)(
                    pyModFactNew((PyTypeObject*)&PythonModuleFactory, NULL, NULL) );

    PyObject* tmp=NULL;

    // init
    // retrieve name and description
    tmp = pyParent->name;
    pyParent->name = PyString_FromString((*factory)->name().c_str());
    Py_XDECREF(tmp);

    tmp = pyParent->description;
    pyParent->description = PyString_FromString((*factory)->description());
    Py_XDECREF(tmp);

    // set ModuleFactory reference
    *(pyParent->moduleFactory) = factory;

    return (PyObject*)(pyParent);
}

#include "Dispatcher.h"
#include "InPort.h"
#include "PythonInPort.h"

PyObject* pyModInPorts(ModMembers* self)
{
    std::vector<InPort*> ports;
    Dispatcher& dispatcher = Poco::Util::Application::instance()
                                        .getSubsystem<Dispatcher>();

    ports = (**self->module)->getInPorts();

    // prepare python list
    PyObject* pyPorts = PyList_New(0);

    // prepare InPort python type
    if (PyType_Ready(&PythonInPort) < 0)
    {
        PyErr_SetString(PyExc_RuntimeError,
                "Not able to create the InPort Type");
        return NULL;
    }

    try
    {
        for (std::vector<InPort*>::iterator it = ports.begin(),
                ite = ports.end(); it != ite; it++ )
        {
            Poco::SharedPtr<InPort*> sharedPort;
            sharedPort = dispatcher.getInPort(*it);

            // create the python object
            InPortMembers* pyPort =
                (InPortMembers*)(pyInPortNew((PyTypeObject*)&PythonInPort, NULL, NULL) );

            PyObject* tmp=NULL;

            // init
            // retrieve name and description
            tmp = pyPort->name;
            pyPort->name = PyString_FromString((*sharedPort)->name().c_str());
            Py_XDECREF(tmp);

            tmp = pyPort->description;
            pyPort->description = PyString_FromString((*sharedPort)->description().c_str());
            Py_XDECREF(tmp);

            // set ModuleFactory reference
            *(pyPort->inPort) = sharedPort;

            // create the dict entry
            if (0 > PyList_Append(
                    pyPorts,
                    (PyObject*) pyPort))
            {
                // appending the item failed
                PyErr_SetString(PyExc_RuntimeError,
                        "Not able to build the return list");
                return NULL;
            }
        }
    }
    catch (DispatcherException& e)
    {
        PyErr_SetString(PyExc_RuntimeError,
                e.displayText().c_str());
        return NULL;
    }

    return pyPorts;
}

#endif /* HAVE_PYTHON27 */
