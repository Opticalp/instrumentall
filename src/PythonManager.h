/**
 * @file	src/PythonManager.h
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

#ifndef SRC_PYTHONMANAGER_H_
#define SRC_PYTHONMANAGER_H_

#ifdef HAVE_PYTHON27

#include "Poco/Util/Subsystem.h"
#include "Poco/Util/Application.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Logger.h"

/**
 * PythonManager
 *
 * Manage the Python UI.
 *  - Launch scripts
 *  - Launch console
 *
 * Child class of Poco::Util::Subsystem to
 * handle command line options (initscript, iconsole, execute).
 */
class PythonManager: public Poco::Util::Subsystem
{
public:
    PythonManager();
    virtual ~PythonManager();

    /**
     * Subsystem name
     *
     * Shall not contain spaces: see setLogger() call in initialize().
     */
    const char * name() const;

    /// @name un/re/initialization methods
    ///@{
    /**
     * Initialize the Python environment
     *
     * Redirect the standard error to custom function
     *
     * Any Python script should check `os.path.basename(sys.argv[0])`
     * to be sure that it is called from inside instrumentall.
     */
    void initialize(Poco::Util::Application& app);
    // void reinitialize(Application & app); // not needed. By default: uninit, then init.
    void uninitialize();
    ///@}

    /**
     * Subsystem main logic.
     *
     * @return exit code from Poco::Util::Application::ExitCode enum
     */
    int main(Poco::Util::Application& app);

    /**
     * Called before the Application's command line processing begins.
     *
     * Allow command line arguments support.
     */
    void defineOptions(Poco::Util::OptionSet & options);

    /**
     * Determine if the subsystem requires full control
     *
     * e.g. instead of a GUI beeing launched
     */
    bool requestFullControl();

    /**
     * Launch python script file.
     *
     * Exposed runScript function (see protected form).
     * Could be used by a module.
     */
    void runScript(Poco::Path filePath);

    /**
     * Display an error message via the local logger
     *
     * Used to redirect Python interpreter error messages
     */
    void errorMessage(std::string errorMsg);

protected:
    /// set the local logger
    void setLogger(Poco::Logger& logger);

    /// Get the local logger
    Poco::Logger& logger();

    /**
     * Run the given scriptFile as a python script.
     *
     * If a relative scriptFile path is given, the default behavior is:
     *  - check the given path relative to the current working directory
     *  - if not found, check it relative to the standard path (python/scripts)
     *
     * Viewed from the script itself,
     *  - the __file__ variable is set to the current script path.
     *  - the exit() function is explicitly set to sys.exit() (no console hocus pocus)
     *
     *  @see PythonManager::initialize regarding program name emulation
     *  in Python scripts.
     */
    void runScript(Poco::Util::Application& app, Poco::Path scriptFile);

    /// run the python console
    void runConsole(Poco::Util::Application& app);

    /**
     * Expose python API
     *
     * - forward the Python interpreter standard error to the PythonManager logger
     * - create the Python `instru` module to access instrumentall features
     * from inside the Python interpreter
     * - add python/embed to the Python path to allow easy import of custom modules
     */
    void exposeAPI();

    /**
     * Check if the Python interpreter is initialized
     *
     * @throw Poco::Exception if the interpreter is not initialized
     */
    void checkInit();

    /// set variable in python env
    void setVar(double value, const char* name, const char* module = "instru");

    /// set variable in python env
    void setVar(long value, const char* name, const char* module = "instru");

    /// set variable in python env
    void setVar(const char* value, const char* name, const char* module = "instru");

    /**
     * Get a variable value from the python env
     *
     * @param[out] value returned value (floating point)
     * @param[in] name variable name
     * @param[in] module (optionnal) module name
     * @throw Exception if the variable does not exist
     * or if the type does not fit
     */
    void getVar(double& value, const char* name, const char* module = "instru");

    /**
     * Get a variable value from the python env
     *
     * @param[out] value returned value (integer)
     * @param[in] name variable name
     * @param[in] module (optionnal) module name
     * @throw Exception if the variable does not exist
     * or if the type does not fit
     */
    void getVar(long& value, const char* name, const char* module = "instru");

    /**
     * Get a variable value from the python env
     *
     * @param[out] value returned value (std::string)
     * @param[in] name variable name
     * @param[in] module (optionnal) module name
     * @throw Exception if the variable does not exist
     * or if the type does not fit
     */
    void getVar(std::string& value, const char* name, const char* module = "instru");

    /// delete from python env a variable previously set
    void delVar(const char* name, const char* module = "instru");


private:
    /// local logger
    Poco::Logger* _pLogger;

    /// python variable tracking structure
    struct _varItem
    {
        std::string varName ; ///< python variable name
        std::string modName ; ///< python module name
    };

    /// keep trace of the variables that were added in the python env
    std::vector<_varItem> _addedVarStore;
};

//
// inlines
//
inline const char * PythonManager::name() const
{
    return "PythonManager";
}

inline void PythonManager::setLogger(Poco::Logger& logger)
{
    _pLogger = &logger;

    poco_information(this->logger(),
        "The " + std::string(name()) + " logger is now available");
}

inline Poco::Logger& PythonManager::logger()
{
    poco_check_ptr (_pLogger);
    return *_pLogger;
}

#endif /* HAVE_PYTHON27 */
#endif /* SRC_PYTHONMANAGER_H_ */
