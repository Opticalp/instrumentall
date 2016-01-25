/**
 * @file	src/Module.cpp
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

#include "Module.h"
#include "ModuleFactory.h"
#include "ModuleManager.h"

#include <typeinfo>
POCO_IMPLEMENT_EXCEPTION( ModuleException, Poco::Exception, "Module error")

Module::Module(ModuleFactory* parent):
      mParent(parent)
{
    // if not EmptyModule, add this to module manager
    if (parent)
        Poco::Util::Application::instance().getSubsystem<ModuleManager>().addModule(this);
}

Module::~Module()
{
	// TODO:
    // - notify dispatcher

    // notify parent factory
    if (mParent)
    {
        mParent->removeChildModule(this);

        // notify module manager
        Poco::Util::Application::instance().getSubsystem<ModuleManager>().removeModule(this);
    }
}

void Module::setInternalName(std::string internalName)
{
    // TODO:
    //  - check unicity

//    if internalName is unique
        mInternalName = internalName;
//    else
//        throw ModuleException("setInternalName",
//            "the name: " + internalName + " is already in use");
}

void Module::setCustomName(std::string customName)
{
    // TODO:
    //  - check unicity

    std::string name;

    if (customName.empty())
        name = internalName();
    else
        name = customName;

//    if name is unique
        mName = name;
//    else
//        throw ModuleException("setCustomName",
//            "the name: " + name + " is already in use");
}

ModuleFactory* Module::parent()
{
    if (mParent)
        return mParent;
    else
        throw ModuleException("parent",
                "This module has no valid parent factory");
}
