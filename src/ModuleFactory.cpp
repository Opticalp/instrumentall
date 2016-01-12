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

#include "ModuleFactory.h"
#include "ModuleFactoryBranch.h"

#include <typeinfo>
POCO_IMPLEMENT_EXCEPTION( ModuleFactoryException, Poco::Exception, "ModuleFactory error")

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
    return factory;
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
            childFactories.erase(it);
            return;
        }
    }

    throw ModuleFactoryException("deleteChildFactory",
            "Child factory not found");
}

void ModuleFactory::deleteChildFactories()
{
    if (isLeaf())
        return;

    for (std::vector<ModuleFactoryBranch*>::iterator it=childFactories.begin(),ite=childFactories.end();
            it!=ite;
            it++)
    {
        delete (*it);
    }

    childFactories.clear();
}

Module* ModuleFactory::create(std::string customName)
{
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
        throw ModuleFactoryException("removeChildModule",
                "No child module: this factory is not a leaf");

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

    throw ModuleFactoryException("removeChildFactory",
            "Child not found");
}

void ModuleFactory::deleteChildModules()
{
    if (!isLeaf())
        return;

    for (std::vector<Module*>::reverse_iterator it=childModules.rbegin(),ite=childModules.rend();
            it!=ite;
            it++)
        delete *it;
}
