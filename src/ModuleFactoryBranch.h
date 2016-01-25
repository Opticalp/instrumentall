/**
 * @file	src/ModuleFactoryBranch.h
 * @date	jan. 2016
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2016 Ph. Renaud-Goud / Opticalp

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

#ifndef SRC_MODULEFACTORYBRANCH_H_
#define SRC_MODULEFACTORYBRANCH_H_

#include "ModuleFactory.h"

/**
 * ModuleFactoryBranch
 *
 * Base class for module factories obtained via selection using select()
 */
class ModuleFactoryBranch: public ModuleFactory
{
public:
    ModuleFactoryBranch(ModuleFactory* parent, std::string selector):
        ModuleFactory(false),
        mParent(parent), mSelector(selector) { }

    /**
     * Destructor
     *
     * Notify the parent that it has to be removed from the list
     */
    virtual ~ModuleFactoryBranch()
    {
        mParent->removeChildFactory(this);
    }

    /**
     * Retrieve the select() value of the parent
     *
     * that drove to the present module factory
     */
    std::string getSelector() { return mSelector; }

    /**
     * Get parent factory.
     *
     * if parent().isRoot() is false, then this cast is made possible:
     *
     *     reinterpret_cast<DeviceFactoryBranch*>(parent())
     *
     */
    ModuleFactory* parent() { return mParent; }

protected:
    std::string mSelector;
    ModuleFactory* mParent;
};

#endif /* SRC_MODULEFACTORYBRANCH_H_ */
