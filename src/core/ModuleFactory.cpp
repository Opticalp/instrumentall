/**
 * @file	src/core/ModuleFactory.cpp
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

ModuleFactory::ModuleFactory(bool leaf, bool root):
    bRoot(root), bLeaf(leaf),
    deletingChildFact(NULL),
	selectingNewFact(false),
    deletingChildMod(NULL), 
	VerboseEntity()
{
    if (!isRoot())
        Poco::Util::Application::instance().getSubsystem<ModuleManager>().addFactory(this);
}

void ModuleFactory::delThis()
{
	if (isLeaf())
		deleteChildModules();

	deleteChildFactories();
	
	delete this;
}

ModuleFactory::~ModuleFactory()
{
	if (!isRoot()) 
		Poco::Util::Application::instance().getSubsystem<ModuleManager>().removeFactory(this);
}

ModuleFactoryBranch& ModuleFactory::select(std::string selector)
{
    if (isLeaf())
        throw Poco::RuntimeException(name() + "::select",
                "No child factory: this factory is a leaf");

    std::string validated(validateSelector(selector));

    Poco::RWLock::ScopedWriteLock wLock(childFactLock);

    for (std::vector<ModuleFactoryBranch*>::iterator it=childFactories.begin(), ite=childFactories.end(); it != ite ; it++)
        if (validated.compare((*it)->getSelector())==0)
            return **it;

    // child factory not found, create one.
	try
	{
		selectingNewFact = true;
		ModuleFactoryBranch* factory(newChildFactory(validated));
		childFactories.push_back(factory);
		return *factory;
	}
	catch (...)
	{
		selectingNewFact = false;
		throw;
	}
}

std::string ModuleFactory::validateSelector(std::string selector)
{
    std::vector<std::string> list = selectValueList();

    // TODO: For case insensitivity, compare to lower case and return *it
    for (std::vector<std::string>::iterator it = list.begin(),
            ite = list.end(); it != ite; it++)
    {
        if (it->compare(selector) == 0)
            return selector;
    }

    throw Poco::InvalidArgumentException("Unrecognized selector: " + selector);
}

void ModuleFactory::deleteChildFactory(std::string property)
{
    if (isLeaf())
        throw Poco::RuntimeException(name() + "::deleteChildFactory",
                "No child: this factory is a leaf");

    std::string validated(validateSelector(property));

    Poco::RWLock::ScopedWriteLock wLock(childFactLock);

    for (std::vector<ModuleFactoryBranch*>::iterator it=childFactories.begin(),ite=childFactories.end();
            it!=ite;
            it++)
    {
        if (validated.compare((*it)->getSelector())==0)
        {
            deletingChildFact = *it;
			deletingChildFact->delThis();
            deletingChildFact = NULL;
            return;
        }
    }

    throw Poco::RuntimeException(name() + "deleteChildFactory",
            "Child factory not found");
}

void ModuleFactory::removeChildFactory(ModuleFactoryBranch* factory)
{
    if (deletingChildFact != factory)
	{
		if (selectingNewFact && !isChildFactory(factory))
			// exception during child factory creation
			return;
		else
			childFactLock.writeLock();
	}
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

bool ModuleFactory::isChildFactory(ModuleFactoryBranch * factory)
{
	for (std::vector<ModuleFactoryBranch*>::iterator it = childFactories.begin(),
		ite = childFactories.end(); it != ite; it++)
		if (*it == factory)
			return true;

	return false;
}

void ModuleFactory::deleteChildFactories()
{
    if (isLeaf())
    {
        // poco_information(logger(), "deleteChildFactories: factory is a leaf. "
        //         "Nothing to delete.");
        return;
    }

    childFactLock.writeLock();

    while( childFactories.size() )
    {
        deletingChildFact = childFactories.back();
        poco_information(logger(),"deleting child factory: "
                                    + std::string(deletingChildFact->name()));
        deletingChildFact->delThis();
        deletingChildFact = NULL;
    }

    childFactLock.unlock();
}


std::vector<Poco::SharedPtr<ModuleFactory*> > ModuleFactory::getChildFactories()
{
    if (isLeaf())
        return std::vector<Poco::SharedPtr<ModuleFactory*> >();

    std::vector< Poco::SharedPtr<ModuleFactory*> > list;

    ModuleManager& modMan =
            Poco::Util::Application::instance().getSubsystem<ModuleManager>();

    childFactLock.readLock();

    for (std::vector<ModuleFactoryBranch*>::iterator it = childFactories.begin(),
            ite = childFactories.end(); it != ite; it++)
        list.push_back(modMan.getFactory(*it));

    childFactLock.unlock();

    return list;
}

Module* ModuleFactory::create(std::string customName)
{
    if (!isLeaf())
    {
        Poco::RWLock::ScopedReadLock rLock(childFactLock);

        for (std::vector<ModuleFactoryBranch*>::iterator it=childFactories.begin(), ite=childFactories.end();
                it != ite ;
                it++)
            if ((*it)->countRemain())
                return (*it)->create(customName);

        throw Poco::RuntimeException(name() + "::create()",
                "ChildFact countRemain() is null! ");
    }

    {
    	// check if a module already exists with this custom name here
        Poco::RWLock::ScopedReadLock rLock(childModLock);

    	for (std::vector<Module*>::iterator it = childModules.begin(), ite = childModules.end();
    			it != ite; it++)
    	{
    		if ((*it)->name() == customName)
    		{
    			poco_notice(logger(), "A module is already existing with the given name, "
    					"using it. ");
    			return *it;
    		}
    	}
    }

    if (countRemain())
    {
        Module* module(newChildModule(customName));
        childFactLock.writeLock();
        childModules.push_back(module);
        childFactLock.unlock();
        return module;
    }
    else
    {
        throw Poco::RuntimeException(name() + "::create()",
                "countRemain() is null! ");
    }
}

size_t ModuleFactory::countRemain()
{
	if (isLeaf())
		throw Poco::NotImplementedException(name() + "::countRemain");

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
        // poco_information(logger(), "deleteChildModules: factory is not a leaf. "
        //         "Nothing to delete.");
        return;
    }

    childModLock.writeLock();

    while( childModules.size() )
    {
        deletingChildMod = childModules.back();
        delete (deletingChildMod);
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
