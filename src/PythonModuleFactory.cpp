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
#include "PythonModule.h"

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

PyObject* pyModFactSelect(ModFactMembers* self, PyObject* args)
{
    char *charParamName;

    if (!PyArg_ParseTuple(args, "s:getParameter", &charParamName))
        return NULL;

    std::string paramName = charParamName;
    Poco::SharedPtr<ModuleFactory*> childFact;

    try
    {
        childFact = Poco::Util::Application::instance()
                      .getSubsystem<ModuleManager>()
                      .getFactory(
                              reinterpret_cast<ModuleFactory*>(
                                      &(**self->moduleFactory)->select(paramName)) );
    }
    catch (ModuleFactoryException& e)
    {
        PyErr_SetString(PyExc_RuntimeError,
                e.displayText().c_str());
        return NULL;
    }

    // construct the new python factory

    // alloc
    if (PyType_Ready(&PythonModuleFactory) < 0)
        return NULL;

    ModFactMembers* pyChildFact =
            (ModFactMembers*)(
                    pyModFactAlloc((PyTypeObject*)&PythonModuleFactory, NULL, NULL) );

    PyObject* tmp=NULL;

    // init
    // retrieve name and description
    tmp = pyChildFact->name;
    pyChildFact->name = PyString_FromString((*childFact)->name());
    Py_XDECREF(tmp);

    tmp = pyChildFact->description;
    pyChildFact->description = PyString_FromString((*childFact)->description());
    Py_XDECREF(tmp);

    // set ModuleFactory reference
    *(pyChildFact->moduleFactory) = childFact;

    return (PyObject*)(pyChildFact);
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

PyObject* pyModFactCountRemain(ModFactMembers* self)
{
    size_t cnt;

    try
    {
        cnt = (**self->moduleFactory)->countRemain();
    }
    catch (ModuleFactoryException& e)
    {
        PyErr_SetString(PyExc_RuntimeError,
                e.displayText().c_str());
        return NULL;
    }

    return PyInt_FromSize_t(cnt);
}

PyObject* pyModFactCreate(ModFactMembers* self, PyObject *args)
{
    char empty[] = "";
    char *charParamName = empty;
    std::string paramName;

    if (!PyArg_ParseTuple(args, "|s:create", &charParamName))
        return NULL;

    paramName = charParamName;

    Poco::SharedPtr<Module*> module;

    try
    {
        module = Poco::Util::Application::instance()
                      .getSubsystem<ModuleManager>()
                      .getModule( (**self->moduleFactory)->create(paramName) );
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

    // construct the new python module

    // alloc
    if (PyType_Ready(&PythonModule) < 0)
        return NULL;

    ModMembers* pyMod =
            (ModMembers*)(
                    pyModNew((PyTypeObject*)&PythonModule, NULL, NULL) );

    PyObject* tmp=NULL;

    // init
    // retrieve name and description
    tmp = pyMod->name;
    pyMod->name = PyString_FromString((*module)->name().c_str());
    Py_XDECREF(tmp);

    tmp = pyMod->internalName;
    pyMod->internalName = PyString_FromString((*module)->internalName().c_str());
    Py_XDECREF(tmp);

    tmp = pyMod->description;
    pyMod->description = PyString_FromString((*module)->description());
    Py_XDECREF(tmp);

    // set ModuleFactory reference
    *(pyMod->module) = module;

    return (PyObject*)(pyMod);
}

#endif /* HAVE_PYTHON27 */
