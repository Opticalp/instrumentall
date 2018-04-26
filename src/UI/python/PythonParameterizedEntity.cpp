/**
 * @file	src/UI/pythonPythonParameterizedEntity.cpp
 * @date	Jan. 2017
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2017 Ph. Renaud-Goud / Opticalp

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

#include "PythonParameterizedEntity.h"

PyObject* pyGetParameterSet(ParameterizedEntity* pEntity)
{
    ParameterSet paramSet;

    pEntity->getParameterSet(&paramSet);

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

PyObject* pyGetParameterValue(ParameterizedEntity* pEntity, PyObject *args)
{
    char *charParamName;

    if (!PyArg_ParseTuple(args, "s:getParameterValue", &charParamName))
        return NULL;

    std::string paramName(charParamName);

    try
    {
        switch (pEntity->getParameterType(paramName))
        {
        case ParamItem::typeInteger:
        {
            long value = static_cast<long>(
                    pEntity->getParameterValue<Poco::Int64>(paramName) );
            return PyLong_FromLong(value);
        }
        case ParamItem::typeFloat:
        {
            double value =
                    pEntity->getParameterValue<double>(paramName);
            return PyFloat_FromDouble(value);
        }
        case ParamItem::typeString:
        {
            std::string value =
                    pEntity->getParameterValue<std::string>(paramName);
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

PyObject* pySetParameterValue(ParameterizedEntity* pEntity, PyObject *args)
{
    char* charParamName;
    PyObject* pyValue;

    if (!PyArg_ParseTuple(args, "sO:setParameterValue", &charParamName, &pyValue))
        return NULL;

    std::string paramName(charParamName);

    try
    {
        switch (pEntity->getParameterType(paramName))
        {
        case ParamItem::typeInteger:
        {
            long value = PyLong_AsLong(pyValue);
            if (PyErr_Occurred()) { return NULL; }
            pEntity->setParameterValue<Poco::Int64>(paramName, value, true);
            break;
        }
        case ParamItem::typeFloat:
        {
            double value = PyFloat_AsDouble(pyValue);
            if (PyErr_Occurred()) { return NULL; }
            pEntity->setParameterValue<double>(paramName, value, true);
            break;
        }
        case ParamItem::typeString:
        {
            std::string value(PyString_AsString(pyValue)) ;
            if (PyErr_Occurred()) { return NULL; }
            pEntity->setParameterValue<std::string>(paramName, value, true);
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

PyObject* pySetParameterValues(ParameterizedEntity* pEntity, PyObject* args)
{
    PyObject* pyArg;

    if (!PyArg_ParseTuple(args, "O:setParameterValues", &pyArg))
        return NULL;

    // check that the input argument is a dict
    if (!PyDict_Check(pyArg))
    {
        PyErr_SetString(PyExc_TypeError,
                "A dict input is required (parameter name/value pairs)");
        return NULL;
    }

    try
    {
        PyObject *key, *pyValue;
        Py_ssize_t pos = 0;

        while (PyDict_Next(pyArg, &pos, &key, &pyValue))
        {
            std::string paramName(PyString_AsString(key));

            switch (pEntity->getParameterType(paramName))
            {
            case ParamItem::typeInteger:
            {
                long value = PyLong_AsLong(pyValue);
                if (PyErr_Occurred()) { return NULL; }
                pEntity->setParameterValue<Poco::Int64>(paramName, value, false);
                break;
            }
            case ParamItem::typeFloat:
            {
                double value = PyFloat_AsDouble(pyValue);
                if (PyErr_Occurred()) { return NULL; }
                pEntity->setParameterValue<double>(paramName, value, false);
                break;
            }
            case ParamItem::typeString:
            {
                std::string value(PyString_AsString(pyValue)) ;
                if (PyErr_Occurred()) { return NULL; }
                pEntity->setParameterValue<std::string>(paramName, value, false);
                break;
            }
            default:
                poco_bugcheck_msg("python setParameterValue, "
                        "getParameterType returned unknown data type");
                throw Poco::BugcheckException();
            }
        }

        pEntity->tryApplyParameters(true);
    }
    catch (Poco::Exception& e)
    {
        PyErr_SetString(PyExc_RuntimeError, e.displayText().c_str());
        return NULL;
    }

    Py_RETURN_NONE;
}

#endif /* HAVE_PYTHON27 */
