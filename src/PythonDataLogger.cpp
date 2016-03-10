/**
 * @file	src/PythonDataLogger.cpp
 * @date	Mar 2016
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

#include "DataManager.h"
#include "PythonDataLogger.h"
#include "PythonData.h"
#include "Poco/Util/Application.h"

extern "C" void pyDataLoggerDealloc(DataLoggerMembers* self)
{
    // "name" and "description" are python objects,
    // so we decrement the references on them,
    // instead of deleting it directly.
    Py_XDECREF(self->name);
    Py_XDECREF(self->description);
    delete self->logger;

    self->ob_type->tp_free((PyObject*) self); // free the object’s memory
}

extern "C" PyObject* pyDataLoggerNew(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    DataLoggerMembers* self;

    self = (DataLoggerMembers*) type->tp_alloc(type, 0);
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

        self->logger = new Poco::SharedPtr<DataLogger*>;
      }

    return (PyObject *) self;
}

extern "C" int pyDataLoggerInit(DataLoggerMembers* self, PyObject *args, PyObject *kwds)
{
    PyObject* tmp=NULL;

    char* charClassName;
    if (!PyArg_ParseTuple(args, "s:__init__", &charClassName))
    {
        return -1;
    }

    std::string className(charClassName);

    SharedPtr<DataLogger*> newLogger;

    try
    {
        newLogger = Poco::Util::Application::instance()
                              .getSubsystem<DataManager>()
                              .newDataLogger(className);
    }
    catch (Poco::NotFoundException& e)
    {
        PyErr_SetString(PyExc_NameError, e.displayText().c_str());
        return -1;
    }

    *self->logger = newLogger;

    // retrieve name
    tmp = self->name;
    self->name = PyString_FromString((**self->logger)->name().c_str());
    Py_XDECREF(tmp);


    // retrieve description
    std::map<std::string, std::string> classes;
    classes = Poco::Util::Application::instance()
                        .getSubsystem<DataManager>()
                        .dataLoggerClasses();

    std::map<std::string, std::string>::iterator it = classes.find(className);
    if (it == classes.end())
    {
        PyErr_SetString(PyExc_RuntimeError, "Class description not found" );
        return -1;
    }

    tmp = self->description;
    self->description = PyString_FromString(it->second.c_str());
    Py_XDECREF(tmp);

    return 0;
}

PyObject* pyDataLoggerSource(DataLoggerMembers* self)
{
    Poco::SharedPtr<DataItem*> data;

    try
    {
        data = Poco::Util::Application::instance()
                          .getSubsystem<DataManager>()
                          .getSourceDataItem( *self->logger );
    }
    catch (Poco::NotFoundException& e)
    {
        PyErr_SetString(PyExc_RuntimeError,
                e.displayText().c_str());
        return NULL;
    }

    // alloc
    if (PyType_Ready(&PythonData) < 0)
    {
        PyErr_SetString(PyExc_ImportError,
                "Not able to create the Data Type");
        return NULL;
    }

    DataMembers* pyData =
            (DataMembers*)(
                    pyDataNew((PyTypeObject*)&PythonData, NULL, NULL) );

    PyObject* tmp=NULL;

    // init
    // set Data reference
    *(pyData->data) = data;

    return (PyObject*)(pyData);
}

PyObject* pyDataLoggerDetach(DataLoggerMembers* self)
{
    (**self->logger)->detach();
    return Py_BuildValue("");
}

#endif /* HAVE_PYTHON27 */
