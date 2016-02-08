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

extern "C" PyObject*
pythonModManGetRootFact(PyObject *self, PyObject *args)
{
    std::vector<std::string> strFactories;

    strFactories = Poco::Util::Application::instance()
                .getSubsystem<ModuleManager>()
                .getRootFactories();

    // construct the list to return
    PyObject* pyFactories = PyList_New(0);

    for (std::vector<std::string>::iterator it=strFactories.begin(), ite=strFactories.end();
            it != ite; it++ )
    {
        // create the dict entry
        if (0 > PyList_Append(
                pyFactories,
                PyString_FromString(it->c_str())))
        {
            // appending the item failed
            PyErr_SetString(PyExc_RuntimeError,
                    "Not able to build the return list");
            return NULL;
        }
    }

    return pyFactories;
}


#endif /* HAVE_PYTHON27 */
