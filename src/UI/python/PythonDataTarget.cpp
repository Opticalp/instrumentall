/**
 * @file	src/UI/python/PythonDataTarget.cpp
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

#include "PythonDataTarget.h"
#include "core/DataTarget.h"

extern "C" void pyDataTargetDealloc(DataTargetMembers* self)
{
    // "name" and "description" are python objects,
    // so we decrement the references on them,
    // instead of deleting it directly.
    Py_XDECREF(self->name);
    Py_XDECREF(self->description);

    self->ob_type->tp_free((PyObject*) self); // free the object’s memory
}

extern "C" PyObject* pyDataTargetNew(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    DataTargetMembers* self;

    self = (DataTargetMembers*) type->tp_alloc(type, 0);
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
#include "PythonParameterSetter.h"
#include "PythonDataProxy.h"
#include "PythonDataLogger.h"

extern "C" int pyDataTargetInit(DataTargetMembers* self, PyObject *args, PyObject *kwds)
{
    PyObject* pyArg;

    if (!PyArg_ParseTuple(args, "O", &pyArg))
        return -1;

    // check the type of the object.
    // the comparison uses type name (str)
    std::string typeName(pyArg->ob_type->tp_name);

    if (typeName.compare("instru.DataLogger") == 0)
    {
        DataLoggerMembers* pyLogger = reinterpret_cast<DataLoggerMembers*>(pyArg);

        self->target = const_cast<DataLogger*>(pyLogger->logger->get());
    }
    else if (typeName.compare("instru.DataProxy") == 0)
    {
        DataProxyMembers* pyProxy = reinterpret_cast<DataProxyMembers*>(pyArg);

        self->target = const_cast<DataProxy*>(pyProxy->proxy->get());
    }
    else if (typeName.compare("instru.ParameterGetter") == 0)
    {
        ParameterGetterMembers* pyGetter = reinterpret_cast<ParameterGetterMembers*>(pyArg);

        self->target = const_cast<ParameterGetter*>(pyGetter->getter->get());
    }
    else if (typeName.compare("instru.ParameterSetter") == 0)
    {
        ParameterSetterMembers* pySetter = reinterpret_cast<ParameterSetterMembers*>(pyArg);

        self->target = const_cast<ParameterSetter*>(pySetter->setter->get());
    }
    else
    {
		PyErr_SetString(PyExc_TypeError,
				"The cast can only be done from a ParameterGetter, "
				"DataLogger or DataProxy");
		return -1;
    }

    PyObject* tmp;

    // retrieve name
    tmp = self->name;
    self->name = PyString_FromString(self->target->name().c_str());
    Py_XDECREF(tmp);

    // retrieve description
    tmp = self->description;
    self->description = PyString_FromString(self->target->description().c_str());
    Py_XDECREF(tmp);

	return 0;
}


#include "core/Dispatcher.h"
#include "core/DataSource.h"
#include "core/OutPort.h"
#include "PythonDataSource.h"
#include "PythonOutPort.h"
#include "Poco/Util/Application.h"

PyObject* pyDataTargetGetDataSource(DataTargetMembers* self)
{
    DataSource* tmpSource;
    OutPort* tmpPort;

    try
    {
    	tmpSource = self->target->getDataSource();
    }
    catch (Poco::NullPointerException& )
    {
        Py_RETURN_NONE;
    }

	tmpPort = dynamic_cast<OutPort*>(tmpSource);

	if (tmpPort)
	{
		Poco::SharedPtr<OutPort*> sharedSource = Poco::Util::Application::instance()
			.getSubsystem<Dispatcher>()
			.getOutPort(tmpPort);

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
	else // Python DataSource
	{
		// prepare DataSource python type
		if (PyType_Ready(&PythonDataSource) < 0)
		{
			PyErr_SetString(PyExc_ImportError,
					"Not able to create the DataSource Type");
			return NULL;
		}

		// create the python object
		DataSourceMembers* pySource =
			(DataSourceMembers*)(pyDataSourceNew((PyTypeObject*)&PythonDataSource, NULL, NULL) );

		PyObject* tmp=NULL;

		// init
		// retrieve name and description
		tmp = pySource->name;
		pySource->name = PyString_FromString(tmpSource->name().c_str());
		Py_XDECREF(tmp);

		tmp = pySource->description;
		pySource->description = PyString_FromString(tmpSource->description().c_str());
		Py_XDECREF(tmp);

		// set DataSource reference
		pySource->source = tmpSource;

		return (PyObject*) pySource;
	}
}

#endif /* HAVE_PYTHON27 */
