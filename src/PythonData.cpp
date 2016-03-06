/**
 * @file	src/PythonData.cpp
 * @date	mar. 2016
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

#include "Module.h" // for ModuleException
//#include "ModuleManager.h"
#include "Dispatcher.h"
#include "PythonData.h"
#include "PythonOutPort.h"

#include "Poco/Util/Application.h"

extern "C" void pyDataDealloc(DataMembers* self)
{
    delete self->data;

    self->ob_type->tp_free((PyObject*) self); // free the object’s memory
}

extern "C" PyObject* pyDataNew(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    DataMembers* self;

    self = (DataMembers*) type->tp_alloc(type, 0);
    if (self != NULL)
      {
        self->data = new Poco::SharedPtr<DataItem*>;
      }

    return (PyObject *) self;
}

extern "C" int pyDataInit(DataMembers* self, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(PyExc_NotImplementedError, "__init__ is not implemented");
    return -1;
}

PyObject* pyDataParent(DataMembers* self)
{
    Poco::SharedPtr<OutPort*> outPort;

    try
    {
        OutPort* tmp = (**self->data)->parentPort();

        if (tmp)
            outPort = Poco::Util::Application::instance()
                              .getSubsystem<Dispatcher>()
                              .getOutPort( tmp );
        else
            throw ModuleException("parentPort()",
                    "This data has no parent port");
    }
    catch (ModuleException& e) // from getOutPort
    {
        PyErr_SetString(PyExc_RuntimeError,
                e.displayText().c_str());
        return NULL;
    }

    // alloc
    if (PyType_Ready(&PythonOutPort) < 0)
    {
        PyErr_SetString(PyExc_ImportError,
                "Not able to create the OutPort Type");
        return NULL;
    }

    OutPortMembers* pyParent =
            reinterpret_cast<OutPortMembers*>(
                    pyOutPortNew((PyTypeObject*)&PythonOutPort, NULL, NULL) );

    PyObject* tmp=NULL;

    // init
    // retrieve name, internal name, and description
    tmp = pyParent->name;
    pyParent->name = PyString_FromString((*outPort)->name().c_str());
    Py_XDECREF(tmp);

    tmp = pyParent->description;
    pyParent->description = PyString_FromString((*outPort)->description().c_str());
    Py_XDECREF(tmp);

    // set ModuleFactory reference
    *(pyParent->outPort) = outPort;

    return (PyObject*)(pyParent);
}

#include "Dispatcher.h"
#include "OutPort.h"
#include "PythonOutPort.h"

PyObject* pyDataGetValue(DataMembers* self)
{
    // TODO: other data types

    if ((**self->data)->dataType() == DataItem::typeInteger)
    {
        (**self->data)->readLock();

        int value = *((**self->data)->getDataToRead<int>());

        (**self->data)->releaseData();

        return PyInt_FromLong(value);
    }
    else
    {
        PyErr_SetString(PyExc_NotImplementedError,
                "getValue is not implemented for dataType != integer");
        return NULL;
    }
}

#include "PythonDataLogger.h"
#include "DataManager.h"

extern "C" PyObject* pyDataRegister(DataMembers *self, PyObject* args)
{
    PyObject *pyObj;

    // arguments parsing
    if (!PyArg_ParseTuple(args, "O:register", &pyObj))
        return NULL;

    // check the type of the object.
    // the comparison uses type name (str)
    std::string typeName(pyObj->ob_type->tp_name);

    if (typeName.compare("instru.DataLogger"))
    {
        PyErr_SetString(PyExc_TypeError,
                "The argument must be a DataLogger");
        return NULL;
    }

    DataLoggerMembers* pyLogger = reinterpret_cast<DataLoggerMembers*>(pyObj);

    try
    {
        Poco::Util::Application::instance()
                                .getSubsystem<DataManager>()
                                .registerLogger(*self->data, *pyLogger->logger);
    }
    catch (Poco::NotFoundException& e)
    {
        PyErr_SetString(PyExc_RuntimeError,
                e.displayText().c_str());
        return NULL;
    }
    catch (Poco::InvalidAccessException& e1)
    {
        PyErr_SetString(PyExc_RuntimeError,
                e1.displayText().c_str());
        return NULL;
    }

    return Py_BuildValue("");
}

extern "C" PyObject* pyDataLoggers(DataMembers *self)
{
    std::set< SharedPtr<DataLogger*> > loggers;
    try
    {
        loggers = (**self->data)->loggers();
    }
    catch (Poco::NotFoundException& e)
    {
        PyErr_SetString( PyExc_RuntimeError,
                e.displayText().c_str() );
        return NULL;
    }

    if (loggers.size() == 0)
        return Py_BuildValue("");

    // construct the list to return
    PyObject* pyLoggers = PyList_New(0);

    // prepare DataLogger python type
    if (PyType_Ready(&PythonDataLogger) < 0)
    {
        PyErr_SetString(PyExc_ImportError,
                "Not able to create the DataLogger Type");
        return NULL;
    }

    // to retrieve the logger description
    std::map<std::string, std::string> classes;
    classes = Poco::Util::Application::instance()
                        .getSubsystem<DataManager>()
                        .dataLoggerClasses();

    for (std::set< SharedPtr<DataLogger*> >::iterator it = loggers.begin(),
            ite = loggers.end(); it != ite; it++ )
    {
        // create the python object
        DataLoggerMembers* pyLogger =
            reinterpret_cast<DataLoggerMembers*>(
                pyDataLoggerNew(
                    reinterpret_cast<PyTypeObject*>(&PythonDataLogger), NULL, NULL) );

        PyObject* tmp=NULL;

        // init
        // retrieve name and description
        tmp = pyLogger->name;
        pyLogger->name = PyString_FromString((**it)->name().c_str());
        Py_XDECREF(tmp);

        std::map<std::string, std::string>::iterator loggerClass = classes.find((**it)->name());
        if (loggerClass == classes.end())
        {
            PyErr_SetString(PyExc_RuntimeError, "Logger description not found" );
            return NULL;
        }

        tmp = pyLogger->description;
        pyLogger->description = PyString_FromString(loggerClass->second.c_str());
        Py_XDECREF(tmp);

        // set Logger reference
        *(pyLogger->logger) = *it;

        // create the dict entry
        if (0 > PyList_Append(
                pyLoggers,
                reinterpret_cast<PyObject*>(pyLogger)))
        {
            // appending the item failed
            PyErr_SetString(PyExc_RuntimeError,
                    "Not able to build the return list");
            return NULL;
        }
    }

    return pyLoggers;

}

#endif /* HAVE_PYTHON27 */
