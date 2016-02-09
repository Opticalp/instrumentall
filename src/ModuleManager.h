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
#include "EmptyModuleFactory.h"
#include "Module.h"
#include "EmptyModule.h"

#include "Poco/Util/Subsystem.h"
#include "Poco/Util/Application.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Logger.h"
#include "Poco/SharedPtr.h"
#include "Poco/RWLock.h"

using Poco::SharedPtr;

/**
 * ModuleManager
 *
 * Manage the modules, including module factories
 * Instantiate the module factories
 */
class ModuleManager: public Poco::Util::Subsystem, VerboseEntity
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
     * Reset factories.
     *
     * - reset factories discovery
     * - clear child factories
     *0x0
     * @note reseting the factories removes their child modules
     */
    void uninitialize();
    ///@}

    /**
     * Called before the Application's command line processing begins.
     *
     * Allow command line arguments support.
     */
    void defineOptions(Poco::Util::OptionSet & options);

    /**
     * Add a Module to the list
     *
     * This function is called by the @ref Module constructor
     *
     * Notify the @ref Dispatcher subsystem that a module is created
     */
    void addModule(Module* pModule);

    /**
     * Remove a Module from the list
     *
     * This function is called by the @ref Module destructor.
     *
     * Notify the @ref Dispatcher subsystem that a module is deleted
     */
    void removeModule(Module* pModule);

    /**
     * Get a shared pointer on a module
     */
    SharedPtr<Module*> getModule(Module* pModule);

    /**
     * Get all the modules
     *
     * @return a copy of allModules
     */
     std::vector< SharedPtr<Module*> > getModules();

    /**
     * Add a factory to the list
     *
     * This function is called by the @ref ModuleFactory constructor.
     */
    void addFactory(ModuleFactory* pFactory);

    /**
     * Remove a factory from the list
     *
     * This function is called by the @ref ModuleFactory destructor
     */
    void removeFactory(ModuleFactory* pFactory);

    /**
     * Get the list of root factories names
     */
    std::vector<std::string> getRootFactories();

    /**
     * Get a root factory, given its name
     */
    SharedPtr<ModuleFactory*> getRootFactory(std::string name);

    /**
     * Get a shared pointer on a factory
     */
    SharedPtr<ModuleFactory*> getFactory(ModuleFactory* pFactory);

    /**
     * Return the address of the empty module
     */
    Module* getEmptyModule() { return &emptyModule; }

private:
    /**
     * Root module factory list
     *
     * This list is not associated to a RWLock since the rootFactories
     * are created once at the manager creation, and then, it is
     * deleted at the manager destruction. Then, there should not
     * be concurrent modification in those processes.
     */
    std::vector<ModuleFactory*> rootFactories;

    /**
     * All module factory list
     *
     * When a Factory is exported to the "outside", a shared pointer
     * on its pointer is exported. Then, if the factory is deleted,
     * the shared pointer will point on EmptyModuleFactory emptyFactory,
     * and the entry is removed from this list.
     */
    std::vector< SharedPtr<ModuleFactory*> > allFactories;
    Poco::RWLock factoriesLock;

    /**
     * To be used to replace an expired factory to throw errors
     * when its pointer is used.
     *
     * This is the only factory that do not stand in allFactories
     */
    EmptyModuleFactory emptyFactory;

    /**
     * All module list
     *
     * When a Module is exported to the "outside", a shared pointer
     * on its pointer is exported. Then, if the module is deleted,
     * the shared pointer will point to EmptyModule emptyModule,
     * and the entry is removed from this list.
     */
    std::vector< SharedPtr<Module*> > allModules;
    Poco::RWLock modulesLock;

    /**
     * To be used to replace an expired module to throw errors
     * when its pointer is used.
     *
     * This module is not added to allModules
     * @see addModule()
     */
    EmptyModule emptyModule;
};

//
// inlines
//
inline const char * ModuleManager::name() const
{
    return "ModuleManager";
}

#endif /* SRC_MODULEMANAGER_H_ */
