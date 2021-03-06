/**
 * @file	src/UI/python/PythonMainAppExport.h
 * @date	dec. 2015
 * @author	PhRG - opticalp.fr
 * 
 * @brief Python exposition of methods of MainApplication class
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


#ifndef SRC_PYTHONMAINAPPEXPORT_H_
#define SRC_PYTHONMAINAPPEXPORT_H_

#ifdef HAVE_PYTHON27

#include "PythonAPI.h"

/**
 * @brief Python wrapper to retrieve the "about" information
 *
 * Call MainApplication::about() method
 *
 */
extern "C" PyObject*
pythonMainAppAbout(PyObject *self, PyObject *args);

static PyMethodDef pyMethodMainAppAbout =
{
    "about",
    pythonMainAppAbout,
    METH_NOARGS,
    "Retrieve general verbose information about the current application"
};

/**
 * @brief Python wrapper to retrieve the version information
 *
 * Call MainApplication::version() method
 *
 */
extern "C" PyObject*
pythonMainAppVersion(PyObject *self, PyObject *args);

static PyMethodDef pyMethodMainAppVersion =
{
    "version",
    pythonMainAppVersion,
    METH_NOARGS,
    "Retrieve version information of the current application"
};

/**
 * @brief Python wrapper to load a configuration file
 *
 * Call MainApplication::loadConfiguration() method
 * inherited from Poco::Application::loadConfiguration()
 *
 */
extern "C" PyObject*
pythonMainAppLoadConfig(PyObject *self, PyObject *args);

static PyMethodDef pyMethodMainAppLoadConfig =
{
    "loadConfiguration",
    pythonMainAppLoadConfig,
    METH_VARARGS,
    "Load a configuration file. "
    "Any default Poco format config file "
    "is supported. "
};


// ----------------------------------------------------------------
//     Module Manager
// ----------------------------------------------------------------

/**
 * @brief Python wrapper to retrieve the list of root factories
 *
 * Call ModuleManager::getRootFactories() method
 *
 */
extern "C" PyObject*
pythonModManGetRootFact(PyObject *self, PyObject *args);

static PyMethodDef pyMethodModManGetRootFact =
{
    "getRootFactories",
    pythonModManGetRootFact,
    METH_NOARGS,
    "Retrieve the list of root factories"
};

/**
 * @brief Python wrapper to retrieve the list of modules
 *
 * Call ModuleManager::getModules() method
 *
 */
extern "C" PyObject*
pythonModManGetModules(PyObject *self, PyObject *args);

static PyMethodDef pyMethodModManGetModules =
{
    "getModules",
    pythonModManGetModules,
    METH_NOARGS,
    "Retrieve the list of modules"
};

/**
 * @brief Python wrapper to clear all the modules
 *
 * Call ModuleManager::clearModules() method
 *
 */
extern "C" PyObject*
pythonModManClearModules(PyObject *self, PyObject *args);

static PyMethodDef pyMethodModManClearModules =
{
    "clearModules",
    pythonModManClearModules,
    METH_NOARGS,
    "Clear all the modules"
};

/**
 * Python wrapper to export the workflow as a graphviz dot graph
 *
 * Call ModuleManager::exportWFGraphviz
 */
extern "C" PyObject*
pythonModManExportWFGraphviz(PyObject *self, PyObject *args);

static PyMethodDef pyMethodModManExportWFGraphviz =
{
    "exportWorkflow",
    pythonModManExportWFGraphviz,
    METH_VARARGS,
    "Export the workflow as a graphviz dot graph. arg: file path"
};

/**
 * Python wrapper to export the factories tree as a graphviz dot graph
 *
 * Call ModuleManager::exportFacTreeGraphviz
 */
extern "C" PyObject*
pythonModManExportFacTreeGraphviz(PyObject *self, PyObject *args);

static PyMethodDef pyMethodModManExportFacTreeGraphviz =
{
    "exportFactoriesTree",
    pythonModManExportFacTreeGraphviz,
    METH_VARARGS,
    "Export the factories tree as a graphviz dot graph. arg: file path"
};

// ----------------------------------------------------------------
//     Dispatcher
// ----------------------------------------------------------------

/**
 * @brief Python wrapper to unplug all the modules
 *
 * Call Dispatcher::resetWorkflow method
 *
 */
extern "C" PyObject*
pythonDispatchResetWorkflow(PyObject *self, PyObject *args);

static PyMethodDef pyMethodDispatchResetWorkflow =
{
    "resetWorkflow",
    pythonDispatchResetWorkflow,
    METH_NOARGS,
    "Unbind all the workflow connexions. "
};

/**
 * @brief Python wrapper to bind ports
 *
 * Call Dispatcher::bind(portA, portB) method
 *
 */
extern "C" PyObject*
pythonDispatchBind(PyObject *self, PyObject *args);

static PyMethodDef pyMethodDispatchBind =
{
    "bind",
    pythonDispatchBind,
    METH_VARARGS,
    "bind(source, target): Bind a data source (e.g. output port) "
    "to a data target (e.g. input port). \n"
    "bind(source, target, proxy): Bind source to target using the "
    "given data proxy."
};

/**
 * @brief Python wrapper to unbind ports
 *
 * Call Dispatcher::unbind(inPortA) method
 *
 */
extern "C" PyObject*
pythonDispatchUnbind(PyObject *self, PyObject *args);

