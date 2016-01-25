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
                    pyModFactAlloc((PyTypeObject*)&PythonModuleFactory, NULL, NULL) );

    PyObject* tmp=NULL;

    // init
    // retrieve name and description
    tmp = pyParent->name;
    pyParent->name = PyString_FromString((*factory)->name());
    Py_XDECREF(tmp);

    tmp = pyParent->description;
    pyParent->description = PyString_FromString((*factory)->description());
    Py_XDECREF(tmp);

    // set ModuleFactory reference
    *(pyParent->moduleFactory) = factory;

    return (PyObject*)(pyParent);
}

#endif /* HAVE_PYTHON27 */
