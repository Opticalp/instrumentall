/**
 * @file	src/PythonMainAppExport.cpp
 * @date	dec. 2015
 * @author	PhRG - opticalp.fr
 */

/*
Copyright (c) 2015 Ph. Renaud-Goud / Opticalp

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

#include "MainApplication.h"
#include "ModuleManager.h"

#include "PythonMainAppExport.h"

extern "C" PyObject*
pythonMainAppAbout(PyObject *self, PyObject *args)
{
    std::string retVal;

    retVal = dynamic_cast<MainApplication&>(
            Poco::Util::Application::instance() )
            .about();

    return PyString_FromString(retVal.c_str());

}

extern "C" PyObject*
pythonMainAppVersion(PyObject *self, PyObject *args)
{
    std::string retVal;

    retVal = dynamic_cast<MainApplication&>(
            Poco::Util::Application::instance() )
            .version();

    return PyString_FromString(retVal.c_str());
}

#include "PythonModuleFactory.h"

extern "C" PyObject*
pythonModManGetRootFact(PyObject *self, PyObject *args)
{
    std::vector< SharedPtr<ModuleFactory*> > factories;

    factories = Poco::Util::Application::instance()
                .getSubsystem<ModuleManager>()
                .getRootFactories();

    // construct the list to return
    PyObject* pyFactories = PyList_New(0);

    // prepare ModuleFactory python type
    if (PyType_Ready(&PythonModuleFactory) < 0)
    {
        PyErr_SetString(PyExc_ImportError,
                "Not able to create the ModuleFactory Type");
        return NULL;
    }

    for (std::vector< SharedPtr<ModuleFactory*> >::iterator it=factories.begin(), ite=factories.end();
            it != ite; it++ )
    {
        // create the python object
        ModFactMembers* pyFact =
            reinterpret_cast<ModFactMembers*>(
                pyModFactNew(
                    reinterpret_cast<PyTypeObject*>(&PythonModuleFactory), NULL, NULL) );

        PyObject* tmp=NULL;

        // init
        // retrieve name and description
        tmp = pyFact->name;
        pyFact->name = PyString_FromString((**it)->name().c_str());
        Py_XDECREF(tmp);

        tmp = pyFact->description;
        pyFact->description = PyString_FromString((**it)->description());
        Py_XDECREF(tmp);

        // set InPort reference
        *(pyFact->moduleFactory) = *it;

        // create the dict entry
        if (0 > PyList_Append(
                pyFactories,
                reinterpret_cast<PyObject*>(pyFact)))
        {
            // appending the item failed
            PyErr_SetString(PyExc_RuntimeError,
                    "Not able to build the return list");
            return NULL;
        }
    }

    return pyFactories;
}

#include "PythonModule.h"

extern "C" PyObject*
pythonModManGetModules(PyObject *self, PyObject *args)
{
    std::vector< SharedPtr<Module*> > modules;

    modules = Poco::Util::Application::instance()
                .getSubsystem<ModuleManager>()
                .getModules();

    // construct the list to return
    PyObject* pyModules = PyList_New(0);

    // prepare Module python type
    if (PyType_Ready(&PythonModule) < 0)
    {
        PyErr_SetString(PyExc_ImportError,
                "Not able to create the Module Type");
        return NULL;
    }

    for (std::vector< SharedPtr<Module*> >::iterator it = modules.begin(),
            ite = modules.end(); it != ite; it++ )
    {
        // create the python object
        ModMembers* pyMod =
            reinterpret_cast<ModMembers*>(
                pyModNew(
                    reinterpret_cast<PyTypeObject*>(&PythonModule), NULL, NULL) );

        PyObject* tmp=NULL;

        // init
        // retrieve name and description
        tmp = pyMod->name;
        pyMod->name = PyString_FromString((**it)->name().c_str());
        Py_XDECREF(tmp);

        tmp = pyMod->internalName;
        pyMod->internalName = PyString_FromString((**it)->internalName().c_str());
        Py_XDECREF(tmp);

        tmp = pyMod->description;
        pyMod->description = PyString_FromString((**it)->description());
        Py_XDECREF(tmp);

        // set InPort reference
        *(pyMod->module) = *it;

        // create the dict entry
        if (0 > PyList_Append(
                pyModules,
                reinterpret_cast<PyObject*>(pyMod)))
        {
            // appending the item failed
            PyErr_SetString(PyExc_RuntimeError,
                    "Not able to build the return list");
            return NULL;
        }
    }

    return pyModules;
}

#include "Dispatcher.h"
#include "PythonInPort.h"
#include "PythonOutPort.h"
#include <algorithm> // std::swap

extern "C" PyObject*
pythonDispatchBind(PyObject* self, PyObject* args)
{
    PyObject *pyObj1, *pyObj2;

    // arguments parsing
    if (!PyArg_ParseTuple(args, "OO:bind", &pyObj1, &pyObj2))
        return NULL;

    // check the type of the object.
    // the comparison uses type name (str)
    std::string typeName1(pyObj1->ob_type->tp_name);
    std::string typeName2(pyObj2->ob_type->tp_name);

    if (typeName2.compare("instru.InPort")==0
            && typeName1.compare("instru.OutPort")==0)
    {
        std::swap(pyObj1, pyObj2);
    }
    else if (typeName1.compare("instru.InPort")
            || typeName2.compare("instru.OutPort"))
    {
        PyErr_SetString(PyExc_TypeError,
                "The arguments must be an InPort and an OutPort");
        return NULL;
    }

    InPortMembers* pyInPort = reinterpret_cast<InPortMembers*>(pyObj1);
    OutPortMembers* pyOutPort = reinterpret_cast<OutPortMembers*>(pyObj2);

    try
    {
        Poco::Util::Application::instance()
            .getSubsystem<Dispatcher>()
            .bind(*pyOutPort->outPort,*pyInPort->inPort);
    }
    catch (DispatcherException& e)
    {
        PyErr_SetString(PyExc_RuntimeError, e.displayText().c_str());
        return NULL;
    }

    return Py_BuildValue("");
}

extern "C" PyObject*
pythonDispatchUnbind(PyObject* self, PyObject* args)
{
    PyObject* pyObj;

    // arguments parsing
    if (!PyArg_ParseTuple(args, "O:unbind", &pyObj))
        return NULL;

    // check the type of the object.
    // the comparison uses type name (str)
    std::string typeName(pyObj->ob_type->tp_name);

    if (typeName.compare("instru.InPort"))
    {
        PyErr_SetString(PyExc_TypeError, "inPort has to be an InPort");
        return NULL;
    }

    InPortMembers* pyPort = reinterpret_cast<InPortMembers*>(pyObj);

    Poco::Util::Application::instance()
        .getSubsystem<Dispatcher>()
        .unbind(*pyPort->inPort);

    return Py_BuildValue("");
}

extern "C" PyObject*
pythonDispatchSeqBind(PyObject* self, PyObject* args)
{
    PyObject *pyObj1, *pyObj2;

    // arguments parsing
    if (!PyArg_ParseTuple(args, "OO:bind", &pyObj1, &pyObj2))
        return NULL;

    // check the type of the object.
    // the comparison uses type name (str)
    std::string typeName1(pyObj1->ob_type->tp_name);
    std::string typeName2(pyObj2->ob_type->tp_name);

    if (typeName2.compare("instru.InPort")==0
            && typeName1.compare("instru.OutPort")==0)
    {
        std::swap(pyObj1, pyObj2);
    }
    else if (typeName1.compare("instru.InPort")
            || typeName2.compare("instru.OutPort"))
    {
        PyErr_SetString(PyExc_TypeError,
                "The arguments must be an InPort and an OutPort");
        return NULL;
    }

    InPortMembers* pyInPort = reinterpret_cast<InPortMembers*>(pyObj1);
    OutPortMembers* pyOutPort = reinterpret_cast<OutPortMembers*>(pyObj2);

    try
    {
        Poco::Util::Application::instance()
            .getSubsystem<Dispatcher>()
            .seqBind(*pyOutPort->outPort,*pyInPort->inPort);
    }
    catch (DispatcherException& e)
    {
        PyErr_SetString(PyExc_RuntimeError, e.displayText().c_str());
        return NULL;
    }

    return Py_BuildValue("");
}

extern "C" PyObject*
pythonDispatchSeqUnbind(PyObject* self, PyObject* args)
{
    PyObject* pyObj;

    // arguments parsing
    if (!PyArg_ParseTuple(args, "O:unbind", &pyObj))
        return NULL;

    // check the type of the object.
    // the comparison uses type name (str)
    std::string typeName(pyObj->ob_type->tp_name);

    if (typeName.compare("instru.InPort"))
    {
        PyErr_SetString(PyExc_TypeError, "inPort has to be an InPort");
        return NULL;
    }

    InPortMembers* pyPort = reinterpret_cast<InPortMembers*>(pyObj);

    Poco::Util::Application::instance()
        .getSubsystem<Dispatcher>()
        .seqUnbind(*pyPort->inPort);

    return Py_BuildValue("");
}

#endif /* HAVE_PYTHON27 */
