/**
 * @file	src/modules/demo/DemoBranchFactory.cpp
 * @date	jan. 2016
 * @author	PhRG - opticalp.fr
 *
 * $Id$
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

#include "DemoBranchFactory.h"
#include "DemoLeafFactory.h"
#include "modules/GenericLeafFactory.h"

#include "DemoModuleA.h"
#include "DemoModuleFreeze.h"

std::vector<std::string> DemoBranchFactory::selectValueList()
{
	std::vector<std::string> list;
	list.push_back("leaf");
	list.push_back("leafA");
	list.push_back("leafB");
	list.push_back("leafDataSeq");
	list.push_back("leafSeqAccu");
	list.push_back("leafSeqMax");
	list.push_back("leafForwarder");
	list.push_back("leafParam");
	list.push_back("leafTwoInputs");
	list.push_back("freezer");
	return list;
}

ModuleFactoryBranch* DemoBranchFactory::newChildFactory(std::string selector)
{
    if (selector.compare("leafA") == 0)
        return new GenericLeafFactory<DemoModuleA>("DemoLeafAFactory",
                "Example code for using the GenericLeafFactory",
                this, selector);
    if (selector.compare("freezer") == 0)
        return new GenericLeafFactory<DemoModuleFreeze>("DemoFreezerFactory",
                "Create a freezing module to test the watchdog",
                this, selector);
    else
        return new DemoLeafFactory(this, selector);
}
