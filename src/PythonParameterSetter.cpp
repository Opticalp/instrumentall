/**
 * @file	src/PythonParameterSetter.cpp
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

#include "ParameterizedEntity.h"

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

#endif /* HAVE_PYTHON27 */
