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
#include "PythonOutPort.h"
#include "PythonModule.h"

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
    catch (ModuleException& e) // from getModule
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
    pyParent->description = PyString_FromString((*module)->description());
    Py_XDECREF(tmp);

    // set ModuleFactory reference
    *(pyParent->module) = module;

    return (PyObject*)(pyParent);
}

#include "Dispatcher.h"
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


#include "DataItem.h"
#include "DataManager.h"
#include "PythonData.h"

extern "C" PyObject* pyOutPortData(OutPortMembers *self)
{
    Poco::SharedPtr<DataItem*> dataItem;

    try
    {
        dataItem = Poco::Util::Application::instance()
                          .getSubsystem<DataManager>()
                          .getDataItem( (**self->outPort)->dataItem() );
    }
    catch (Poco::NotFoundException& e) // from getDataItem
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
            reinterpret_cast<DataMembers*>(
                    pyDataNew((PyTypeObject*)&PythonData, NULL, NULL) );

    PyObject* tmp=NULL;

    // init
    // set ModuleFactory reference
    *(pyData->data) = dataItem;

    return (PyObject*)(pyData);
}

#endif /* HAVE_PYTHON27 */
