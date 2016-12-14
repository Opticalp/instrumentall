/**
 * @file	src/UI/python/PythonDataSource.cpp
 * @date	jul. 2016
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

#include "PythonDataSource.h"
#include "core/DataSource.h"

extern "C" void pyDataSourceDealloc(DataSourceMembers* self)
{
    // "name" and "description" are python objects,
    // so we decrement the references on them,
    // instead of deleting it directly.
    Py_XDECREF(self->name);
    Py_XDECREF(self->description);

    self->ob_type->tp_free((PyObject*) self); // free the object’s memory
}

extern "C" PyObject* pyDataSourceNew(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    DataSourceMembers* self;

    self = (DataSourceMembers*) type->tp_alloc(type, 0);
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
      }

    return (PyObject *) self;
}

#include "PythonParameterGetter.h"
#include "PythonDataProxy.h"
#include "PythonDataLogger.h"

extern "C" int pyDataSourceInit(DataSourceMembers* self, PyObject *args, PyObject *kwds)
{
    PyObject* pyArg;

    if (!PyArg_ParseTuple(args, "O", &pyArg))
        return -1;

    // check the type of the object.
    // the comparison uses type name (str)
    std::string typeName(pyArg->ob_type->tp_name);

    if (typeName.compare("instru.DataProxy") == 0)
    {
        DataProxyMembers* pyProxy = reinterpret_cast<DataProxyMembers*>(pyArg);

        self->source = const_cast<DataProxy*>(pyProxy->proxy->get());

        PyObject* tmp;

        // retrieve name
        tmp = self->name;
        self->name = PyString_FromString(self->source->name().c_str());
        Py_XDECREF(tmp);

        // retrieve description
        tmp = self->description;
        self->description = PyString_FromString(self->source->description().c_str());
        Py_XDECREF(tmp);

    	return 0;
    }

    if (typeName.compare("instru.ParameterGetter") == 0)
    {
        ParameterGetterMembers* pyGetter = reinterpret_cast<ParameterGetterMembers*>(pyArg);

        self->source = const_cast<ParameterGetter*>(pyGetter->getter->get());

        PyObject* tmp;

        // retrieve name
        tmp = self->name;
        self->name = PyString_FromString(self->source->name().c_str());
        Py_XDECREF(tmp);

        // retrieve description
        tmp = self->description;
        self->description = PyString_FromString(self->source->description().c_str());
        Py_XDECREF(tmp);

    	return 0;
    }

	PyErr_SetString(PyExc_TypeError,
			"The cast can only be done from a ParameterGetter "
			"or DataProxy");
	return -1;
}


#include "PythonDataTarget.h"
#include "PythonInPort.h"
#include "core/Dispatcher.h"
#include "core/DataTarget.h"
#include "core/InPort.h"
#include "Poco/Util/Application.h"

PyObject* itemFromInPort(InPort* port, PyTypeObject& PythonInPortEX)
{
	SharedPtr<InPort*> targetPort =
			Poco::Util::Application::instance()
				.getSubsystem<Dispatcher>()
				.getInPort(port);

    // create the python object
    InPortMembers* pyPort =
        (InPortMembers*)(pyInPortNew((PyTypeObject*)&PythonInPortEX, NULL, NULL) );

    PyObject* tmp=NULL;

    // init
    // retrieve name and description
    tmp = pyPort->name;
    pyPort->name = PyString_FromString(port->name().c_str());
    Py_XDECREF(tmp);

    tmp = pyPort->description;
    pyPort->description = PyString_FromString(port->description().c_str());
    Py_XDECREF(tmp);

    // set InPort reference
    *(pyPort->inPort) = targetPort;

    return (PyObject*)pyPort;
}

PyObject* itemFromDataTarget(DataTarget* target, PyTypeObject& PythonDataTargetEX)
{
    // create the python object
    DataTargetMembers* pyTarget =
        (DataTargetMembers*)(pyDataTargetNew((PyTypeObject*)&PythonDataTargetEX, NULL, NULL) );

    PyObject* tmp=NULL;

    // init
    // retrieve name and description
    tmp = pyTarget->name;
    pyTarget->name = PyString_FromString(target->name().c_str());
    Py_XDECREF(tmp);

    tmp = pyTarget->description;
    pyTarget->description = PyString_FromString(target->description().c_str());
    Py_XDECREF(tmp);

    // set DataTarget reference
    pyTarget->target = target;

    return (PyObject*)pyTarget;
}

PyObject* pyDataSourceGetDataTargets(DataSourceMembers* self)
{
    std::set<DataTarget*> targets = self->source->getDataTargets();

    // prepare python list
    PyObject* pyTargets = PyList_New(0);

    // prepare InPort python type
    if (PyType_Ready(&PythonInPort) < 0)
    {
        PyErr_SetString(PyExc_ImportError,
                "Not able to create the InPort Type");
        return NULL;
    }

    // prepare DataTarget python type
    if (PyType_Ready(&PythonDataTarget) < 0)
    {
        PyErr_SetString(PyExc_ImportError,
                "Not able to create the DataTarget Type");
        return NULL;
    }

    for (std::set<DataTarget*>::iterator it = targets.begin(),
    		ite = targets.end(); it != ite; it++)
    {
        InPort* tmpPort = dynamic_cast<InPort*>(*it);

        PyObject* item;

        try
        {
			if (tmpPort)
				item = itemFromInPort(tmpPort, PythonInPort);
			else
				item = itemFromDataTarget(*it, PythonDataTarget);
        }
        catch (Poco::Exception& e)
        {
            PyErr_SetString(PyExc_RuntimeError,
                    ("Not able to build the list item: "
            		+ e.displayText()).c_str());
            return NULL;
        }

        // create the dict entry
        if (0 > PyList_Append(
                pyTargets,
                item))
        {
            // appending the item failed
            PyErr_SetString(PyExc_RuntimeError,
                    "Not able to build the return list");
            return NULL;
        }

    }

    return pyTargets;
}

/**
 * Construct a python list from a vector
 */
