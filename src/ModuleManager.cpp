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

ModuleManager::ModuleManager():
	VerboseEntity(name())
{
    // TODO:
    //  - create root factories
}

ModuleManager::~ModuleManager()
{
    uninitialize();

    for (std::vector<ModuleFactory*>::reverse_iterator it=_factories.rbegin(), ite=_factories.rend();
            it != ite; it++)
    {
        delete *it;
        // it is then removed from the list because removeRootFactory()
        // is called by the ModuleFactory::~ModuleFactory() if it is a root.
    }
}

void ModuleManager::initialize(Poco::Util::Application& app)
{
    setLogger(name());

    for (std::vector<ModuleFactory*>::iterator it=_factories.begin(), ite=_factories.end();
            it != ite; it++)
    {
        (*it)->moduleDiscover();
    }
}

void ModuleManager::uninitialize()
{
    for (std::vector<ModuleFactory*>::reverse_iterator it=_factories.rbegin(), ite=_factories.rend();
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

void ModuleManager::removeRootFactory(ModuleFactory* pFactory)
{
    for (std::vector<ModuleFactory*>::iterator it=_factories.begin(),ite=_factories.end();
            it!=ite;
            it++)
    {
        if (pFactory == *it)
        {
            _factories.erase(it);
            return;
        }
    }

    poco_error(logger(), "removeRootFactory(): "
            "the factory was not found");
}
