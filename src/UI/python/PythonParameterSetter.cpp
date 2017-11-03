/**
 * @file	src/UI/python/PythonParameterSetter.cpp
 * @date	Aug. 2016
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

#include "PythonParameterSetter.h"

#include "core/ParameterizedEntityWithWorkers.h"
#include "core/Module.h"

#include "core/ModuleManager.h"

#include "PythonModule.h"

using Poco::AutoPtr;

extern "C" void pyParameterSetterDealloc(ParameterSetterMembers* self)
{
    delete self->setter;

    self->ob_type->tp_free((PyObject*) self); // free the object’s memory
}

extern "C" PyObject* pyParameterSetterNew(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    ParameterSetterMembers* self;

    self = (ParameterSetterMembers*) type->tp_alloc(type, 0);
    if (self != NULL)
        self->setter = new AutoPtr<ParameterSetter>;

    return (PyObject *) self;
}

extern "C" int pyParameterSetterInit(ParameterSetterMembers* self, PyObject *args, PyObject *kwds)
{
    PyObject* tmp=NULL;

    if (!PyArg_ParseTuple(args, ""))
        return -1;

	PyErr_SetString(PyExc_NotImplementedError,
			"Initializer not implemented. "
			"Please use module.buildParameterSetter instead. ");
	return -1;
}

PyObject* pyParameterSetterParameterName(ParameterSetterMembers* self)
{
	std::string ret;

	try
	{
		ret = (*self->setter)->getParent()->name() + "."
				+ (*self->setter)->getParameterName();
	}
	catch (Poco::Exception& e)
	{
		PyErr_SetString(PyExc_RuntimeError,
				e.displayText().c_str());
		return NULL;
	}

	PyObject* pyRet = PyString_FromString(ret.c_str());

	return pyRet;
}

PyObject* pyParameterSetterParent(ParameterSetterMembers* self)
{
    Poco::SharedPtr<Module*> module;

    try
    {
        module = Poco::Util::Application::instance()
                          .getSubsystem<ModuleManager>()
                          .getModule( reinterpret_cast<Module*>((*self->setter)->getParent()) );
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

#endif /* HAVE_PYTHON27 */
