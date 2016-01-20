/**
 * @file	src/PythonModuleFactory.cpp
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

#include "ModuleManager.h"
#include "PythonModuleFactory.h"

extern "C" void pyModFactDealloc(ModFactMembers* self)
{
    // "name" and "description" are python objects,
    // so we decrement the references on them,
    // instead of deleting it directly.
    Py_XDECREF(self->name);
    Py_XDECREF(self->description);
    delete self->moduleFactory;

    self->ob_type->tp_free((PyObject*) self); // free the object’s memory
}

extern "C" PyObject* pyModFactAlloc(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    ModFactMembers* self;

    self = (ModFactMembers*) type->tp_alloc(type, 0);
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

        self->moduleFactory = new Poco::SharedPtr<ModuleFactory*>;
      }

    return (PyObject *) self;
}

extern "C" int pyModFactInit(ModFactMembers* self, PyObject *args, PyObject *kwds)
{
    PyObject* tmp=NULL;

    char* charName;
    if (!PyArg_ParseTuple(args, "s:__init__", &charName))
    {
        return -1;
    }

    std::string name(charName);

    // retrieve module factory
    try
    {
        *self->moduleFactory =
                Poco::Util::Application::instance().getSubsystem<ModuleManager>().getRootFactory(name);
    }
    catch (ModuleFactoryException& e)
    {
        std::string errMsg = "Not able to initialize the factory: "
                + e.displayText();
        PyErr_SetString(PyExc_RuntimeError, errMsg.c_str());
        return -1;
    }

    // retrieve name and description
    tmp = self->name;
    self->name = PyString_FromString(charName);
    Py_XDECREF(tmp);

    tmp = self->description;
    self->description = PyString_FromString((**self->moduleFactory)->description());
    Py_XDECREF(tmp);

    return 0;
}

PyObject* pyModFactSelectDescription(ModFactMembers* self)
{
    try
    {
        return PyString_FromString((**self->moduleFactory)->selectDescription());
    }
    catch (ModuleFactoryException& e)
    {
        PyErr_SetString(PyExc_RuntimeError,
                e.displayText().c_str());
        return NULL;
    }
}

PyObject* pyModFactSelectValueList(ModFactMembers* self)
{
    std::vector<std::string> strValues;

    try
    {
        strValues = (**self->moduleFactory)->selectValueList();
    }
    catch (ModuleFactoryException& e)
    {
        PyErr_SetString(PyExc_RuntimeError,
                e.displayText().c_str());
        return NULL;
    }

    // construct the list to return
    PyObject* pyValues = PyList_New(0);

    for (std::vector<std::string>::iterator it=strValues.begin(), ite=strValues.end();
            it != ite; it++ )
    {
        // create the dict entry
        if (0 > PyList_Append(
                pyValues,
                PyString_FromString(it->c_str())))
        {
            // appending the item failed
            PyErr_SetString(PyExc_RuntimeError,
                    "Not able to build the return list");
            return NULL;
        }
    }

    return pyValues;
}

#endif /* HAVE_PYTHON27 */