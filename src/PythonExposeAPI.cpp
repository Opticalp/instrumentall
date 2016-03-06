/**
 * @file	src/PythonExposeAPI.cpp
 * @date	nov. 2015
 * @author	PhRG - opticalp.fr
 *
 * @brief C to Python exposition
 * 
 * This file encloses the PythonManager::exposeAPI() implementation
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

#include "PythonAPI.h"

#include "PythonRedirection.h"
#include "PythonMainAppExport.h"
#include "PythonManager.h"

#include "PythonModuleFactory.h"
#include "PythonModule.h"
#include "PythonInPort.h"
#include "PythonOutPort.h"
#include "PythonData.h"
#include "PythonDataLogger.h"

/**
 * array to bind to-be-exposed methods (C to Python wrappers)
 */
static PyMethodDef EmbMethods[] =
{
    // misc
    pyMethodMainAppAbout,
    pyMethodMainAppVersion,

    // module manager
    pyMethodModManGetRootFact,
    pyMethodModManGetModules,

    // dispatcher
    pyMethodDispatchBind,
    pyMethodDispatchUnbind,

    pyMethodDispatchSeqBind,
    pyMethodDispatchSeqUnbind,

    pyMethodDispatchRunModule,

    // thread manager
    pyMethodThreadManWaitAll,

    // data manager
    pyMethodDataManDataLoggerClasses,
    pyMethodDataManDataLoggers,
    pyMethodDataManRemoveDataLogger,

    // sentinel
    {NULL, NULL, 0, NULL}
};


void PythonManager::exposeAPI()
{
    // stderr redirection
    PyObject *merr = Py_InitModule("fwerr", stderr_methods);
    if (merr == NULL)
        throw Poco::Exception("unable to redirect stderr");
    PySys_SetObject(const_cast<char*>("stderr"), merr);

    // adding python dir to PYTHONPATH
    PyObject* pyPath = PySys_GetObject(const_cast<char *>("path"));
    if (!PyList_Check(pyPath))
        poco_bugcheck_msg("unable to get python's sys.path");

    Poco::Path appDir( Poco::Util::Application::instance()
                            .config()
                            .getString("application.dir")  );
    appDir.append("python");
    appDir.append("embed");

    poco_information(logger(),
            appDir.toString()
            + " will be added to the python path");

    PyList_Append( pyPath,
            PyString_FromString(
                    const_cast<const char *>(appDir.toString().c_str())));

    // module types initialization
    if (PyType_Ready(&PythonModuleFactory) < 0)
        return;

    if (PyType_Ready(&PythonModule) < 0)
        return;

    if (PyType_Ready(&PythonInPort) < 0)
        return;

    if (PyType_Ready(&PythonOutPort) < 0)
        return;

    if (PyType_Ready(&PythonData) < 0)
        return;

    if (PyType_Ready(&PythonDataLogger) < 0)
        return;

    PyObject* m;

    m = Py_InitModule3("instru", EmbMethods,
        "Module that enables to control InstrumentAll");

    if (m == NULL)
    {
        Poco::Exception e("exposeAPI","unable to initialize \"instru\" module");
        throw e;
    }

    Py_INCREF(&PythonModuleFactory);
    PyModule_AddObject(m, "Factory", (PyObject *)&PythonModuleFactory);

    Py_INCREF(&PythonModule);
    PyModule_AddObject(m, "Module", (PyObject *)&PythonModule);

    Py_INCREF(&PythonInPort);
    PyModule_AddObject(m, "InPort", (PyObject *)&PythonInPort);

    Py_INCREF(&PythonOutPort);
    PyModule_AddObject(m, "OutPort", (PyObject *)&PythonOutPort);

    Py_INCREF(&PythonData);
    PyModule_AddObject(m, "Data", (PyObject *)&PythonData);

    Py_INCREF(&PythonDataLogger);
    PyModule_AddObject(m, "DataLogger", (PyObject *)&PythonDataLogger);
}





#endif /* HAVE_PYTHON27 */
