/**
 * @file	src/UI/PythonManager.cpp
 * @date	nov. 2015
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

#include "python/PythonAPI.h"

#include "Poco/String.h" // trim
#include "Poco/File.h"

#include "core/ThreadManager.h"

#include "python/PythonRedirection.h"
#include "python/ScopedGIL.h"
#include "python/PyThreadKeeper.h"

#include "PythonManager.h"

#include <stdio.h>

/// @name Custom config keys
///@{
#define CONF_KEY_PY_SCRIPT         "python.script"
#define CONF_KEY_PY_INITSCRIPT     "python.initScript"
///@}

using Poco::Util::Application;

PythonManager::PythonManager():
	VerboseEntity(name()),
	iconsoleFlag(false),
	pyMultiThread()
{
	// nothing to do yet
}

PythonManager::~PythonManager()
{
    if (isInit())
        uninitialize();
}


/// Convenience function to forward an object member function
void stderrForwarder(const char* message)
{
    std::string errorMsg = Poco::trim(std::string(message));

    if (errorMsg.length())
    {
        Application::instance()
            .getSubsystem<PythonManager>()
            .errorMessage(errorMsg);
    }
}

void PythonManager::initialize(Application& app)
{
    setLogger(name());

    _addedVarStore.clear();

    // -- initialize the python bindings ---
    std::string commandName = app.config().getString("application.argv[0]");
    Py_SetProgramName(const_cast<char*>(commandName.c_str()));

    PyEval_InitThreads();

    Py_Initialize();

    const int argc = 1;
    char * argv[argc];
    argv[0] = const_cast<char*>(commandName.c_str());
    PySys_SetArgv(argc, argv);

    setPyStderrForwarder(&stderrForwarder);

    exposeAPI();

    pyMultiThread = new PyThreadKeeper();

    if (app.config().hasProperty(CONF_KEY_PY_INITSCRIPT))
    {
        runScript(app, app.config().getString(CONF_KEY_PY_INITSCRIPT));
    }
}

void PythonManager::uninitialize()
{
    poco_information(logger(), "Python manager uninitializing...");

    try
    {
        checkInit();
    }
    catch (Poco::Exception&)
    {
        poco_error(logger(), "Trying to uninit the Python Manager "
                "that is not initialized");
    }

    delete pyMultiThread;
    poco_information(logger(), "Python main thread restored");

    // purge added variables
    std::vector<_varItem> varStoreCopy(_addedVarStore);

    for( std::vector<_varItem>::iterator
            it = varStoreCopy.begin(),
           ite = varStoreCopy.end();
         it != ite;
         ++it )
    {
        try
        {
            delVar(it->varName.c_str(),it->modName.c_str());
        }
        catch (Poco::Exception& e)
        {
            poco_error(logger(),e.displayText());
        }
    }
    _addedVarStore.clear();

    Py_Finalize();

    poco_information(logger(), "Python manager uninitialized. ");
}

int PythonManager::main(Application& app)
{
    if ( iconsoleFlag
        && app.config().hasProperty(CONF_KEY_PY_SCRIPT)  )
    {
        poco_warning(logger(), "The \"iconsole\" command line option "
                "takes precedence over the script defined by the "
                "command line \"execute\" option, or by the "
                + std::string(CONF_KEY_PY_SCRIPT)
                + " key in the config file");
    }

    if (iconsoleFlag)
    {
        runConsole();
    }
    else if (app.config().hasProperty(CONF_KEY_PY_SCRIPT))
    {
        Poco::Util::Application::instance().getSubsystem<ThreadManager>().startWatchDog();
        runScript(app, Poco::Path(app.config().getString(CONF_KEY_PY_SCRIPT)));
        Poco::Util::Application::instance().getSubsystem<ThreadManager>().waitAll();
    }

    return Application::EXIT_OK;
}

bool PythonManager::requestFullControl()
{
    Poco::Util::Application *pApp = &Poco::Util::Application::instance();

    return ( iconsoleFlag || pApp->config().hasProperty(CONF_KEY_PY_SCRIPT)  );
}

void PythonManager::runScript(Poco::Path filePath)
{
    runScript(Poco::Util::Application::instance(), filePath);
}

void PythonManager::errorMessage(std::string errorMsg)
{
    poco_error(logger(), errorMsg);
}


#include <fstream>
#include <streambuf>

void PythonManager::runScript(Poco::Util::Application& app, Poco::Path scriptFile)
{
    checkInit();

    Poco::Path stdPath("python");
    stdPath.append("scripts");

    // first check in python/scripts if the script is present
    if (scriptFile.isRelative())
    {
        if (scriptFile.isFile()
                && Poco::File(scriptFile).exists()
                && Poco::File(scriptFile).canRead() )
        {
            poco_information(logger(),
                    scriptFile.toString()
                    + " script found in the working directory. " );
        }
        else
        {
            Poco::Path tmpPath(app.config().getString("application.dir"));
            tmpPath.append(stdPath);
            tmpPath.append(scriptFile);

            if ( tmpPath.isFile()
                    && Poco::File(tmpPath).exists()
                    && Poco::File(tmpPath).canRead() )
            {
                poco_information(logger(),
                        scriptFile.toString()
                        + " script found in standard path "
                        + stdPath.toString() );
                scriptFile = tmpPath;
            }
            else
            {
                Poco::Exception e("runScript","unable to read the script: "
                    + scriptFile.toString());
                poco_error(logger(),e.displayText());
                throw e;
            }
        }
    }

    scriptFile.makeAbsolute();

    ScopedGIL GIL;

    PyObject *py_main, *py_global, *py_local;
    py_main = PyImport_AddModule("__main__");
    py_global = PyModule_GetDict(py_main);
    py_local = PyDict_New();

    PyObject* exitFct = PySys_GetObject(const_cast<char*>("exit"));
    PyDict_SetItemString(py_global, "exit", exitFct);
    PyObject* pyScript = PyString_FromString(scriptFile.toString().c_str());
    PyDict_SetItemString(py_local, "__file__", pyScript);

    // transfer the script content into a string
    std::ifstream ifs(scriptFile.toString().c_str());

    ifs.seekg(0, std::ios::end);
    std::vector<char> scriptBuf(ifs.tellg());
    ifs.seekg(0, std::ios::beg);

    scriptBuf.assign((std::istreambuf_iterator<char>(ifs)),
                std::istreambuf_iterator<char>());

    scriptBuf.push_back('\0');

    PyCodeObject* compiledStr = reinterpret_cast<PyCodeObject*>(
            Py_CompileString(&(scriptBuf[0]), scriptFile.getFileName().c_str(), Py_file_input) );

    // launch the string using a python command
    if ((compiledStr==NULL) || (PyEval_EvalCode(compiledStr, py_global, py_local)==NULL))
    {
        // check for system exit exception
        if (PyErr_Occurred())
        {
            if (PyErr_ExceptionMatches(PyExc_SystemExit))
            {
                poco_information(logger(),
                        "Script leaved on a SystemExit exception");
                PyErr_Clear();
            }
            else if (PyErr_ExceptionMatches(PyExc_RuntimeError))
            {
                Poco::RuntimeException e("runScript","The script exited on RuntimeError");
                poco_error(logger(),e.displayText());
                PyErr_Print();
                throw  e;
            }
            else
            {
            	Poco::UnhandledException e("runScript",
            			"The script exited on unhandled error. ");
                poco_error(logger(), e.displayText());
                PyErr_Print();
                throw  e;
            }
        }
        else
        {
            Poco::Exception e("runScript","unable to run the script: "
                + scriptFile.toString());
            poco_error(logger(),e.displayText());
            PyErr_Print();
            throw e;
        }
    }

    poco_information(logger(), "python script gracefully executed");
}

void PythonManager::runConsole()
{
    poco_information(logger(), "Launching the console... ");

    ScopedGIL GIL;

    PyObject *py_main, *py_global, *py_local;
    py_main = PyImport_AddModule("__main__");
    py_global = PyModule_GetDict(py_main);
    py_local = PyDict_New();

    PyObject* exitFct = PySys_GetObject(const_cast<char*>("exit"));
    PyDict_SetItemString(py_global, "exit", exitFct);

    char consoleScript[] =
        "class Quitter(object): \n"
        "    def __init__(self, name): \n"
        "        self.name = name \n"
        "    def __repr__(self): \n"
        "        return 'Use %s() to exit' % (self.name) \n"
        "    def __call__(self, code=None): \n"
        "        raise SystemExit(code) \n"

        "import code \n"

        "# dictionary definition to transmit local variables \n"
        "dico = {} \n"
        "dico['quit']=Quitter('quit') \n"
        "dico['exit']=Quitter('exit') \n"

        "import instru \n"
        "print('\"instru\" module loaded...') \n"
        "dico['instru']=instru \n"

        "# interactive console launch \n"
        "try: \n"
        "    code.interact(local=dico) \n"
        "except SystemExit: \n"
        "    print('Interactive console left on SystemExit.') \n";

    PyCodeObject* compiledStr = reinterpret_cast<PyCodeObject*>(
            Py_CompileString(consoleScript, "console", Py_file_input) );

    // launch the string using a python command
    if ((compiledStr==NULL) || (PyEval_EvalCode(compiledStr, py_global, py_local)==NULL))
    {
        // check for system exit exception
        if (PyErr_Occurred())
        {
            if (PyErr_ExceptionMatches(PyExc_SystemExit))
            {
                poco_information(logger(),
                        "Console script leaved on a SystemExit exception");
                PyErr_Clear();
            }
            else if (PyErr_ExceptionMatches(PyExc_RuntimeError))
            {
                Poco::RuntimeException e("runConsole","The console exited on RuntimeError");
                poco_error(logger(),e.displayText());
                PyErr_Print();
                throw  e;
            }
            else
            {
                Poco::UnhandledException e("runConsole",
                        "The console exited on unhandled error. ");
                poco_error(logger(), e.displayText());
                PyErr_Print();
                throw  e;
            }
        }
        else
        {
            Poco::Exception e("runConsole","unable to run the console script");
            poco_error(logger(),e.displayText());
            PyErr_Print();
            throw e;
        }
    }
}

void PythonManager::checkInit()
{
    if (!Py_IsInitialized())
        throw Poco::Exception("PythonManager::runScript()",
                "The Python interpreter is not initialized. ");
}

bool PythonManager::isInit()
{
    return Py_IsInitialized();
}

void PythonManager::setVar(double value, const char* name, const char* module)
{
    // get module to load into
    PyObject *m = PyImport_AddModule(module);

    bool found = false;

    // check that it not already exists
    if (PyObject_HasAttrString(m,name))
    {
        // then check if the var is in the _addedVarStore
        for( std::vector<_varItem>::iterator
                it = _addedVarStore.begin(),
               ite = _addedVarStore.end();
             it != ite;
             ++it )
        {
            if (it->varName.compare(name)==0 && it->modName.compare(module)==0)
            {
                poco_information( logger(),
                        std::string("python var > overriding: ")
                        + name );
                found = true;
                break;
            }
        }

        if (!found)
            throw Poco::Exception(std::string("var: \"")
                    + name + "\" already exists");
    }

    // else set / overrides
    PyObject *var = PyFloat_FromDouble(value);
    if (PyObject_SetAttrString(m,name,var)<0)
    {
        throw Poco::Exception(
                std::string("error when setting the new double variable: ")
                + name + " in " + module);
    }
    else if (!found)
    {
        _varItem myVar;
        myVar.varName = name;
        myVar.modName = module;
        _addedVarStore.push_back(myVar);
    }
}

void PythonManager::setVar(long value, const char* name, const char* module)
{
    // get module to load into
    PyObject *m = PyImport_AddModule(module);

    bool found = false;

    // check that it not already exists
    if (PyObject_HasAttrString(m,name))
    {
        // then check if the var is in the _addedVarStore
        for( std::vector<_varItem>::iterator
                it = _addedVarStore.begin(),
               ite = _addedVarStore.end();
             it != ite;
             ++it )
        {
            if (it->varName.compare(name)==0 && it->modName.compare(module)==0)
            {
                poco_information( logger(),
                        std::string("python var > overriding: ")
                        + name );
                found = true;
                break;
            }
        }

        if (!found)
            throw Poco::Exception(std::string("var: \"")
                    + name + "\" already exists");
    }

    // else set / overrides
    PyObject *var = PyLong_FromLong(value);
    if (PyObject_SetAttrString(m,name,var)<0)
    {
        throw Poco::Exception(
                std::string("error when setting the new double variable: ")
                + name + " in " + module);
    }
    else if (!found)
    {
        _varItem myVar;
        myVar.varName = name;
        myVar.modName = module;
        _addedVarStore.push_back(myVar);
    }
}

void PythonManager::setVar(const char* value, const char* name, const char* module)
{
    // get module to load into
    PyObject *m = PyImport_AddModule(module);

    bool found = false;

    // check that it not already exists
    if (PyObject_HasAttrString(m,name))
    {
        // then check if the var is in the _addedVarStore
        for( std::vector<_varItem>::iterator
                it = _addedVarStore.begin(),
               ite = _addedVarStore.end();
             it != ite;
             ++it )
        {
            if (it->varName.compare(name)==0 && it->modName.compare(module)==0)
            {
                poco_information( logger(),
                        std::string("python var > overriding: ")
                        + name );
                found = true;
                break;
            }
        }

        if (!found)
            throw Poco::Exception(std::string("var: \"")
                    + name + "\" already exists");
    }

    // else set / overrides
    PyObject *var = PyString_FromString(value);
    if (PyObject_SetAttrString(m,name,var)<0)
    {
        throw Poco::Exception(
                std::string("error when setting the new double variable: ")
                + name + " in " + module);
    }
    else if (!found)
    {
        _varItem myVar;
        myVar.varName = name;
        myVar.modName = module;
        _addedVarStore.push_back(myVar);
    }
}

void PythonManager::getVar(
        double& value,
        const char* name,
        const char* module)
{
    // get module to load into
    PyObject *m = PyImport_AddModule(module);

    // check that it not already exists
    if (!PyObject_HasAttrString(m,name))
        throw Poco::Exception(std::string("getVar: no var ")
            + name + " in " + module);

    PyObject *obj = PyObject_GetAttrString(m,name);
    if (!PyFloat_Check(obj))
        throw Poco::Exception(std::string("getVar: var ")
            + name + " in " + module
            + " is not a float. ");

    value = PyFloat_AsDouble(obj);
}

void PythonManager::getVar(
        long& value,
        const char* name,
        const char* module)
{
    // get module to load into
    PyObject *m = PyImport_AddModule(module);

    // check that it not already exists
    if (!PyObject_HasAttrString(m,name))
        throw Poco::Exception(std::string("getVar: no var ")
            + name + " in " + module);

    PyObject *obj = PyObject_GetAttrString(m,name);
    if (!PyLong_Check(obj))
        throw Poco::Exception(std::string("getVar: var ")
            + name + " in " + module
            + " is not an integer. ");

    value = PyLong_AsLong(obj);
}

void PythonManager::getVar(
        std::string& value,
        const char* name,
        const char* module)
{
    // get module to load into
    PyObject *m = PyImport_AddModule(module);

    // check that it not already exists
    if (!PyObject_HasAttrString(m,name))
        throw Poco::Exception(std::string("getVar: no var ")
            + name + " in " + module);

    PyObject *obj = PyObject_GetAttrString(m,name);
    if (!PyString_Check(obj))
        throw Poco::Exception(std::string("getVar: var ")
            + name + " in " + module
            + " is not a string. ");

    value = std::string(PyString_AsString(obj));
}

void PythonManager::delVar(const char* name, const char* module)
{
    // get module to delete from
    PyObject *m = PyImport_AddModule(module);

    // check if the var is in the _addedVarStore
    for( std::vector<_varItem>::iterator
            it = _addedVarStore.begin();
            it != _addedVarStore.end();
            ++it )
    {
        if (it->varName.compare(name)==0 && it->modName.compare(module)==0)
        {
            // then deletes
            if (PyObject_DelAttrString(m,name)<0)
            {
                poco_error(logger(),
                    std::string("python var > error deleting var: ")
                    + name );
                return;
            }
            else
            {
                poco_information(logger(),
                    std::string("python var: ")
                    + name + " is deleted");
                it = _addedVarStore.erase(it);
                return;
            }
        }
    }
    poco_warning(logger(),std::string("deleting: python var ")
        + module + '.' + name + " not found");
}

using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::OptionCallback;

void PythonManager::defineOptions(OptionSet & options)
{
    options.addOption(
        Option(
                "iconsole", "i",
                "open an interactive python console" )
            .required(false)
            .repeatable(false)
            .callback(OptionCallback<PythonManager>(
                                this, &PythonManager::handleIConsole) )
            .group("interface"));

    options.addOption(
        Option(
                "execute", "x",
                "execute the given python script: SCRIPT, then exit" )
            .required(false)
            .repeatable(false)
            .argument("SCRIPT")
            .binding(CONF_KEY_PY_SCRIPT)
            .group("interface"));

    options.addOption(
        Option(
                "initscript", "",
                "execute the specified python script: INITSCRIPT, "
                "before launching any other interface" )
            .required(false)
            .repeatable(false)
            .argument("INITSCRIPT")
            .binding(CONF_KEY_PY_INITSCRIPT));
}

#endif /* HAVE_PYTHON27 */
