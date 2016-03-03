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
    PyErr_SetString(PyExc_NotImplementedError, "getValue is not implemented");
    return NULL;

//    Poco::SharedPtr<OutPort*> sharedSource;
//
//    sharedSource = (**self->inPort)->getSourcePort();
//
//    // check if connected
//    if ( *sharedSource == Poco::Util::Application::instance()
//                            .getSubsystem<Dispatcher>()
//                            .getEmptyOutPort() )
//    {
//        PyErr_SetString(PyExc_ReferenceError,
//                "This input port has no source");
//        return NULL;
//    }
//
//    // prepare OutPort python type
//    if (PyType_Ready(&PythonOutPort) < 0)
//    {
//        PyErr_SetString(PyExc_ImportError,
//                "Not able to create the OutPort Type");
//        return NULL;
//    }
//
//    // create the python object
//    OutPortMembers* pyPort =
//        (OutPortMembers*)(pyOutPortNew((PyTypeObject*)&PythonOutPort, NULL, NULL) );
//
//    PyObject* tmp=NULL;
//
//    // init
//    // retrieve name and description
//    tmp = pyPort->name;
//    pyPort->name = PyString_FromString((*sharedSource)->name().c_str());
//    Py_XDECREF(tmp);
//
//    tmp = pyPort->description;
//    pyPort->description = PyString_FromString((*sharedSource)->description().c_str());
//    Py_XDECREF(tmp);
//
//    // set ModuleFactory reference
//    *(pyPort->outPort) = sharedSource;
//
//    return (PyObject*) pyPort;
}

PyObject* pyDataGetNewValue(DataMembers* self)
{
    PyErr_SetString(PyExc_NotImplementedError, "getNewValue is not implemented");
    return NULL;

//    Poco::SharedPtr<OutPort*> sharedSource;
//
//    sharedSource = (**self->inPort)->getSeqSourcePort();
//
//    // check if connected
//    if ( *sharedSource == Poco::Util::Application::instance()
//                            .getSubsystem<Dispatcher>()
//                            .getEmptyOutPort() )
//    {
//        PyErr_SetString(PyExc_ReferenceError,
//                "This input port has no source");
//        return NULL;
//    }
//
//    // prepare OutPort python type
//    if (PyType_Ready(&PythonOutPort) < 0)
//    {
//        PyErr_SetString(PyExc_ImportError,
//                "Not able to create the OutPort Type");
//        return NULL;
//    }
//
//    // create the python object
//    OutPortMembers* pyPort =
//        (OutPortMembers*)(pyOutPortNew((PyTypeObject*)&PythonOutPort, NULL, NULL) );
//
//    PyObject* tmp=NULL;
//
//    // init
//    // retrieve name and description
//    tmp = pyPort->name;
//    pyPort->name = PyString_FromString((*sharedSource)->name().c_str());
//    Py_XDECREF(tmp);
//
//    tmp = pyPort->description;
//    pyPort->description = PyString_FromString((*sharedSource)->description().c_str());
//    Py_XDECREF(tmp);
//
//    // set ModuleFactory reference
//    *(pyPort->outPort) = sharedSource;
//
//    return (PyObject*) pyPort;
}

#endif /* HAVE_PYTHON27 */
