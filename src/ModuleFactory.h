/**
 * @file	src/ModuleFactory.h
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

#ifndef SRC_MODULEFACTORY_H_
#define SRC_MODULEFACTORY_H_

#include "Module.h"

#include <string>

/**
 * ModuleFactory
 *
 * Base class for any module factory. A module factory is a class
 * which instantiation is able to create modules on demand.
 */
class ModuleFactory
{
public:
	ModuleFactory();
	virtual ~ModuleFactory();

	/**
	 * Name of the module factory
	 *
	 * @note no duplicate should be allowed
	 */
	virtual const char * name() const = 0;

	/**
	 * Description of the module factory
	 *
	 * @return full description of the module factory:
	 * short description of modules to be created, default behavior.
	 */
	virtual const char * description() const = 0;

	/**
	 * Refine factory settings
	 */
	virtual ModuleFactory& select(std::string property)=0;

	// TODO: query to know how to "select()"

	virtual Module* create(std::string customName)=0;
};

#endif /* SRC_MODULEFACTORY_H_ */
