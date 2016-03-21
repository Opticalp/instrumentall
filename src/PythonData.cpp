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

/**
 * Construct a python list from a vector
 */
PyObject* getVectorValue(SharedPtr<DataItem*> data)
{
    // construct pyList
    PyObject* list = PyList_New(0);

    (*data)->readLock();

    switch (DataItem::noContainerDataType((*data)->dataType()))
    {
    case DataItem::typeInt32:
    {
        std::vector<Poco::Int32>* pData;
        pData = (*data)->getDataToRead< std::vector<Poco::Int32> >();

        for (std::vector<Poco::Int32>::iterator it = pData->begin(),
                ite = pData->end(); it != ite; it++)
        {
            // create the dict entry
            if ( 0 > PyList_Append(list, PyInt_FromLong(*it)) )
            {
                // appending the item failed
                PyErr_SetString(PyExc_RuntimeError,
                        "Not able to build the return list");
                (*data)->releaseData();
                return NULL;
            }
        }
        break;
    }
    case DataItem::typeUInt32:
    {
        std::vector<Poco::UInt32>* pData;
        pData = (*data)->getDataToRead< std::vector<Poco::UInt32> >();

        for (std::vector<Poco::UInt32>::iterator it = pData->begin(),
                ite = pData->end(); it != ite; it++)
        {
            // create the dict entry
            if ( 0 > PyList_Append(list, PyInt_FromSize_t(*it)) )
            {
                // appending the item failed
                PyErr_SetString(PyExc_RuntimeError,
                        "Not able to build the return list");
                (*data)->releaseData();
                return NULL;
            }
        }
        break;
    }
    case DataItem::typeInt64:
    {
        std::vector<Poco::Int64>* pData;
        pData = (*data)->getDataToRead< std::vector<Poco::Int64> >();

        for (std::vector<Poco::Int64>::iterator it = pData->begin(),
                ite = pData->end(); it != ite; it++)
        {
            // create the dict entry
            if ( 0 > PyList_Append(list, PyInt_FromLong(static_cast<long>(*it))) )
            {
                // appending the item failed
                PyErr_SetString(PyExc_RuntimeError,
                        "Not able to build the return list");
                (*data)->releaseData();
                return NULL;
            }
        }
        break;
    }
    case DataItem::typeUInt64:
    {
        std::vector<Poco::UInt64>* pData;
        pData = (*data)->getDataToRead< std::vector<Poco::UInt64> >();

        for (std::vector<Poco::UInt64>::iterator it = pData->begin(),
                ite = pData->end(); it != ite; it++)
        {
            // create the dict entry
            if ( 0 > PyList_Append(list, PyInt_FromSize_t(*it)) )
            {
                // appending the item failed
                PyErr_SetString(PyExc_RuntimeError,
                        "Not able to build the return list");
                (*data)->releaseData();
                return NULL;
            }
        }
        break;
    }
    case DataItem::typeFloat:
    {
        std::vector<float>* pData;
        pData = (*data)->getDataToRead< std::vector<float> >();

        for (std::vector<float>::iterator it = pData->begin(),
                ite = pData->end(); it != ite; it++)
        {
            // create the dict entry
            if ( 0 > PyList_Append(list, PyFloat_FromDouble(*it)) )
            {
                // appending the item failed
                PyErr_SetString(PyExc_RuntimeError,
                        "Not able to build the return list");
                (*data)->releaseData();
                return NULL;
            }
        }
        break;
    }
    case DataItem::typeDblFloat:
    {
        std::vector<double>* pData;
        pData = (*data)->getDataToRead< std::vector<double> >();

        for (std::vector<double>::iterator it = pData->begin(),
                ite = pData->end(); it != ite; it++)
        {
            // create the dict entry
            if ( 0 > PyList_Append(list, PyFloat_FromDouble(*it)) )
            {
                // appending the item failed
                PyErr_SetString(PyExc_RuntimeError,
                        "Not able to build the return list");
                (*data)->releaseData();
                return NULL;
            }
        }
        break;
    }
    case DataItem::typeString:
    {
        std::vector<std::string>* pData;
        pData = (*data)->getDataToRead< std::vector<std::string> >();

        for (std::vector<std::string>::iterator it = pData->begin(),
                ite = pData->end(); it != ite; it++)
        {
            // create the dict entry
            if ( 0 > PyList_Append(list, PyString_FromString(it->c_str())) )
            {
                // appending the item failed
                PyErr_SetString(PyExc_RuntimeError,
                        "Not able to build the return list");
                (*data)->releaseData();
                return NULL;
            }
        }
        break;
    }
    default:
        PyErr_SetString(PyExc_NotImplementedError,
                "getValue is not implemented for this dataType");
        (*data)->releaseData();
        return NULL;
    }

    (*data)->releaseData();

    return list;
}

PyObject* pyDataGetValue(DataMembers* self)
{
    int dataType = (**self->data)->dataType();

    if (DataItem::isVector(dataType))
        return getVectorValue(*self->data);

    // python object to return
    PyObject* pyObj;

    (**self->data)->readLock();

    switch (DataItem::noContainerDataType(dataType))
    {
    case DataItem::typeInt32:
    {
        long data = *((**self->data)->getDataToRead<Poco::Int32>());
        pyObj = PyInt_FromLong(data);
        break;
    }
    case DataItem::typeUInt32:
    {
        size_t data = *((**self->data)->getDataToRead<Poco::UInt32>());
        pyObj = PyInt_FromSize_t(data);
        break;
    }
    case DataItem::typeInt64:
    {
        long data = static_cast<long>(*((**self->data)->getDataToRead<Poco::Int64>()));
        pyObj = PyInt_FromLong(data);
        break;
    }
    case DataItem::typeUInt64:
    {
        size_t data = *((**self->data)->getDataToRead<Poco::UInt64>());
        pyObj = PyInt_FromSize_t(data);
        break;
    }
    case DataItem::typeFloat:
    {
        double data = *((**self->data)->getDataToRead<float>());
        pyObj = PyFloat_FromDouble(data);
        break;
    }
    case DataItem::typeDblFloat:
    {
        double data = *((**self->data)->getDataToRead<double>());
        pyObj = PyFloat_FromDouble(data);
        break;
    }
    case DataItem::typeString:
    {
        std::string* pData = (**self->data)->getDataToRead<std::string>();
        pyObj = PyString_FromString(pData->c_str());
        break;
    }
    default:
        PyErr_SetString(PyExc_NotImplementedError,
                "getValue is not implemented for this dataType");
        (**self->data)->releaseData();
        return NULL;
    }

    (**self->data)->releaseData();

    return pyObj;
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
