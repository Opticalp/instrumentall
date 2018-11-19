/**
 * @file	src/core/ModuleFactory.h
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

#include "Poco/SharedPtr.h"
#include "Poco/RWLock.h"

#include "VerboseEntity.h"
#include "Module.h"

class ModuleFactoryBranch;

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
	ModuleFactory(bool leaf = true, bool root = true);

	/**
	* Delete this object (tweak)
	*
	* Do not call `delete object;` directly, since the deleting instructions
	* need to be called in reversed order (general, then child).
	*/
	void delThis();

	/**
	 * Name of the module factory
	 *
	 * @note no duplicate should be allowed
	 */
	virtual std::string name() = 0;

	/**
	 * Description of the module factory
	 *
	 * @return full description of the module factory:
	 * short description of modules to be created, default behavior.
	 */
	virtual std::string description() = 0;

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
	 *  - check if a child factory already corresponds to the
	 *  given selector
	 *     - create a new child factory using newChildFactory()
	 *     if no existing factory was found. The newly created
	 *     factory is inserted into the child factory list
	 *     - or return the existing child factory
	 *
	 * @see newChildFactory()
	 */
    ModuleFactoryBranch& select(std::string selector);

    /**
     * Describe the role of select()
     *
     * Usage help
     *
     * @note This method does not need to be implemented in inherited
     * leaf factories according to the default behavior.
     */
    virtual std::string selectDescription()
    {
        throw Poco::NotImplementedException(name() + "::selectDescription()",
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
        throw Poco::NotImplementedException(name() + "::selectValueList()",
                "This factory is probably a leaf. ");
    }

    /**
     * Check if the current factory is the last leaf of the select() tree
     *
     * If the returned value is `true` (default behavior),
     * select() should not be called.
     */
    bool isLeaf() { return bLeaf; }

    /**
     * Check if the current factory is the root factory
     *
     * If the returned value is `false`, a reinterpret_cast to
     * ModuleFactoryBranch* should be possible (without using a safer
     * dynamic_cast, then).
     */
    bool isRoot() { return bRoot; }

    /**
     * Delete the child factory given by the selector
     *
     * Inverse as select()
     * @param property selector, as used in select()
     */
    void deleteChildFactory(std::string selector);

    /**
     * Remove the given child factory from the local list
     *
     * Do not throw exception on error, since this method
     * should be called by a destructor
     */
    void removeChildFactory(ModuleFactoryBranch* factory);

    /**
     * Retrieve the child factories
     *
     * Those factories are ModuleFactoryBranch, but we get a shared ptr
     * on ModuleFactory from the ModuleManager
     *
     * @see select
     */
    std::vector< Poco::SharedPtr<ModuleFactory*> > getChildFactories();

	/**
	 * Create a module
	 *
	 * If this factory is not a leaf, call the create() method of the
	 * next child factory having a countRemain() not returning zero.
	 *
	 * If this factory is a leaf, call newChildModule() and add it
	 * to the module list.
	 * The leaf factories derived from this class
	 * have to implement newChildModule().
	 *
	 * @param customName custom name to be given to the module.
	 * If this string is empty, the module internal name shall be used.
	 *
     * @throw Poco::ExistsException If customName is already in use
     * @throw Poco::SyntaxException If customName contains bad characters
	 */
	Module* create(std::string customName="");

    /**
     * Remove the given child module from the local list
     *
     * Do not throw exception on error, since this method
     * should be called by a destructor
     */
    void removeChildModule(Module* module);

	/**
	 * Count how many times create() can be called
	 *
	 * The default implementation calls the countRemain() of
	 * all the child factories and add it all.
	 */
    virtual size_t countRemain();

    /**
     * Delete all the child factories
     *
     * Shall be called in the derived class destructor.
     * @note constructors are called base first, derived last,
     * destructors are called in the reverse order.
     */
    void deleteChildFactories();

    /**
     * Delete all the child modules
     *
     * Shall be called in the derived class destructor.
     */
    void deleteChildModules();

    /**
     * Retrieve all the child modules
     *
     * If the factory is not a leaf, retrieve the child modules
     * from the child factories
     */
    std::vector< Poco::SharedPtr<Module*> > getChildModules();

protected:
	/**
	* Standard destructor
	*
	* If not EmptyModuleFactory, only called by ModuleFactory::delThis
	*
	* @note base class destructor is called (last) after derived class destructor
	*/
	virtual ~ModuleFactory();

	/**
     * Create a new child factory from ModuleFactoryBranch
     *
     * This method is to be called by select(). It should not
     * be directly accessed.
     *
     * The selector validity is checked in select().
     *
     * select() inserts the newly created child factory into
     * the child factories list. It shall not be done in the implementation.
     */
    virtual ModuleFactoryBranch* newChildFactory(std::string selector)
    {
        throw Poco::NotImplementedException(name() + "::select()",
                "This factory is probably a leaf. ");
    }

    /**
     * Validate the selector to get an absolute value
     *
     * Should insure that the selector is unique for a given factory
     */
    virtual std::string validateSelector(std::string selector);

    /**
     * Create a new module given its custom name
     *
     * This function is called by create() if the factory is a leaf factory
     *
     * @see isLeaf()
     *
     * @param customName custom name to be given to the module.
     * If this string is empty, the module internal name shall be used.
     */
    virtual Module* newChildModule(std::string customName)
    {
        throw Poco::NotImplementedException(name() + "::create()",
                "This factory is not able to create a new child module. ");
    }

private:
	/**
     * Direct child factories
     *
     *  - Only newChildFactory() should create new factories. This function
     *  is called by select(). select() adds the new factory into this list.
     *  - Only deleteChildFactories() and deleteChildFactory() should
     *  erase factories from that list.
     */
    std::vector<ModuleFactoryBranch*> childFactories;
    Poco::RWLock childFactLock; ///< childFactories access lock
	/**
	 * New factory being created by select()
	 *
	 * this flag is used to avor deadlock if an exception occurs
	 * during child factory creation. 
	 * @see ModuleFactoryBranch::~ModuleFactoryBranch
	 */
	bool selectingNewFact;
    /**
     * Child factory being deleted
     *
     * Use for thread safe child factory deletion operation.
     * The risk comes from the fact that a factory can be removed by
     * another way than removeChildFactory or removeChildFactories
     */
    ModuleFactoryBranch* deletingChildFact;

	/**
	 * check if the given factory is a child factory
	 */
	bool isChildFactory(ModuleFactoryBranch* factory);

    /**
     * Direct child modules
     *
     * - Only newChildModule() should create new modules. This function
     * is called by create(). create() adds the new module into the list.
     */
    std::vector<Module*> childModules;
    Poco::RWLock childModLock; ///< childModules access lock
    /**
     * Child module being deleted
     *
     * Use for thread safe child module deletion operation
     * @see deletingChildFact
     */
    Module* deletingChildMod;

	bool bRoot; ///< flag to check if this module factory is a root factory
    bool bLeaf; ///< flag to check if this module factory is a leaf factory
};

#endif /* SRC_MODULEFACTORY_H_ */
