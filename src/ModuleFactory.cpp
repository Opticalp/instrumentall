/**
 * @file	src/ModuleFactory.cpp
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
#include "ModuleFactory.h"
#include "ModuleFactoryBranch.h"

#include <typeinfo>
POCO_IMPLEMENT_EXCEPTION( ModuleFactoryException, Poco::Exception, "ModuleFactory error")

ModuleFactory::ModuleFactory(bool root):
    bRoot(root)
{
    if (!isRoot())
        Poco::Util::Application::instance().getSubsystem<ModuleManager>().addFactory(this);
}

ModuleFactory::~ModuleFactory()
{
    if (!isRoot())
        Poco::Util::Application::instance().getSubsystem<ModuleManager>().removeFactory(this);
}

ModuleFactoryBranch& ModuleFactory::select(std::string selector)
{
    std::string validated(validateSelector(selector));

    for (std::vector<ModuleFactoryBranch*>::iterator it=childFactories.begin(), ite=childFactories.end(); it != ite ; it++)
    {
        if (validated.compare((*it)->getSelector())==0)
            return **it;
    }

    // child factory not found, create one.
    ModuleFactoryBranch* factory(newChildFactory(validated));
    childFactories.push_back(factory);
    return *factory;
}

void ModuleFactory::deleteChildFactory(std::string property)
{
    if (isLeaf())
        throw ModuleFactoryException("deleteChildFactory",
                "No child: this factory is a leaf");

    std::string validated(validateSelector(property));

    for (std::vector<ModuleFactoryBranch*>::iterator it=childFactories.begin(),ite=childFactories.end();
            it!=ite;
            it++)
    {
        if (validated.compare((*it)->getSelector())==0)
        {
            delete (*it);
            return;
        }
    }

    throw ModuleFactoryException("deleteChildFactory",
            "Child factory not found");
}

void ModuleFactory::removeChildFactory(ModuleFactoryBranch* factory)
{
    for (std::vector<ModuleFactoryBranch*>::iterator it=childFactories.begin(),ite=childFactories.end();
            it!=ite;
            it++)
    {
        if (factory == (*it))
        {
            childFactories.erase(it);
            return;
        }
    }

    poco_error(logger(), "removeChildFactory: "
            "Child factory not found");
}

void ModuleFactory::deleteChildFactories()
{
    if (isLeaf())
    {
        poco_debug(logger(), "deleteChildFactories: factory is a leaf. "
                "Nothing to delete.");
        return;
    }

    for (std::vector<ModuleFactoryBranch*>::reverse_iterator it=childFactories.rbegin(),ite=childFactories.rend();
            it!=ite;
            it++)
        delete (*it);
}

Module* ModuleFactory::create(std::string customName)
{
    // TODO: notify the module manager that a module was created?
    if (!isLeaf())
    {
        for (std::vector<ModuleFactoryBranch*>::iterator it=childFactories.begin(), ite=childFactories.end();
                it != ite ;
                it++)
        {
            if ((*it)->countRemain())
                return (*it)->create();
        }

        throw ModuleFactoryException("create()",
                "countRemain() is null! ");
    }
    else if (countRemain())
    {
        Module* module(newChildModule(customName));
        childModules.push_back(module);
        return module;
    }
    else
    {
        throw ModuleFactoryException("create()",
                "countRemain() is null! ");
    }
}

size_t ModuleFactory::countRemain()
{
    size_t count=0;

    for (std::vector<ModuleFactoryBranch*>::iterator it=childFactories.begin(), ite=childFactories.end();
            it != ite ;
            it++)
    {
        count += (*it)->countRemain();
    }

    return count;
}

void ModuleFactory::removeChildModule(Module* module)
{
    if (!isLeaf())
    {
        poco_error(logger(),"removeChildModule: "
                "No child module: this factory is not a leaf");
        return;
    }

    for (std::vector<Module*>::iterator it=childModules.begin(),ite=childModules.end();
            it!=ite;
            it++)
    {
        if ((*it)==module)
        {
            childModules.erase(it);
            return;
        }
    }

    poco_error(logger(),"removeChildFactory: "
            "Child not found");
}

void ModuleFactory::deleteChildModules()
{
    if (!isLeaf())
    {
        poco_debug(logger(), "deleteChildModules: factory is not a leaf. "
                "Nothing to delete.");
        return;
    }

    for (std::vector<Module*>::reverse_iterator it=childModules.rbegin(),ite=childModules.rend();
            it!=ite;
            it++)
        delete *it;
}
