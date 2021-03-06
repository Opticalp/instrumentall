/**
 * @file	src/core/ModuleManager.cpp
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

#include "ModuleManager.h"
#include "Dispatcher.h"

#include "modules/demo/DemoRootFactory.h"
#include "modules/dataGen/DataGenFactory.h"
#include "modules/signalProc/SignalProcFactory.h"
#include "modules/imageProc/ImageProcFactory.h"
#include "modules/devices/DeviceFactory.h"
#include "modules/fieldBus/FieldBusFactory.h"
#include "modules/extern/ExternFactory.h"
#include "modules/UI/UiFactory.h"
#include "modules/control/ControlFactory.h"


ModuleManager::ModuleManager():
	VerboseEntity(name())
{
    // Add root factories
    addFactory(new DemoRootFactory);
    addFactory(new DataGenFactory);
    addFactory(new SignalProcFactory);
    addFactory(new ImageProcFactory);
    addFactory(new DeviceFactory);
    addFactory(new FieldBusFactory);
    addFactory(new ExternFactory);
    addFactory(new UiFactory);
    addFactory(new ControlFactory);
}

ModuleManager::~ModuleManager()
{
    // uninitialize(); // should be already called by the system.

    for (std::vector<ModuleFactory*>::reverse_iterator it=rootFactories.rbegin(), ite=rootFactories.rend();
            it != ite; it++)
    {
        (*it)->delThis();
    }

    // allFactories should clean itself nicely
}

void ModuleManager::initialize(Poco::Util::Application& app)
{
    setLogger(name());

    for (std::vector<ModuleFactory*>::iterator it=rootFactories.begin(), ite=rootFactories.end();
            it != ite; it++)
    {
        (*it)->moduleDiscover();
    }
}

void ModuleManager::uninitialize()
{
    poco_information(logger(), "ModuleManager::uninitialize");

    for (std::vector<ModuleFactory*>::reverse_iterator it=rootFactories.rbegin(), ite=rootFactories.rend();
            it != ite; it++)
    {
        if ((*it)->isLeaf())
            (*it)->deleteChildModules();
        else
            (*it)->deleteChildFactories();
    }

    poco_information(logger(), "ModuleManager uninitialized");
}

void ModuleManager::defineOptions(Poco::Util::OptionSet& options)
{
	// TODO:
	//  - option to load some xml generator file
	//  - option to enable/disable dummies factory
	//  - ... ?
}

void ModuleManager::addModule(Module* pModule)
{
    SharedPtr<Module*> pMod(new (Module*)(pModule));
    modulesLock.writeLock();
    allModules.push_back(pMod);
    // poco_information(logger(),"module " + pModule->name() + " added in the module manager");
    Poco::Util::Application::instance().getSubsystem<Dispatcher>().addModule(pMod);
    modulesLock.unlock();
}

void ModuleManager::removeModule(Module* pModule)
{
    modulesLock.writeLock();
    for (std::vector< SharedPtr<Module*> >::iterator it=allModules.begin(),ite=allModules.end();
            it!=ite;
            it++)
    {
        if (pModule == **it)
        {
            Poco::Util::Application::instance().getSubsystem<Dispatcher>().removeModule(*it);
            // remove module
            **it = &emptyModule; // replace the pointed factory by something throwing exceptions
            allModules.erase(it);
            // poco_information(logger(), pModule->name() + " module erased from ModuleManager::allModules. ");
            modulesLock.unlock();
            return;
        }
    }

    modulesLock.unlock();
    poco_error(logger(), "removeModule(): "
            "the module was not found");
}

SharedPtr<Module*> ModuleManager::getModule(Module* pModule)
{
    modulesLock.readLock();
    for (std::vector< SharedPtr<Module*> >::iterator it=allModules.begin(), ite=allModules.end();
            it!=ite; it++)
    {
        if (pModule==**it)
        {
            modulesLock.unlock();
            return *it;
        }
    }

    modulesLock.unlock();
    throw Poco::NotFoundException("getModule", "module not found: "
            "Should have been deleted during the query");
}

SharedPtr<Module*> ModuleManager::getModule(std::string modName)
{
    modulesLock.readLock();
    for (std::vector< SharedPtr<Module*> >::iterator it=allModules.begin(), ite=allModules.end();
            it!=ite; it++)
    {
        if (modName.compare((**it)->name())==0
                || modName.compare((**it)->internalName())==0)
        {
            modulesLock.unlock();
            return *it;
        }
    }

    modulesLock.unlock();
    throw Poco::NotFoundException("getModule",
            "module " + modName + " not found");
}

std::vector< SharedPtr<Module*> > ModuleManager::getModules()
{
    // use a temp list to be thread safe
    std::vector< SharedPtr<Module*> > list;

    modulesLock.readLock();
    list = allModules;
    modulesLock.unlock();

    return list;
}

void ModuleManager::clearModules()
{
    poco_information(logger(), "Clearing all the modules...");

    // dispatcher.resetWorkflow
    Poco::Util::Application::instance()
            .getSubsystem<Dispatcher>()
            .resetWorkflow();

    // clear all modules
    std::vector< SharedPtr<Module*> > allModCopy = getModules();

    for (std::vector< SharedPtr<Module*> >::iterator it=allModCopy.begin(), ite=allModCopy.end();
            it!=ite; it++)
    {
        (**it)->destroy();
    }

    poco_information(logger(), "All modules cleared. ");
}

void ModuleManager::addFactory(ModuleFactory* pFactory)
{
    if (pFactory->isRoot())
        rootFactories.push_back(pFactory);

    factoriesLock.writeLock();
    allFactories.push_back(SharedPtr<ModuleFactory*>(new (ModuleFactory*)(pFactory)));
    factoriesLock.unlock();
}

void ModuleManager::removeFactory(ModuleFactory* pFactory)
{
    if (pFactory->isRoot())
        poco_bugcheck_msg("ModuleManager::removeFactory, "
                "the factory should not be a root factory");

    // find pFactory in allFactories
    factoriesLock.writeLock();
    for (std::vector< SharedPtr<ModuleFactory*> >::iterator it=allFactories.begin(), ite=allFactories.end();
            it!=ite; it++)
    {
        if (pFactory==**it)
        {
            **it = &emptyFactory; // replace the pointed factory by something throwing exceptions
            allFactories.erase(it);
            // poco_information(logger(), "factory erased " + std::string(pFactory->name()) + " from allFactories. ");
            factoriesLock.unlock();
            return;
        }
    }

    factoriesLock.unlock();
    poco_error(logger(), "removeFactory(): "
        "the factory was not found");
}

std::vector< SharedPtr<ModuleFactory*> > ModuleManager::getRootFactories()
{
    std::vector< SharedPtr<ModuleFactory*> > list;

    for (std::vector<ModuleFactory*>::iterator it=rootFactories.begin(),ite=rootFactories.end();
            it!=ite;
            it++)
        list.push_back(getFactory(*it));

    return list;
}

SharedPtr<ModuleFactory*> ModuleManager::getRootFactory(std::string name)
{
    for (std::vector<ModuleFactory*>::iterator it=rootFactories.begin(),ite=rootFactories.end();
            it!=ite;
            it++)
    {
        if (name.compare((*it)->name()) == 0)
        {
            return getFactory(*it);
        }
    }

    throw Poco::NotFoundException("getRootFactory",
            "factory " + name + " not found among the root factories. ");
}

SharedPtr<ModuleFactory*> ModuleManager::getFactory(ModuleFactory* pFactory)
{
    factoriesLock.readLock();
    for (std::vector< SharedPtr<ModuleFactory*> >::iterator it=allFactories.begin(), ite=allFactories.end();
            it!=ite; it++)
    {
        if (pFactory==**it)
        {
            factoriesLock.unlock();
            return *it;
        }
    }

    factoriesLock.unlock();
    throw Poco::NotFoundException("getFactory", "factory not found: "
            "Should have been deleted during the query");
}

#include "tools/graphviz/GvExportWorkFlow.h"

void ModuleManager::exportWFGraphviz(Poco::Path filePath)
{
	GvExportWorkFlow gvTool(getModules(), true);

	gvTool.writeDotFile(filePath);
}

std::string ModuleManager::exportWFGraphviz()
{
    GvExportWorkFlow gvTool(getModules(), true);

	return gvTool.getDotString();
}

#include "tools/graphviz/GvExportFactories.h"

void ModuleManager::exportFacTreeGraphviz(Poco::Path filePath)
{
    GvExportFactories gvTool(getRootFactories(), true);

    gvTool.writeDotFile(filePath);
}

std::string ModuleManager::exportFacTreeGraphviz()
{
    GvExportFactories gvTool(getRootFactories(), true);

    return gvTool.getDotString();
}

bool ModuleManager::allModuleReady()
{
    for (std::vector< SharedPtr<Module*> >::iterator it=allModules.begin(), ite=allModules.end();
            it!=ite; it++)
    {
        if (!(**it)->moduleReady())
        {
            poco_information(logger(), "ModMan::allModuleReady: "
                    + (**it)->name() + " not ready");
            return false;
        }
    }

    return true;
}
