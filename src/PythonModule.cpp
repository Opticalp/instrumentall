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
        *self->module =
                Poco::Util::Application::instance().getSubsystem<ModuleManager>().getModule(name);
    }
    catch (Poco::Exception& e)
    {
        std::string errMsg = "Not able to initialize the module: "
                + e.displayText();
        PyErr_SetString(PyExc_RuntimeError, errMsg.c_str());
        return -1;
    }

    // retrieve name and description
    tmp = self->name;
    self->name = PyString_FromString((**self->module)->name().c_str());
    Py_XDECREF(tmp);

    tmp = self->internalName;
    self->internalName = PyString_FromString((**self->module)->internalName().c_str());
    Py_XDECREF(tmp);

    tmp = self->description;
    self->description = PyString_FromString((**self->module)->description().c_str());
    Py_XDECREF(tmp);

    return 0;
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
    catch (Poco::Exception& e)
    {
        PyErr_SetString(PyExc_RuntimeError,
                e.displayText().c_str());
        return NULL;
    }

    // alloc
    if (PyType_Ready(&PythonModuleFactory) < 0)
    {
        PyErr_SetString(PyExc_ImportError,
                "Not able to create the ModuleFactory Type");
        return NULL;
    }

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
    pyParent->description = PyString_FromString((*factory)->description().c_str());
    Py_XDECREF(tmp);

    // set ModuleFactory reference
    *(pyParent->moduleFactory) = factory;

    return (PyObject*)(pyParent);
}

#include "Dispatcher.h"
#include "InDataPort.h"
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
        PyErr_SetString(PyExc_ImportError,
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

            // set InPort reference
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
    catch (Poco::Exception& e)
    {
        PyErr_SetString(PyExc_RuntimeError,
                e.displayText().c_str());
        return NULL;
    }

    return pyPorts;
}

PyObject* pyModInPort(ModMembers* self, PyObject *args)
{
    char* portNameChar;

    if (!PyArg_ParseTuple(args, "s:inPort", &portNameChar))
        return NULL;

    std::string portName(portNameChar);

    SharedPtr<InPort*> sharedPort;
    try
    {
        sharedPort = Poco::Util::Application::instance()
                        .getSubsystem<Dispatcher>()
                        .getInPort((**self->module)->getInPort(portName));
    }
    catch (Poco::Exception& e)
    {
        PyErr_SetString(PyExc_RuntimeError,
                e.displayText().c_str());
        return NULL;
    }

    // prepare InPort python type
    if (PyType_Ready(&PythonInPort) < 0)
    {
        PyErr_SetString(PyExc_ImportError,
                "Not able to create the InPort Type");
        return NULL;
    }

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

    // set InPort reference
    *(pyPort->inPort) = sharedPort;

    return reinterpret_cast<PyObject*>(pyPort);
}

#include "OutPort.h"
#include "PythonOutPort.h"

