/**
 * @file	src/ModuleManager.cpp
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

#include "DemoRootFactory.h"

ModuleManager::ModuleManager():
	VerboseEntity(name())
{
    // Add root factories
    // They are added automatically to the list via their constructor
    new DemoRootFactory;
}

ModuleManager::~ModuleManager()
{
    uninitialize();

    for (std::vector<ModuleFactory*>::reverse_iterator it=rootFactories.rbegin(), ite=rootFactories.rend();
            it != ite; it++)
    {
        delete *it;
        // it is then removed from the list because removeRootFactory()
        // is called by the ModuleFactory::~ModuleFactory() if it is a root.
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
    for (std::vector<ModuleFactory*>::reverse_iterator it=rootFactories.rbegin(), ite=rootFactories.rend();
            it != ite; it++)
    {
        if ((*it)->isLeaf())
            (*it)->deleteChildModules();
        else
            (*it)->deleteChildFactories();
    }
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
    _modules.push_back(pModule);
}

void ModuleManager::removeModule(Module* pModule)
{
    for (std::vector<Module*>::iterator it=_modules.begin(),ite=_modules.end();
            it!=ite;
            it++)
    {
        if (pModule == *it)
        {
            _modules.erase(it);
            return;
        }
    }

    poco_error(logger(), "removeModule(): "
            "the module was not found");
}

void ModuleManager::addFactory(ModuleFactory* pFactory)
{
    if (pFactory->isRoot())
        rootFactories.push_back(pFactory);

    ModuleFactory** ppFactory = new (ModuleFactory*)(pFactory);
    allFactories.push_back(SharedPtr<ModuleFactory*>(ppFactory));
}

void ModuleManager::removeFactory(ModuleFactory* pFactory)
{
    if (pFactory->isRoot())
    {
        bool notFound = true;

        for (std::vector<ModuleFactory*>::iterator it=rootFactories.begin(),ite=rootFactories.end();
                it!=ite;
                it++)
        {
            if (pFactory == *it)
            {
                rootFactories.erase(it);
                notFound = false;
                poco_debug(logger(), "factory " + pFactory->name() + " erased from rootFactories. ");
                break;
            }
        }

        if (notFound)
            poco_error(logger(), "removeFactory(): "
                "the factory was not found as a root factory "
                "although isRoot() is set");
    }

    // find pFactory in allFactories
    for (std::vector<SharedPtr<ModuleFactory*>>::iterator it=allFactories.begin(), ite=allFactories.end();
            it!=ite; it++)
    {
        if (pFactory==**it)
        {
            **it = &emptyFactory; // replace the pointed factory by something throwing exceptions
            allFactories.erase(it);
            poco_debug(logger(), "factory erased " + pFactory->name() + " from allFactories. ");
            return;
        }
    }

    poco_error(logger(), "removeFactory(): "
        "the factory was not found");
}

SharedPtr<ModuleFactory*> ModuleManager::getFactory(ModuleFactory* pFactory)
{
    for (std::vector<SharedPtr<ModuleFactory*>>::iterator it=allFactories.begin(), ite=allFactories.end();
            it!=ite; it++)
    {
        if (pFactory==**it)
            return *it;
    }

    throw ModuleFactoryException("getFactory", "factory not found: "
            "Should have been deleted during the query");
}
