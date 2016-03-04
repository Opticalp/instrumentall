/**
 * @file	src/DemoBranchFactory.h
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

#ifndef SRC_DEMOBRANCHFACTORY_H_
#define SRC_DEMOBRANCHFACTORY_H_

#include "ModuleFactoryBranch.h"

/**
 * DemoBranchFactory
 *
 * Very simple module factory to illustrate ModuleFactoryBranch usage
 *
 * Child of DemoRootFactory
 */
class DemoBranchFactory: public ModuleFactoryBranch
{
public:
    DemoBranchFactory(ModuleFactory* parent, std::string selector):
        ModuleFactoryBranch(parent, selector, false) { setLogger(name()); }
    virtual ~DemoBranchFactory() { }

    std::string name() { return "DemoBranchFactory"; }
    const char * description() const
    {
        return "Example code for a branch module factory. "
                "Please use selectValueList() to check "
                "the authorized selectors. ";
    }

    const char* selectDescription()
    {
        return "Demo select() with multiple selector choice. "
                "See selectValueList()";
    }

    std::vector<std::string> selectValueList()
    {
        std::vector<std::string> list;
        list.push_back("leaf");
        list.push_back("leafA");
        list.push_back("leafB");
        list.push_back("leafDataSeq");
        list.push_back("leafSeqAccu");
        list.push_back("leafSeqMax");
        list.push_back("leafForwarder");
        return list;
    }

private:
    ModuleFactoryBranch* newChildFactory(std::string selector);

    std::string validateSelector(std::string selector);
};

#endif /* SRC_DEMOBRANCHFACTORY_H_ */