PyObject* pyModOutPorts(ModMembers* self)
{
    std::vector<OutPort*> ports;
    Dispatcher& dispatcher = Poco::Util::Application::instance()
                                        .getSubsystem<Dispatcher>();

    ports = (**self->module)->getOutPorts();

    // prepare python list
    PyObject* pyPorts = PyList_New(0);

    // prepare OutPort python type
    if (PyType_Ready(&PythonOutPort) < 0)
    {
        PyErr_SetString(PyExc_ImportError,
                "Not able to create the OutPort Type");
        return NULL;
    }

    try
    {
        for (std::vector<OutPort*>::iterator it = ports.begin(),
                ite = ports.end(); it != ite; it++ )
        {
            Poco::SharedPtr<OutPort*> sharedPort;
            sharedPort = dispatcher.getOutPort(*it);

            // create the python object
            OutPortMembers* pyPort =
                (OutPortMembers*)(pyOutPortNew((PyTypeObject*)&PythonOutPort, NULL, NULL) );

            PyObject* tmp=NULL;

            // init
            // retrieve name and description
            tmp = pyPort->name;
            pyPort->name = PyString_FromString((*sharedPort)->name().c_str());
            Py_XDECREF(tmp);

            tmp = pyPort->description;
            pyPort->description = PyString_FromString((*sharedPort)->description().c_str());
            Py_XDECREF(tmp);

            // set OutPort reference
            *(pyPort->outPort) = sharedPort;

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
    catch (Poco::Exception& e)
    {
        PyErr_SetString(PyExc_RuntimeError,
                e.displayText().c_str());
        return NULL;
    }

    return pyPorts;
}

PyObject* pyModOutPort(ModMembers* self, PyObject *args)
{
    char* portNameChar;

    if (!PyArg_ParseTuple(args, "s:outPort", &portNameChar))
        return NULL;

    std::string portName(portNameChar);

    SharedPtr<OutPort*> sharedPort;

    try
    {
        sharedPort = Poco::Util::Application::instance()
                        .getSubsystem<Dispatcher>()
                        .getOutPort((**self->module)->getOutPort(portName));
    }
    catch (Poco::Exception& e)
    {
        PyErr_SetString(PyExc_RuntimeError,
                e.displayText().c_str());
        return NULL;
    }

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
    pyPort->name = PyString_FromString((*sharedPort)->name().c_str());
    Py_XDECREF(tmp);

    tmp = pyPort->description;
    pyPort->description = PyString_FromString((*sharedPort)->description().c_str());
    Py_XDECREF(tmp);

    // set InPort reference
    *(pyPort->outPort) = sharedPort;

    return reinterpret_cast<PyObject*>(pyPort);
}

PyObject* pyModGetParameterSet(ModMembers *self)
{
    ParameterSet paramSet;

    (**self->module)->getParameterSet(&paramSet);

    // prepare python list
    PyObject* pyParamSet = PyList_New(0);

    size_t length = paramSet.size();
    for (size_t index = 0; index < length; index++)
    {
        PyObject* pyParam = PyDict_New();

        PyDict_SetItemString(pyParam, "index", PyLong_FromSize_t(index));
        PyDict_SetItemString(pyParam, "name", PyString_FromString(paramSet[index].name.c_str()));
        PyDict_SetItemString(pyParam, "descr", PyString_FromString(paramSet[index].descr.c_str()));

        PyObject* paramType;
        switch (paramSet[index].datatype)
        {
        case ParamItem::typeInteger:
            paramType = PyObject_Type(PyInt_FromLong(0));
            break;
        case ParamItem::typeFloat:
            paramType = PyObject_Type(PyFloat_FromDouble(0.0));
            break;
        case ParamItem::typeString:
            paramType = PyObject_Type(PyString_FromString("0"));
            break;
        default:
            PyErr_SetString(PyExc_RuntimeError,"unknown parameter type");
            return NULL;
        }

        PyDict_SetItemString(pyParam, "type", paramType);

        // create the dict entry
        if (0 > PyList_Append(
                pyParamSet,
                pyParam))
        {
            // appending the item failed
            PyErr_SetString(PyExc_RuntimeError,
                    "Not able to build the return list");
            return NULL;
        }

    }

    return pyParamSet;
}

PyObject* pyModGetParameterValue(ModMembers *self, PyObject *args)
{
    char *charParamName;

    if (!PyArg_ParseTuple(args, "s:getParameterValue", &charParamName))
        return NULL;

    std::string paramName(charParamName);

    try
    {
        switch ((**self->module)->getParameterType(paramName))
        {
        case ParamItem::typeInteger:
        {
            long value =
                    (**self->module)->getParameterValue<Poco::Int64>(paramName);
            return PyLong_FromLong(value);
        }
        case ParamItem::typeFloat:
        {
            double value =
                    (**self->module)->getParameterValue<double>(paramName);
            return PyFloat_FromDouble(value);
        }
        case ParamItem::typeString:
        {
            std::string value =
                    (**self->module)->getParameterValue<std::string>(paramName);
            return PyString_FromString(value.c_str());
        }
        default:
            poco_bugcheck_msg("python getParameterValue, "
                    "getParameterType returned unknown data type");
            throw Poco::BugcheckException();
        }
    }
    catch (Poco::Exception& e)
    {
        PyErr_SetString(PyExc_RuntimeError, e.displayText().c_str());
        return NULL;
    }
}

PyObject* pyModSetParameterValue(ModMembers *self, PyObject *args)
{
    char* charParamName;
    PyObject* pyValue;

    if (!PyArg_ParseTuple(args, "sO:setParameterValue", &charParamName, &pyValue))
        return NULL;

    std::string paramName(charParamName);

    try
    {
        switch ((**self->module)->getParameterType(paramName))
        {
        case ParamItem::typeInteger:
        {
            long value = PyLong_AsLong(pyValue);
            if (PyErr_Occurred()) { return NULL; }
            (**self->module)->setParameterValue<Poco::Int64>(paramName, value, true);
            break;
        }
        case ParamItem::typeFloat:
        {
            double value = PyFloat_AsDouble(pyValue);
            if (PyErr_Occurred()) { return NULL; }
            (**self->module)->setParameterValue<double>(paramName, value, true);
            break;
        }
        case ParamItem::typeString:
        {
            std::string value(PyString_AsString(pyValue)) ;
            if (PyErr_Occurred()) { return NULL; }
            (**self->module)->setParameterValue<std::string>(paramName, value, true);
            break;
        }
        default:
            poco_bugcheck_msg("python setParameterValue, "
                    "getParameterType returned unknown data type");
            throw Poco::BugcheckException();
        }
    }
    catch (Poco::Exception& e)
    {
        PyErr_SetString(PyExc_RuntimeError, e.displayText().c_str());
        return NULL;
    }

    Py_RETURN_NONE;
}

PyObject* pyModSetVerbosity(ModMembers* self, PyObject* args)
{
	int prio;

    if (!PyArg_ParseTuple(args, "i:setVerbosity", &prio))
        return NULL;

    (**self->module)->setVerbosity(prio);

    Py_RETURN_NONE;
}

PyObject* pyModGetVerbosity(ModMembers* self, PyObject* args)
{
	long prio = (**self->module)->getVerbosity();

	return PyInt_FromLong(prio);
}

#include "PythonParameterGetter.h"

PyObject* pyModBuildParamGetter(ModMembers* self, PyObject* args)
{
	char* charParamName;

    if (!PyArg_ParseTuple(args, "s:buildParameterGetter", &charParamName))
        return NULL;

    std::string paramName(charParamName);

    Poco::AutoPtr<ParameterGetter> getter =
    		(**self->module)->buildParameterGetter(paramName);

    // prepare OutPort python type
    if (PyType_Ready(&PythonParameterGetter) < 0)
    {
        PyErr_SetString(PyExc_ImportError,
                "Not able to create the ParameterGetter Type");
        return NULL;
    }

    // create the python object
    ParameterGetterMembers* pyGetter =
        (ParameterGetterMembers*)(pyParameterGetterNew((PyTypeObject*)&PythonParameterGetter, NULL, NULL) );

    // set InPort reference
    *(pyGetter->getter) = getter;

    return reinterpret_cast<PyObject*>(pyGetter);
}

#include "PythonParameterSetter.h"

PyObject* pyModBuildParamSetter(ModMembers* self, PyObject* args)
{
	char* charParamName;

    if (!PyArg_ParseTuple(args, "s:buildParameterSetter", &charParamName))
        return NULL;

    std::string paramName(charParamName);

    Poco::AutoPtr<ParameterSetter> setter =
    		(**self->module)->buildParameterSetter(paramName);

    // prepare OutPort python type
    if (PyType_Ready(&PythonParameterSetter) < 0)
    {
        PyErr_SetString(PyExc_ImportError,
                "Not able to create the ParameterSetter Type");
        return NULL;
    }

    // create the python object
    ParameterSetterMembers* pySetter =
        (ParameterSetterMembers*)(pyParameterSetterNew((PyTypeObject*)&PythonParameterSetter, NULL, NULL) );

    // set InPort reference
    *(pySetter->setter) = setter;

    return reinterpret_cast<PyObject*>(pySetter);
}

#endif /* HAVE_PYTHON27 */
