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

#include "VerboseEntity.h"
#include "Module.h"

class ModuleFactoryBranch;

POCO_DECLARE_EXCEPTION( , ModuleFactoryException, Poco::Exception)

/**
 * ModuleFactory
 *
 * Base class for any module factory. A module factory is a class
 * which instantiation is able to create modules on demand.
 * The module factories that are retrieved via ModuleFactory::select()
 * are specialized module factories derived from ModuleFactoryBranch
 *
 * A ModuleFactory can be a leaf: it is the last factory of the select()
 * chain. select() should not be called for this factory. It throws an
 * exception. create() should be called there.
 */
class ModuleFactory: public VerboseEntity
{
public:
	/**
	 * Constructor
	 *
	 * The implementation should:
	 *  - generate a name if it is not static
	 *  - set the logger
	 */
	ModuleFactory();

    /**
     * Standard destructor
     *
     * The destructor of the derived classes should explicitly call deleteChildren().
     * It is not called here to let the developer decide what order to respect.
     *
     * @note base class destructor is called (last) after derived class destructor
     *
     * @todo if the device factory is a leaf (check isLeaf() ), then it should
     * be necessary to delete created Module references.
     */
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
     * Update the discovered modules list
     *
     * Should be called at the end of the derived class
     * constructor implementation.
     *
     * Default behavior: does nothing.
     * @note Only relevant for hardware devices
     */
    virtual void moduleDiscover() { }

	/**
	 * Refine factory settings
	 *
	 * @note This method does not need to be implemented in inherited
	 * leaf factories according to the default behavior.
	 */
    virtual ModuleFactoryBranch& select(std::string property)
    {
        throw ModuleFactoryException("select()",
                "This factory is probably a leaf. ");
    }

    /**
     * Describe the role of select()
     *
     * Usage help
     *
     * @note This method does not need to be implemented in inherited
     * leaf factories according to the default behavior.
     */
    virtual const char* selectDescription()
    {
        throw ModuleFactoryException("selectDescription()",
                "This factory is probably a leaf. ");
    }

    /**
     * List select() possible values
     *
     * - The first value can be considered as the default value
     * - an empty string means that the choice is open (e.g. shared lib name)
     *
     * @note This method does not need to be implemented in inherited
     * leaf factories according to the default behavior.
     */
    virtual std::vector<std::string> selectValueList()
    {
        throw ModuleFactoryException("selectValueList()",
                "This factory is probably a leaf. ");
    }

    /**
     * Check if the current factory is the last leaf of the select() tree
     *
     * If the returned value is `true` (default behavior),
     * select() should not be called.
     */
    virtual bool isLeaf() { return true; }

    /**
     * Check if the current factory is the root factory
     *
     * If the returned value is `false`, a reinterpret_cast to
     * ModuleFactoryBranch* should be possible (without using a safer
     * dynamic_cast, then).
     */
    virtual bool isRoot() { return true; }

    /**
     * Delete the child factory given by the selector
     *
     * Inverse as select()
     * @param property selector, as used in select()
     */
    virtual void deleteChildFactory(std::string property);

	/**
	 * Create a module
	 *
	 * Create either the very specified module if the select() chain
	 * was complete, or create the next default module, using the
	 * select() default values if the select() chain was not complete.
	 * @param customName custom name to be given to the module.
	 * If this string is empty, the module internal name shall be used.
	 */
	virtual Module* create(std::string customName="");

	/**
	 * Count how many times create() can be called
	 *
	 * The return value is valid for the current factory.
	 * I.e. no further select() is called.
	 */
    virtual size_t countRemain();

protected:
    /**
     * Delete all the child factories
     *
     * Shall be call at the end of the derived class destructor.
     */
    void deleteChildFactories();

    /**
     * Validate the selector to get an absolute value
     *
     * Should insure that the selector is unique for a given factory
     */
    virtual std::string validateSelector(std::string selector)
    {
        return selector;
    }

    std::vector<ModuleFactoryBranch*> _children; // direct child factories
};

#endif /* SRC_MODULEFACTORY_H_ */