PyObject* getVectorValue(DataSource* data)
{
    // construct pyList
    PyObject* list = PyList_New(0);

    data->readDataLock();

    switch (DataItem::noContainerDataType(data->dataType()))
    {
    case DataItem::typeInt32:
    {
        std::vector<Poco::Int32>* pData;
        pData = data->getData< std::vector<Poco::Int32> >();

        for (std::vector<Poco::Int32>::iterator it = pData->begin(),
                ite = pData->end(); it != ite; it++)
        {
            // create the dict entry
            if ( 0 > PyList_Append(list, PyInt_FromLong(*it)) )
            {
                // appending the item failed
                PyErr_SetString(PyExc_RuntimeError,
                        "Not able to build the return list");
                data->unlockData();
                return NULL;
            }
        }
        break;
    }
    case DataItem::typeUInt32:
    {
        std::vector<Poco::UInt32>* pData;
        pData = data->getData< std::vector<Poco::UInt32> >();

        for (std::vector<Poco::UInt32>::iterator it = pData->begin(),
                ite = pData->end(); it != ite; it++)
        {
            // create the dict entry
            if ( 0 > PyList_Append(list, PyInt_FromSize_t(*it)) )
            {
                // appending the item failed
                PyErr_SetString(PyExc_RuntimeError,
                        "Not able to build the return list");
                data->unlockData();
                return NULL;
            }
        }
        break;
    }
    case DataItem::typeInt64:
    {
        std::vector<Poco::Int64>* pData;
        pData = data->getData< std::vector<Poco::Int64> >();

        for (std::vector<Poco::Int64>::iterator it = pData->begin(),
                ite = pData->end(); it != ite; it++)
        {
            // create the dict entry
            if ( 0 > PyList_Append(list, PyInt_FromLong(static_cast<long>(*it))) )
            {
                // appending the item failed
                PyErr_SetString(PyExc_RuntimeError,
                        "Not able to build the return list");
                data->unlockData();
                return NULL;
            }
        }
        break;
    }
    case DataItem::typeUInt64:
    {
        std::vector<Poco::UInt64>* pData;
        pData = data->getData< std::vector<Poco::UInt64> >();

        for (std::vector<Poco::UInt64>::iterator it = pData->begin(),
                ite = pData->end(); it != ite; it++)
        {
            // create the dict entry
            if ( 0 > PyList_Append(list, PyInt_FromSize_t(*it)) )
            {
                // appending the item failed
                PyErr_SetString(PyExc_RuntimeError,
                        "Not able to build the return list");
                data->unlockData();
                return NULL;
            }
        }
        break;
    }
    case DataItem::typeFloat:
    {
        std::vector<float>* pData;
        pData = data->getData< std::vector<float> >();

        for (std::vector<float>::iterator it = pData->begin(),
                ite = pData->end(); it != ite; it++)
        {
            // create the dict entry
            if ( 0 > PyList_Append(list, PyFloat_FromDouble(*it)) )
            {
                // appending the item failed
                PyErr_SetString(PyExc_RuntimeError,
                        "Not able to build the return list");
                data->unlockData();
                return NULL;
            }
        }
        break;
    }
    case DataItem::typeDblFloat:
    {
        std::vector<double>* pData;
        pData = data->getData< std::vector<double> >();

        for (std::vector<double>::iterator it = pData->begin(),
                ite = pData->end(); it != ite; it++)
        {
            // create the dict entry
            if ( 0 > PyList_Append(list, PyFloat_FromDouble(*it)) )
            {
                // appending the item failed
                PyErr_SetString(PyExc_RuntimeError,
                        "Not able to build the return list");
                data->unlockData();
                return NULL;
            }
        }
        break;
    }
    case DataItem::typeString:
    {
        std::vector<std::string>* pData;
        pData = data->getData< std::vector<std::string> >();

        for (std::vector<std::string>::iterator it = pData->begin(),
                ite = pData->end(); it != ite; it++)
        {
            // create the dict entry
            if ( 0 > PyList_Append(list, PyString_FromString(it->c_str())) )
            {
                // appending the item failed
                PyErr_SetString(PyExc_RuntimeError,
                        "Not able to build the return list");
                data->unlockData();
                return NULL;
            }
        }
        break;
    }
    default:
        PyErr_SetString(PyExc_NotImplementedError,
                "getValue is not implemented for this dataType");
        data->unlockData();
        return NULL;
    }

    data->unlockData();

    return list;
}

