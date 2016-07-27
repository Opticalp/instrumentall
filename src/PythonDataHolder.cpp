/**
 * @file	src/PythonDataHolder.cpp
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

#include "PythonDataHolder.h"

extern "C" void pyDataHolderDealloc(DataHolderMembers* self)
{
    delete self->dupSource;

    self->ob_type->tp_free((PyObject*) self); // free the object’s memory
}

extern "C" PyObject* pyDataHolderNew(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    DataHolderMembers* self;

    self = (DataHolderMembers*) type->tp_alloc(type, 0);
    if (self != NULL)
        self->dupSource = NULL;

    return (PyObject *) self;
}

#include "PythonOutPort.h" // source
#include "PythonInPort.h" // target

#include "DataSource.h"
#include "DataTarget.h"

extern "C" int pyDataHolderInit(DataHolderMembers* self, PyObject* args, PyObject *kwds)
{
	PyObject *pySourceObj;
	PyObject *pyTargetObj = NULL;

	if (!PyArg_ParseTuple(args, "O|O", &pySourceObj, &pyTargetObj))
		return -1;

	DataSource* source;
	DataTarget* target;

    // check if the Source object is a data source
    // the comparison uses type name (str)
    std::string typeName(pySourceObj->ob_type->tp_name);

    if (typeName.compare("instru.OutPort") == 0)
    {
    	source = **reinterpret_cast<OutPortMembers*>(pySourceObj)->outPort;
    }
    else
    {
        PyErr_SetString(PyExc_TypeError,
                "The first argument must be a data source (output port)");
        return -1;
    }

    try
    {
		if (pyTargetObj == NULL)
		{
			self->dupSource = new DuplicatedSource(source);
			return 0;
		}
    }
    catch (Poco::Exception& e)
    {
    	PyErr_SetString(PyExc_RuntimeError, e.displayText().c_str());
    	return -1;
    }

    // check if the Target object is a data target
    // the comparison uses type name (str)
    typeName = std::string(pyTargetObj->ob_type->tp_name);

    if (typeName.compare("instru.InPort") == 0)
    {
    	target = **reinterpret_cast<InPortMembers*>(pyTargetObj)->inPort;
    }
    else
    {
        PyErr_SetString(PyExc_TypeError,
                "The second argument must be a data target "
                "(input port, data logger not supported)");
        return -1;
    }

    try
    {
		self->dupSource = new DuplicatedSource(source, target);
	}
	catch (Poco::Exception& e)
	{
		PyErr_SetString(PyExc_RuntimeError, e.displayText().c_str());
		return -1;
	}

	return 0;
}

extern "C" PyObject* pyDataHolderTrigTargets(DataHolderMembers* self)
{
	if (self->dupSource)
	{
		self->dupSource->trigTargets();
	}
	else
	{
        PyErr_SetString(PyExc_RuntimeError,
                "Invalid DataHolder object");
        return NULL;
	}

	Py_RETURN_NONE;
}


#endif /* HAVE_PYTHON27 */
