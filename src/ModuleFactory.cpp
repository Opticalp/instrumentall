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

ModuleFactory::ModuleFactory(bool leaf, bool root):
    bRoot(root), bLeaf(leaf),
    deletingChildFact(NULL),
    deletingChildMod(NULL)
{
    if (!isRoot())
        Poco::Util::Application::instance().getSubsystem<ModuleManager>().addFactory(this);
}

ModuleFactory::~ModuleFactory()
{
    if (isLeaf())
        deleteChildModules();

    deleteChildFactories();

    if (!isRoot())
        Poco::Util::Application::instance().getSubsystem<ModuleManager>().removeFactory(this);
}

ModuleFactoryBranch& ModuleFactory::select(std::string selector)
{
    std::string validated(validateSelector(selector));

    childFactLock.writeLock();

    for (std::vector<ModuleFactoryBranch*>::iterator it=childFactories.begin(), ite=childFactories.end(); it != ite ; it++)
    {
        if (validated.compare((*it)->getSelector())==0)
        {
            childFactLock.unlock();
            return **it;
        }
    }

    // child factory not found, create one.
    ModuleFactoryBranch* factory(newChildFactory(validated));
    childFactories.push_back(factory);
    childFactLock.unlock();
    return *factory;
}

void ModuleFactory::deleteChildFactory(std::string property)
{
    if (isLeaf())
        throw ModuleFactoryException("deleteChildFactory",
                "No child: this factory is a leaf");

    std::string validated(validateSelector(property));

    childFactLock.writeLock();

    for (std::vector<ModuleFactoryBranch*>::iterator it=childFactories.begin(),ite=childFactories.end();
            it!=ite;
            it++)
    {
        if (validated.compare((*it)->getSelector())==0)
        {
            deletingChildFact = *it;
            delete (*it);
            deletingChildFact = NULL;
            childFactLock.unlock();
            return;
        }
    }

    childFactLock.unlock();
    throw ModuleFactoryException("deleteChildFactory",
            "Child factory not found");
}

void ModuleFactory::removeChildFactory(ModuleFactoryBranch* factory)
{
    if (deletingChildFact != factory)
        childFactLock.writeLock();

    for (std::vector<ModuleFactoryBranch*>::reverse_iterator it = childFactories.rbegin(),
            ite = childFactories.rend(); it != ite; it++)
    {
        if (factory == (*it))
        {
            childFactories.erase((it+1).base());
            if (deletingChildFact != factory)
                childFactLock.unlock();
            return;
        }
    }

    if (deletingChildFact != factory)
        childFactLock.unlock();
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

    childFactLock.writeLock();

    for (std::vector<ModuleFactoryBranch*>::reverse_iterator it=childFactories.rbegin(),ite=childFactories.rend();
            it!=ite;
            it++)
    {
        poco_information(logger(),"deleting child factory: " + std::string((*it)->name()));
        deletingChildFact = *it;
        delete (*it);
        deletingChildFact = NULL;
    }

    childFactLock.unlock();
}

Module* ModuleFactory::create(std::string customName)
{
    if (!isLeaf())
    {
        childFactLock.readLock();
        for (std::vector<ModuleFactoryBranch*>::iterator it=childFactories.begin(), ite=childFactories.end();
                it != ite ;
                it++)
        {
            if ((*it)->countRemain())
                childFactLock.unlock();
                return (*it)->create(customName);
        }

        childFactLock.unlock();
        throw ModuleFactoryException("create()",
                "countRemain() is null! ");
    }
    else if (countRemain())
    {
        Module* module(newChildModule(customName));
        childFactLock.writeLock();
        childModules.push_back(module);
        childFactLock.unlock();
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

    childFactLock.readLock();
    for (std::vector<ModuleFactoryBranch*>::iterator it=childFactories.begin(), ite=childFactories.end();
            it != ite ;
            it++)
    {
        count += (*it)->countRemain();
    }
    childFactLock.unlock();

    return count;
}

void ModuleFactory::removeChildModule(Module* module)
{
    if (!isLeaf())
    {
        poco_bugcheck_msg("removeChildModule: "
                "No child module: this factory is not a leaf");
        return;
    }

    if (deletingChildMod != module)
        childModLock.writeLock();
    for (std::vector<Module*>::iterator it=childModules.begin(),ite=childModules.end();
            it!=ite;
            it++)
    {
        if ((*it)==module)
        {
            childModules.erase(it);
            if (deletingChildMod != module)
                childModLock.unlock();
            return;
        }
    }

    if (deletingChildMod != module)
        childModLock.unlock();
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

    childModLock.writeLock();

    for (std::vector<Module*>::reverse_iterator it = childModules.rbegin(),
            ite = childModules.rend(); it != ite; it++)
    {
        deletingChildMod = *it;
        delete *it;
        deletingChildMod = NULL;
    }

    childModLock.unlock();
}

std::vector< Poco::SharedPtr<Module*> > ModuleFactory::getChildModules()
{
    std::vector< Poco::SharedPtr<Module*> > list;

    if (!isLeaf())
    {
        poco_information(logger(), "getChildModules: factory is not a leaf. "
                "Getting the child modules from the child factories.");

        childFactLock.readLock();
        for (std::vector<ModuleFactoryBranch*>::iterator it = childFactories.begin(),
                ite = childFactories.end(); it != ite; it++)
        {
            std::vector< Poco::SharedPtr<Module*> > tmp = (*it)->getChildModules();
            list.insert(list.end(), tmp.begin(), tmp.end());
        }

        childFactLock.unlock();
    }
    else
    {
        ModuleManager& modMan =
                Poco::Util::Application::instance().getSubsystem<ModuleManager>();

        childModLock.readLock();

        for (std::vector<Module*>::iterator it = childModules.begin(),
                ite = childModules.end(); it != ite; it++)
            list.push_back(modMan.getModule(*it));

        childModLock.unlock();
    }

    return list;
}
