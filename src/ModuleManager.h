/**
 * @file	src/ModuleManager.h
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

#ifndef SRC_MODULEMANAGER_H_
#define SRC_MODULEMANAGER_H_

#include "VerboseEntity.h"

#include "ModuleFactory.h"
#include "Module.h"

#include "Poco/Util/Subsystem.h"
#include "Poco/Util/Application.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Logger.h"

#include <vector>

/**
 * ModuleManager
 *
 * Manage the modules, including module factories
 * Instantiate the module factories
 */
class ModuleManager: public Poco::Util::Subsystem, public VerboseEntity
{
public:
	ModuleManager();
	virtual ~ModuleManager();

    /**
     * Subsystem name
     *
     * Shall not contain spaces: see setLogger() call in initialize().
     */
    const char * name() const;

    /// @name un/re/initialization methods
    ///@{
    /**
     * Instantiate the module factories
     * after having verified that the module list is empty
     */
    void initialize(Poco::Util::Application& app);
    // void reinitialize(Application & app); // not needed. By default: uninit, then init.
    /**
     * Clean module list.
     * Delete factories.
     */
    void uninitialize();
    ///@}

    /**
     * Called before the Application's command line processing begins.
     *
     * Allow command line arguments support.
     */
    void defineOptions(Poco::Util::OptionSet & options);

private:
    /// module factory list
    std::vector<ModuleFactory*> _factories;

    /// module list
    std::vector<Module*> _modules;
};

//
// inlines
//
inline const char * ModuleManager::name() const
{
    return "ModuleManager";
}

#endif /* SRC_MODULEMANAGER_H_ */
