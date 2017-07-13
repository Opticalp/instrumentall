/**
 * @file	src/modules/control/ControlFactory.cpp
 * @date	Jul. 2017
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2017 Ph. Renaud-Goud / Opticalp

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

#include "ControlFactory.h"
#include "DataShapingFactory.h"

std::vector<std::string> ControlFactory::selectValueList()
{
	std::vector<std::string> list;
	list.push_back("dataShaping");
	return list;
}

ModuleFactoryBranch* ControlFactory::newChildFactory(std::string selector)
{
	if (selector.compare("dataShaping") == 0)
	{
		return new DataShapingFactory(this, selector);
	}
	else
	{
		// should not happen unless all the selectValueList()
		// values are not all represented in the above cases
		poco_bugcheck_msg("Create: unknown selector");
		throw Poco::BugcheckException();
	}
}