static PyMethodDef pyMethodDispatchUnbind =
{
    "unbind",
    pythonDispatchUnbind,
    METH_VARARGS,
    "unbind(inPortA): unbind a data input target port inPortA "
    "from its source"
};

/**
 * @brief Python wrapper to bind ports for data sequences
 *
 * Call Dispatcher::seqBind(portA, portB) method
 *
 */
extern "C" PyObject*
pythonDispatchSeqBind(PyObject *self, PyObject *args);

static PyMethodDef pyMethodDispatchSeqBind =
{
    "seqBind",
    pythonDispatchSeqBind,
    METH_VARARGS,
    "seqBind(portA, portB): Bind a data output sequence generator source port portA "
    "to a data input sequence combiner target port portB"
};

/**
 * @brief Python wrapper to unbind ports for data sequences
 *
 * Call Dispatcher::seqUnbind(inPortA) method
 *
 */
extern "C" PyObject*
pythonDispatchSeqUnbind(PyObject *self, PyObject *args);

static PyMethodDef pyMethodDispatchSeqUnbind =
{
    "seqUnbind",
    pythonDispatchSeqUnbind,
    METH_VARARGS,
    "unbind(inPortA): unbind a data input sequence combiner target port inPortA "
    "from its sequence source"
};

/**
 * Python wrapper to launch a Module::runTask()
 *
 * This is done via the dispatcher that creates a thread.
 */
extern "C" PyObject*
pythonDispatchRunModule(PyObject *self, PyObject *args);

static PyMethodDef pyMethodDispatchRunModule =
{
    "runModule",
    pythonDispatchRunModule,
    METH_VARARGS,
    "runModule(module1): Launch module1. Returns the task encapsulating this run. "
};

// ----------------------------------------------------------------
//     Thread Manager
// ----------------------------------------------------------------

/**
 * @brief Python wrapper to wait for all tasks to be terminated
 *
 * Call ThreadManager::waitAll() method
 *
 */
extern "C" PyObject*
pythonThreadManWaitAll(PyObject *self, PyObject *args);

static PyMethodDef pyMethodThreadManWaitAll =
{
    "waitAll",
    pythonThreadManWaitAll,
    METH_NOARGS,
    "Wait for all the tasks to be terminated"
};

/**
 * @brief Python wrapper to cancel all the tasks
 *
 * Call ThreadManager::cancelAll() method
 *
 */
extern "C" PyObject*
pythonThreadManCancelAll(PyObject *self, PyObject *args);

static PyMethodDef pyMethodThreadManCancelAll =
{
    "cancelAll",
    pythonThreadManCancelAll,
    METH_NOARGS,
    "Cancel all the tasks, running or pending. "
};

/**
 * @brief Python wrapper to cancel all the tasks
 *
 * Call ThreadManager::cancelAllNoWait() method
 *
 */
extern "C" PyObject*
pythonThreadManCancelAllNoWait(PyObject *self, PyObject *args);

static PyMethodDef pyMethodThreadManCancelAllNoWait =
{
    "cancelAllNoWait",
    pythonThreadManCancelAllNoWait,
    METH_NOARGS,
    "Cancel all the tasks, running or pending. "
    "Do not wait for the tasks being terminated. "
    "Can be used inside a python module. "
};


/**
 * @brief Python wrapper to stop the watch dog
 *
 * Call ThreadManager::stopWatchDog() method
 *
 */
extern "C" PyObject*
pythonThreadManStopWatchDog(PyObject *self, PyObject *args);

static PyMethodDef pyMethodThreadManStopWatchDog =
{
    "stopWatchDog",
    pythonThreadManStopWatchDog,
    METH_NOARGS,
    "Disable the watch dog that checks if a module is frozen. "
};

// ----------------------------------------------------------------
//     Data Manager
// ----------------------------------------------------------------

/**
 * @brief Python wrapper to retrieve the DataLogger classes
 *
 * Call DataManager::dataLoggerClasses() method
 *
 */
extern "C" PyObject*
pythonDataManDataLoggerClasses(PyObject *self, PyObject *args);

static PyMethodDef pyMethodDataManDataLoggerClasses =
{
    "dataLoggerClasses",
    pythonDataManDataLoggerClasses,
    METH_NOARGS,
    "Retrieve the names and descriptions "
    "of the available DataLogger classes"
};

/**
 * @brief Python wrapper to delete a data logger
 *
 * Call DataManager::removeDataLogger() method
 *
 */
extern "C" PyObject*
pythonDataManRemoveDataLogger(PyObject *self, PyObject *args);

static PyMethodDef pyMethodDataManRemoveDataLogger =
{
    "removeDataLogger",
    pythonDataManRemoveDataLogger,
    METH_VARARGS,
    "removeDataLogger(logger): delete the data logger \"logger\""
};

/**
 * @brief Python wrapper to retrieve the DataProxy classes
 *
 * Call DataManager::dataProxyClasses() method
 *
 */
extern "C" PyObject*
pythonDataManDataProxyClasses(PyObject *self, PyObject *args);

static PyMethodDef pyMethodDataManDataProxyClasses =
{
    "dataProxyClasses",
    pythonDataManDataProxyClasses,
    METH_NOARGS,
    "Retrieve the names and descriptions "
    "of the available DataProxy classes"
};

#endif /* HAVE_PYTHON27 */
#endif /* SRC_PYTHONMAINAPPEXPORT_H_ */
