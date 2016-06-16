/**
 * @file	src/PythonOutPort.cpp
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

#include "OutPort.h"
#include "Module.h"

#include "ModuleManager.h"
#include "DataManager.h"
#include "Dispatcher.h"

#include "PythonOutPort.h"
#include "PythonModule.h"
#include "PythonDataLogger.h"

extern "C" void pyOutPortDealloc(OutPortMembers* self)
{
    // "name" and "description" are python objects,
    // so we decrement the references on them,
    // instead of deleting it directly.
    Py_XDECREF(self->name);
    Py_XDECREF(self->description);
    delete self->outPort;

    self->ob_type->tp_free((PyObject*) self); // free the object’s memory
}

extern "C" PyObject* pyOutPortNew(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    OutPortMembers* self;

    self = (OutPortMembers*) type->tp_alloc(type, 0);
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

        self->outPort = new Poco::SharedPtr<OutPort*>;
      }

    return (PyObject *) self;
}

extern "C" int pyOutPortInit(OutPortMembers* self, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(PyExc_NotImplementedError, "__init__ is not implemented");
    return -1;
}

PyObject* pyOutPortParent(OutPortMembers* self)
{
    Poco::SharedPtr<Module*> module;

    try
    {
        module = Poco::Util::Application::instance()
                          .getSubsystem<ModuleManager>()
                          .getModule( (**self->outPort)->parent() );
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
#include "InDataPort.h"
#include "InPort.h"
#include "PythonInPort.h"

PyObject* pyOutPortGetTargetPorts(OutPortMembers* self)
{
    std::vector< Poco::SharedPtr<InPort*> > targets;

    targets = (**self->outPort)->getTargetPorts();

    // prepare python list
    PyObject* pyPorts = PyList_New(0);

    // prepare InPort python type
    if (PyType_Ready(&PythonInPort) < 0)
    {
        PyErr_SetString(PyExc_ImportError,
                "Not able to create the InPort Type");
        return NULL;
    }


    for ( std::vector< Poco::SharedPtr<InPort*> >::iterator it = targets.begin(),
            ite = targets.end(); it != ite; it++ )
    {
        // create the python object
        InPortMembers* pyPort =
            (InPortMembers*)(pyInPortNew((PyTypeObject*)&PythonInPort, NULL, NULL) );

        PyObject* tmp=NULL;

        // init
        // retrieve name and description
        tmp = pyPort->name;
        pyPort->name = PyString_FromString((**it)->name().c_str());
        Py_XDECREF(tmp);

        tmp = pyPort->description;
        pyPort->description = PyString_FromString((**it)->description().c_str());
        Py_XDECREF(tmp);

        // set ModuleFactory reference
        *(pyPort->inPort) = *it;

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

    return pyPorts;
}

PyObject* pyOutPortGetSeqTargetPorts(OutPortMembers* self)
{
    std::vector< Poco::SharedPtr<InPort*> > targets;

    targets = (**self->outPort)->getSeqTargetPorts();

    // prepare python list
    PyObject* pyPorts = PyList_New(0);

    // prepare InPort python type
    if (PyType_Ready(&PythonInPort) < 0)
    {
        PyErr_SetString(PyExc_ImportError,
                "Not able to create the InPort Type");
        return NULL;
    }


    for ( std::vector< Poco::SharedPtr<InPort*> >::iterator it = targets.begin(),
            ite = targets.end(); it != ite; it++ )
    {
        // create the python object
        InPortMembers* pyPort =
            (InPortMembers*)(pyInPortNew((PyTypeObject*)&PythonInPort, NULL, NULL) );

        PyObject* tmp=NULL;

        // init
        // retrieve name and description
        tmp = pyPort->name;
        pyPort->name = PyString_FromString((**it)->name().c_str());
        Py_XDECREF(tmp);

        tmp = pyPort->description;
        pyPort->description = PyString_FromString((**it)->description().c_str());
        Py_XDECREF(tmp);

        // set ModuleFactory reference
        *(pyPort->inPort) = *it;

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

    return pyPorts;
}

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

PyObject* pyOutPortGetDataValue(OutPortMembers* self)
{
	SharedPtr<DataItem*> sharedData = Poco::Util::Application::instance()
			                          .getSubsystem<DataManager>()
									  .getDataItem((**self->outPort)->dataItem());

    int dataType = (*sharedData)->dataType();

    if (DataItem::isVector(dataType))
        return getVectorValue(sharedData);

    // python object to return
    PyObject* pyObj;

    (*sharedData)->readLock();

    switch (DataItem::noContainerDataType(dataType))
    {
    case DataItem::typeInt32:
    {
        long data = *((*sharedData)->getDataToRead<Poco::Int32>());
        pyObj = PyInt_FromLong(data);
        break;
    }
    case DataItem::typeUInt32:
    {
        size_t data = *((*sharedData)->getDataToRead<Poco::UInt32>());
        pyObj = PyInt_FromSize_t(data);
        break;
    }
    case DataItem::typeInt64:
    {
        long data = static_cast<long>(*((*sharedData)->getDataToRead<Poco::Int64>()));
        pyObj = PyInt_FromLong(data);
        break;
    }
    case DataItem::typeUInt64:
    {
        size_t data = *((*sharedData)->getDataToRead<Poco::UInt64>());
        pyObj = PyInt_FromSize_t(data);
        break;
    }
    case DataItem::typeFloat:
    {
        double data = *((*sharedData)->getDataToRead<float>());
        pyObj = PyFloat_FromDouble(data);
        break;
    }
    case DataItem::typeDblFloat:
    {
        double data = *((*sharedData)->getDataToRead<double>());
        pyObj = PyFloat_FromDouble(data);
        break;
    }
    case DataItem::typeString:
    {
        std::string* pData = (*sharedData)->getDataToRead<std::string>();
        pyObj = PyString_FromString(pData->c_str());
        break;
    }
    default:
        PyErr_SetString(PyExc_NotImplementedError,
                "getValue is not implemented for this dataType");
        (*sharedData)->releaseData();
        return NULL;
    }

    (*sharedData)->releaseData();

    return pyObj;
}

PyObject* pyOutPortRegister(OutPortMembers *self, PyObject* args)
{
	SharedPtr<OutPort*> sharedPort =  Poco::Util::Application::instance()
    								  .getSubsystem<Dispatcher>()
									  .getOutPort(**self->outPort);

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
                                .registerLogger(sharedPort, *pyLogger->logger);
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

    Py_INCREF(Py_None);
    return Py_None;
}

PyObject* pyOutPortLoggers(OutPortMembers *self)
{
    std::set< SharedPtr<DataLogger*> > loggers;
    try
    {
        loggers = (**self->outPort)->loggers();
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
