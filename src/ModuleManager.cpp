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

    // TODO:
    //  - delete root factories
}

void ModuleManager::initialize(Poco::Util::Application& app)
{
	// TODO:
	//  - set logger name
	//  - run factories discovery (?)
}

void ModuleManager::uninitialize()
{
	// TODO:
	//  - reset factories (last in, first reset):
	//     * reset factories discovery
	//     * clear child factories
    //
    // n.b.: reseting the factories will remove
    // their child modules
}

void ModuleManager::defineOptions(Poco::Util::OptionSet& options)
{
	// TODO:
	//  - option to load some xml generator file
	//  - option to enable/disable dummies factory
	//  - ... ?
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
}

