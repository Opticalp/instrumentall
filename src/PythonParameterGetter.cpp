/**
 * @file	src/PythonParameterGetter.cpp
 * @date	Jul. 2016
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

#include "PythonParameterGetter.h"

#include "ParameterizedEntity.h"

using Poco::AutoPtr;

extern "C" void pyParameterGetterDealloc(ParameterGetterMembers* self)
{
    delete self->getter;

    self->ob_type->tp_free((PyObject*) self); // free the object’s memory
}

extern "C" PyObject* pyParameterGetterNew(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    ParameterGetterMembers* self;

    self = (ParameterGetterMembers*) type->tp_alloc(type, 0);
    if (self != NULL)
        self->getter = new AutoPtr<ParameterGetter>;

    return (PyObject *) self;
}

extern "C" int pyParameterGetterInit(ParameterGetterMembers* self, PyObject *args, PyObject *kwds)
{
    PyObject* tmp=NULL;

    if (!PyArg_ParseTuple(args, ""))
        return -1;

	PyErr_SetString(PyExc_NotImplementedError,
			"Initializer not implemented. "
			"Please use module.buildParameterGetter instead. ");
	return -1;
}

PyObject* pyParameterGetterParameterName(ParameterGetterMembers* self)
{
	std::string ret;

	try
	{
		ret = (*self->getter)->getParent()->name() + "."
				+ (*self->getter)->getParameterName();
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
