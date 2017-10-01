/**
 * @file	src/UI/python/PythonBreaker.cpp
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

#include "PythonBreaker.h"


extern "C" void pyBreakerDealloc(BreakerMembers* self)
{
    delete self->breaker;

    self->ob_type->tp_free((PyObject*) self); // free the object’s memory
}

extern "C" PyObject* pyBreakerNew(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    BreakerMembers* self;

    self = (BreakerMembers*) type->tp_alloc(type, 0);
    if (self != NULL)
        self->breaker = new Breaker;

    return (PyObject *) self;
}

extern "C" int pyBreakerInit(BreakerMembers* self, PyObject* args, PyObject *kwds)
{
	PyObject pyObj;

	if (!PyArg_ParseTuple(args, "", &pyObj))
		return -1;

	return 0;
}

#include "PythonOutPort.h"

extern "C" PyObject* pyBreakerBreakAllTargets(BreakerMembers* self, PyObject* args)
{
	PyObject *pyObj;

    // arguments parsing
    if (!PyArg_ParseTuple(args, "O:breakAllTargets", &pyObj))
        return NULL;

    // check if the object is a data source
    // the comparison uses type name (str)
    std::string typeName(pyObj->ob_type->tp_name);

    DataSource* source;

    if (typeName.compare("instru.OutPort")) // only data source yet
    {
        PyErr_SetString(PyExc_TypeError,
                "The argument must be a data source (output port)");
        return NULL;
    }
    else
    {
    	source = **reinterpret_cast<OutPortMembers*>(pyObj)->outPort;
    }

    try
    {
    	self->breaker->breakAllTargetsFromSource(source);
    }
    catch (Poco::Exception& e)
    {
        PyErr_SetString(PyExc_RuntimeError,
                e.displayText().c_str());
        return NULL;
    }

    Py_RETURN_NONE;
}

#include "PythonInPort.h"
#include "PythonDataLogger.h"

extern "C" PyObject* pyBreakerBreakSource(BreakerMembers* self, PyObject* args)
{
	PyObject *pyObj;

    // arguments parsing
    if (!PyArg_ParseTuple(args, "O:breakSource", &pyObj))
        return NULL;

    // check if the object is a data target
    // the comparison uses type name (str)
    std::string typeName(pyObj->ob_type->tp_name);

    DataTarget* target;

    if (typeName.compare("instru.InPort") == 0)
    {
    	target = **reinterpret_cast<InPortMembers*>(pyObj)->inPort;
    }
    else if (typeName.compare("instru.DataLogger") == 0)
    {
    	target = *reinterpret_cast<DataLoggerMembers*>(pyObj)->logger;
    }
    else
    {
        PyErr_SetString(PyExc_TypeError,
                "The argument must be a data target (input port or data logger)");
        return NULL;
    }

    try
    {
    	self->breaker->breakSourceToTarget(target);
    }
    catch (Poco::Exception& e)
    {
        PyErr_SetString(PyExc_RuntimeError,
                e.displayText().c_str());
        return NULL;
    }

	Py_RETURN_NONE;
}

extern "C" PyObject* pyBreakerRelease(BreakerMembers* self)
{
	self->breaker->releaseBreaks();

	Py_RETURN_NONE;
}

#endif /* HAVE_PYTHON27 */
