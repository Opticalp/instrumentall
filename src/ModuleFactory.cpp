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

void ModuleFactory::deleteChildFactory(std::string property)
{
    if (isLeaf())
        throw ModuleFactoryException("deleteChildFactory",
                "No child: this factory is a leaf");

    std::string selector(validateSelector(property));

    for (std::vector<ModuleFactoryBranch*>::iterator it=_children.begin(),ite=_children.end();
            it!=ite;
            it++)
    {
        if (selector.compare((*it)->getSelector())==0)
        {
            delete (*it);
            _children.erase(it);
            return;
        }
    }

    throw ModuleFactoryException("deleteChildFactory",
            "Child not found");
}

void ModuleFactory::deleteChildFactories()
{
    if (isLeaf())
        return;

    for (std::vector<ModuleFactoryBranch*>::iterator it=_children.begin(),ite=_children.end();
            it!=ite;
            it++)
    {
        delete (*it);
    }

    _children.clear();
}

Module* ModuleFactory::create(std::string customName)
{
    for (std::vector<ModuleFactoryBranch*>::iterator it=_children.begin(), ite=_children.end();
            it != ite ;
            it++)
    {
        if ((*it)->countRemain())
            return (*it)->create();
    }

    throw ModuleFactoryException("create()",
            "countRemain() is null! ");
}

size_t ModuleFactory::countRemain()
{
    size_t count=0;

    for (std::vector<ModuleFactoryBranch*>::iterator it=_children.begin(), ite=_children.end();
            it != ite ;
            it++)
    {
        count += (*it)->countRemain();
    }

    return count;
}
