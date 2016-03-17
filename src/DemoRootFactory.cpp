/**
 * @file	src/DemoRootFactory.cpp
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


#include "DemoRootFactory.h"
#include "DemoBranchFactory.h"

std::vector<std::string> DemoRootFactory::selectValueList()
{
	std::vector<std::string> list;
	list.push_back("branch");
	return list;
}


ModuleFactoryBranch* DemoRootFactory::newChildFactory(std::string selector)
{
	if (selector.compare("branch") == 0)
	{
		return new DemoBranchFactory(this, selector);
	}
//	else if (selector.compare("branch1") == 0)
//	{
//		return new Branch1Factory(this, selector);
//	}
//	else if (selector.compare("branch2") == 0)
//	{
//		return new Branch2Factory(this, selector);
//	}
	else
	{
		// should not happen unless all the selectValueList()
		// values are not all represented in the above cases
		poco_bugcheck_msg("Create: unknown selector");
		throw Poco::BugcheckException();
	}
}

std::string DemoRootFactory::validateSelector(std::string selector)
{
	std::vector<std::string> list = selectValueList();

	// TODO: For case insensitivity, compare to lower case and return *it
	for (std::vector<std::string>::iterator it = list.begin(),
			ite = list.end(); it != ite; it++)
	{
		if (it->compare(selector) == 0)
			return selector;
	}

	throw ModuleFactoryException("Unrecognized selector: " + selector);
}