PyObject* pyDataSourceGetDataValue(DataSourceMembers* self)
{
    int dataType = self->source->dataType();

    if (DataItem::isVector(dataType))
        return getVectorValue(self->source);

    // python object to return
    PyObject* pyObj;

    self->source->readDataLock();

    switch (DataItem::noContainerDataType(dataType))
    {
    case DataItem::typeInt32:
    {
        long data = *(self->source->getData<Poco::Int32>());
        pyObj = PyInt_FromLong(data);
        break;
    }
    case DataItem::typeUInt32:
    {
        size_t data = *(self->source->getData<Poco::UInt32>());
        pyObj = PyInt_FromSize_t(data);
        break;
    }
    case DataItem::typeInt64:
    {
        long data = static_cast<long>(*(self->source->getData<Poco::Int64>()));
        pyObj = PyInt_FromLong(data);
        break;
    }
    case DataItem::typeUInt64:
    {
        size_t data = *(self->source->getData<Poco::UInt64>());
        pyObj = PyInt_FromSize_t(data);
        break;
    }
    case DataItem::typeFloat:
    {
        double data = *(self->source->getData<float>());
        pyObj = PyFloat_FromDouble(data);
        break;
    }
    case DataItem::typeDblFloat:
    {
        double data = *(self->source->getData<double>());
        pyObj = PyFloat_FromDouble(data);
        break;
    }
    case DataItem::typeString:
    {
        std::string* pData = self->source->getData<std::string>();
        pyObj = PyString_FromString(pData->c_str());
        break;
    }
    default:
        PyErr_SetString(PyExc_NotImplementedError,
                "getValue is not implemented for this dataType");
        self->source->unlockData();
        return NULL;
    }

    self->source->unlockData();

    return pyObj;
}

#endif /* HAVE_PYTHON27 */
