/**
 * @file	src/UI/python/PythonMainAppExport.cpp
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

#include "core/MainApplication.h"
#include "core/ModuleManager.h"

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

PyObject*
pythonMainAppLoadConfig(PyObject *self, PyObject *args)
{
    char* charName;

    if (!PyArg_ParseTuple(args, "s:loadConfiguration", &charName))
        return NULL;

    std::string cfgFile(charName);
    try
    {
        Poco::Util::Application::instance().loadConfiguration(cfgFile);
    }
    catch (Poco::Exception& e)
    {
        PyErr_SetString(PyExc_RuntimeError,
                e.displayText().c_str());
        return NULL;
    }

    Py_RETURN_NONE;
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
        pyFact->description = PyString_FromString((**it)->description().c_str());
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
        pyMod->description = PyString_FromString((**it)->description().c_str());
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

extern "C" PyObject*
pythonModManClearModules(PyObject *self, PyObject *args)
{
    try
    {
        Poco::Util::Application::instance()
                .getSubsystem<ModuleManager>()
                .clearModules();
    }
    catch (Poco::Exception& e)
    {
        PyErr_SetString(PyExc_RuntimeError, e.displayText().c_str());
        return NULL;
    }

    Py_RETURN_NONE;
}

PyObject* pythonModManExportWFGraphviz(PyObject* self, PyObject* args)
{
    // convert arg to Poco::Path
	// TODO: use unicode?
    char *charParamName;

    if (!PyArg_ParseTuple(args, "s:exportWorkflow", &charParamName))
    {
        return NULL;
    }
    else
    {
        std::string paramName = charParamName;

        try
        {
            Poco::Util::Application::instance()
                .getSubsystem<ModuleManager>()
                .exportWFGraphviz(Poco::Path(paramName));
        }
        catch (Poco::Exception& e)
        {
            PyErr_SetString( PyExc_RuntimeError,
                    e.displayText().c_str() );
            return NULL;
        }
    }

    Py_RETURN_NONE;
}

PyObject* pythonModManExportFacTreeGraphviz(PyObject* self, PyObject* args)
{
    // convert arg to Poco::Path
    // TODO: use unicode?
    char *charParamName;

    if (!PyArg_ParseTuple(args, "s:exportFactoriesTree", &charParamName))
    {
        return NULL;
    }
    else
    {
        std::string paramName = charParamName;

        try
        {
            Poco::Util::Application::instance()
                .getSubsystem<ModuleManager>()
                .exportFacTreeGraphviz(Poco::Path(paramName));
        }
        catch (Poco::Exception& e)
        {
            PyErr_SetString( PyExc_RuntimeError,
                    e.displayText().c_str() );
            return NULL;
        }
    }

    Py_RETURN_NONE;
}

#include "core/Dispatcher.h"
#include "PythonInPort.h"
#include "PythonOutPort.h"
#include "PythonDataProxy.h"
#include "PythonDataSource.h"
#include "PythonDataTarget.h"

extern "C" PyObject*
pythonDispatchResetWorkflow(PyObject *self, PyObject *args)
{
    try
    {
        Poco::Util::Application::instance()
            .getSubsystem<Dispatcher>()
            .resetWorkflow();
    }
    catch (Poco::Exception& e)
    {
        PyErr_SetString(PyExc_RuntimeError, e.displayText().c_str());
        return NULL;
    }

    Py_RETURN_NONE;
}

extern "C" PyObject*
pythonDispatchBind(PyObject* self, PyObject* args)
{
    PyObject *pySource, *pyTarget;
    PyObject *pyProxy = NULL;

    // arguments parsing
    if (!PyArg_ParseTuple(args, "OO|O:bind", &pySource, &pyTarget, &pyProxy))
        return NULL;

    // check the type of the object.
    // the comparison uses type name (str)
    std::string typeNameSource(pySource->ob_type->tp_name);
    std::string typeNameTarget(pyTarget->ob_type->tp_name);

    DataSource* source;
    DataTarget* target;

    if (typeNameSource.compare("instru.OutPort") == 0)
    {
        OutPortMembers* pyOutPort = reinterpret_cast<OutPortMembers*>(pySource);
        source = *pyOutPort->outPort->get();
    }
    else if (typeNameSource.compare("instru.DataSource") == 0)
    {
        DataSourceMembers* pyDataSource = reinterpret_cast<DataSourceMembers*>(pySource);
        source = pyDataSource->source;
    }
    else
    {
        PyErr_SetString(PyExc_TypeError,
                "The first argument must be a DataSource");
        return NULL;
    }

    if (typeNameTarget.compare("instru.InPort") == 0)
    {
        InPortMembers* pyInPort = reinterpret_cast<InPortMembers*>(pyTarget);
        target = *pyInPort->inPort->get();
    }
    else if (typeNameTarget.compare("instru.DataTarget") == 0)
    {
        DataTargetMembers* pyDataTarget = reinterpret_cast<DataTargetMembers*>(pyTarget);
        target = pyDataTarget->target;
    }
    else
    {
        PyErr_SetString(PyExc_TypeError,
                "The second argument must be a DataTarget");
        return NULL;
    }

    if (pyProxy)
    {
    	std::string typeName(pyProxy->ob_type->tp_name);
    	if (typeName.compare("instru.DataProxy"))
    	{
            PyErr_SetString(PyExc_TypeError,
                    "The third argument must be a DataProxy");
            return NULL;
    	}
    }

    try
    {
    	if (pyProxy == NULL)
    	{
			Poco::Util::Application::instance()
				.getSubsystem<Dispatcher>()
				.bind(source, target);
    	}
    	else
    	{
    		DataProxyMembers* proxy = reinterpret_cast<DataProxyMembers*>(pyProxy);

			Poco::Util::Application::instance()
				.getSubsystem<Dispatcher>()
				.bind(source, *proxy->proxy);

			Poco::Util::Application::instance()
				.getSubsystem<Dispatcher>()
				.bind(*proxy->proxy, target);
    	}
    }
    catch (Poco::Exception& e)
    {
        PyErr_SetString(PyExc_RuntimeError, e.displayText().c_str());
        return NULL;
    }

    Py_RETURN_NONE;
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

    DataTarget* target;

    if (typeName.compare("instru.InPort") == 0)
    {
        InPortMembers* pyInPort = reinterpret_cast<InPortMembers*>(pyObj);
        target = *pyInPort->inPort->get();
    }
    else if (typeName.compare("instru.DataTarget") == 0)
    {
        DataTargetMembers* pyDataTarget = reinterpret_cast<DataTargetMembers*>(pyObj);
        target = pyDataTarget->target;
    }
    else
    {
        PyErr_SetString(PyExc_TypeError,
                "The argument must be a DataTarget");
        return NULL;
    }

    Poco::Util::Application::instance()
        .getSubsystem<Dispatcher>()
        .unbind(target);

    Py_RETURN_NONE;
}

extern "C" PyObject*
pythonDispatchSeqBind(PyObject* self, PyObject* args)
{
    PyObject *pyObj1, *pyObj2;

    // arguments parsing
    if (!PyArg_ParseTuple(args, "OO:seqBind", &pyObj1, &pyObj2))
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
            .seqBind(**pyOutPort->outPort,**pyInPort->inPort);
    }
    catch (Poco::Exception& e)
    {
        PyErr_SetString(PyExc_RuntimeError, e.displayText().c_str());
        return NULL;
    }

    Py_RETURN_NONE;
}

extern "C" PyObject*
pythonDispatchSeqUnbind(PyObject* self, PyObject* args)
{
    PyObject* pyObj;

    // arguments parsing
    if (!PyArg_ParseTuple(args, "O:seqUnbind", &pyObj))
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
        .seqUnbind(**pyPort->inPort);

    Py_RETURN_NONE;
}

#include "PythonTask.h"

extern "C" PyObject*
pythonDispatchRunModule(PyObject *self, PyObject *args)
{
    PyObject* pyObj;

    // arguments parsing
    if (!PyArg_ParseTuple(args, "O:unbind", &pyObj))
        return NULL;

    // check the type of the object.
    // the comparison uses type name (str)
    std::string typeName(pyObj->ob_type->tp_name);

    if (typeName.compare("instru.Module"))
    {
        PyErr_SetString(PyExc_TypeError, "The argument has to be a Module");
        return NULL;
    }

    ModMembers* pyMod = reinterpret_cast<ModMembers*>(pyObj);

    Poco::AutoPtr<ModuleTask> task = (**pyMod->module)->runModule();

    // alloc
    if (PyType_Ready(&PythonTask) < 0)
    {
        PyErr_SetString(PyExc_ImportError,
                "Not able to create the Task Type");
        return NULL;
    }

    TaskMembers* pyTask =
            (TaskMembers*)(pyTaskNew((PyTypeObject*)&PythonTask, NULL, NULL) );

    PyObject* tmp=NULL;

    // init
    // retrieve name and description
    tmp = pyTask->name;
    pyTask->name = PyString_FromString(task->name().c_str());
    Py_XDECREF(tmp);

    // set Module reference
    *(pyTask->task) = task;

    return (PyObject*)(pyTask);
}

#include "core/ThreadManager.h"

extern "C" PyObject*
pythonThreadManWaitAll(PyObject *self, PyObject *args)
{
    PyThreadState* state = PyEval_SaveThread();

	try
	{
		Poco::Util::Application::instance()
				.getSubsystem<ThreadManager>()
				.waitAll();
	}
	catch (Poco::RuntimeException& e)
	{
        PyEval_RestoreThread(state);
        PyErr_SetString(PyExc_RuntimeError,
            ("WaitAll processing error: " + e.displayText()).c_str());
        return NULL;
	}

	PyEval_RestoreThread(state);
	Py_RETURN_NONE;
}

extern "C" PyObject*
pythonThreadManCancelAll(PyObject *self, PyObject *args)
{
    Poco::Util::Application::instance()
            .getSubsystem<ThreadManager>()
            .cancelAll();

    Py_RETURN_NONE;
}

extern "C" PyObject*
pythonThreadManCancelAllNoWait(PyObject *self, PyObject *args)
{
    Poco::Util::Application::instance()
            .getSubsystem<ThreadManager>()
            .cancelAllNoWait();

    Py_RETURN_NONE;
}

PyObject* pythonThreadManStopWatchDog(PyObject* self, PyObject* args)
{
    Poco::Util::Application::instance()
            .getSubsystem<ThreadManager>()
            .stopWatchDog();

    Py_RETURN_NONE;
}


#include "core/DataManager.h"

extern "C" PyObject*
pythonDataManDataLoggerClasses(PyObject *self, PyObject *args)
{
    std::map<std::string, std::string> loggerClasses;

    loggerClasses = Poco::Util::Application::instance()
                        .getSubsystem<DataManager>()
                        .dataLoggerClasses();

    // create a dict
    PyObject* pyLoggerClasses = PyDict_New();

    if (pyLoggerClasses == NULL)
    {
        PyErr_SetString(PyExc_MemoryError, "Not able to create the dict");
        return NULL;
    }

    for (std::map<std::string, std::string>::iterator it = loggerClasses.begin(),
            ite = loggerClasses.end(); it != ite; it++)
    {
        if (-1 == PyDict_SetItemString( pyLoggerClasses,
                it->first.c_str(),
                PyString_FromString(it->second.c_str()) ) )
        {
            PyErr_SetString(PyExc_MemoryError, "Not able to insert a new pair in the dict");
            return NULL;
        }
    }

    return pyLoggerClasses;
}

#include "PythonDataLogger.h"


extern "C" PyObject*
pythonDataManRemoveDataLogger(PyObject *self, PyObject *args)
{
    PyObject *pyObj;

    // arguments parsing
    if (!PyArg_ParseTuple(args, "O:register", &pyObj))
        return NULL;

    // check the type of the object.
    // the comparison uses type name (str)
    std::string typeName(pyObj->ob_type->tp_name);

    if (typeName.compare("instru.DataLogger"))
    {
        PyErr_SetString(PyExc_TypeError,
                "The argument must be a DataLogger");
        return NULL;
    }

    DataLoggerMembers* pyLogger = reinterpret_cast<DataLoggerMembers*>(pyObj);

    Poco::Util::Application::instance()
                            .getSubsystem<Dispatcher>()
                            .unbind(*pyLogger->logger);

    Py_RETURN_NONE;
}

extern "C" PyObject*
pythonDataManDataProxyClasses(PyObject *self, PyObject *args)
{
    std::map<std::string, std::string> proxyClasses;

    proxyClasses = Poco::Util::Application::instance()
                        .getSubsystem<DataManager>()
                        .dataProxyClasses();

    // create a dict
    PyObject* pyProxyClasses = PyDict_New();

    if (pyProxyClasses == NULL)
    {
        PyErr_SetString(PyExc_MemoryError, "Not able to create the dict");
        return NULL;
    }

    for (std::map<std::string, std::string>::iterator it = proxyClasses.begin(),
            ite = proxyClasses.end(); it != ite; it++)
    {
        if (-1 == PyDict_SetItemString( pyProxyClasses,
                it->first.c_str(),
                PyString_FromString(it->second.c_str()) ) )
        {
            PyErr_SetString(PyExc_MemoryError, "Not able to insert a new pair in the dict");
            return NULL;
        }
    }

    return pyProxyClasses;
}

#endif /* HAVE_PYTHON27 */
