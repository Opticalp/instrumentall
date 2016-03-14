/**
 * @file	src/Module.h
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

#ifndef SRC_MODULE_H_
#define SRC_MODULE_H_

#include "VerboseEntity.h"

#include "Port.h"
#include "ParameterSet.h"

#include "Poco/Task.h"
#include "Poco/RWLock.h"
#include "Poco/Mutex.h"

class InPort;
class OutPort;
class ModuleFactory;

/// Time lapse in milliseconds between 2 tries in multithread lock case
#define TIME_LAPSE 10

POCO_DECLARE_EXCEPTION( , ModuleException, Poco::Exception)

/**
 * Module
 *
 * Base class for any module.
 * A module is a single cell that processes data. It can be:
 *  - bound to a physical device: camera generating images,
 *  led controller, motion stage controller, ... i.e. either
 *  actuators or sensors
 *  - a computing cell: image processing, signal processing,
 *  ...
 *
 *  Inherit from Poco::Task to have the Poco::Runnable features
 *  and to be able to control progress and task cancellation.
 */
class Module: public VerboseEntity, public Poco::Task
{
public:
	/**
	 * Constructor
	 *
	 * The implementation should at least do the following:
	 *  - explicitly call this constructor
	 *  - generate a name that will be returned by internalName()
	 *  - set the logger
	 *  - call @ref setCustomName after the internalName is set
	 *  - call @ref notifyCreation to let the module being registered
	 *  in the managers
	 *
	 * @param parent Parent module factory
	 * @param name Name to be used as task name. E.g. use the custom name
	 *
	 * @throw ModuleException forwarded from setCustomName or setInternalName
	 * if customName or internalName is already in use.
	 */
	Module(ModuleFactory* parent, std::string name = ""):
	      mParent(parent), Poco::Task(name), VerboseEntity("module") { }

	/**
	 * Destructor
	 *
	 * Notify any entity that could store a pointer on this.
	 *  - parent factory
	 *  - module manager
	 *  - dispatcher
	 */
	virtual ~Module();

	/**
	 * Custom name of the module
	 *
	 * @return Name defined by the user at creation time
	 * @see setCustomName()
	 */
	std::string name() { return mName; }

	/**
	 * Internal name of the module
	 *
	 * This name may include dynamic content (e.g. index number...)
	 * since no duplicates are allowed.
	 * @return Name computed by the module itself regarding internal
	 * considerations
	 * @see setInternalName()
	 */
	std::string internalName() { return mInternalName; }

	/**
	 * Description of the module
	 *
	 * This description does not include dynamic content bound to
	 * parameters (factory parameters or module parameters).
	 * It should be static in the derived classes,
	 * so it can be queried by the factory
	 * before module creation.
	 * @return full description of the module: functionalities, usage
	 */
	virtual const char * description() const = 0;

	/**
	 * Get parent ModuleFactory
	 *
	 * e.g. to get factory parameters
	 */
	ModuleFactory* parent();

	/**
	 * Get input ports
	 *
	 * The dispatcher should be requested to get shared pointer
	 * on the port items
	 * @return a copy of the input ports list
	 */
	std::vector<InPort*> getInPorts() { return inPorts; }

	/**
	 * Get output ports
	 *
	 * @see getInPorts
	 */
	std::vector<OutPort*> getOutPorts() { return outPorts; }

	/**
	 * Implementation of Poco::Task::runTask()
	 */
	virtual void runTask() { poco_warning(logger(), name() + ": empty task"); }

	/**
	 * Retrieve a copy of the parameter set of the module
	 *
	 * @param pSet reference to a user allocated ParameterSet
	 */
	void getParameterSet(ParameterSet* pSet);

    /**
     * Retrieve the parameter data type
     *
     * @throw Poco::NotFoundException if the parameter name is not found
     */
    ParamItem::ParamType getParameterType(std::string paramName);

	/**
	 * Retrieve the value of the parameter given by its name
	 *
	 * Check the parameter type and call one of:
	 *  - getIntParameterValue
	 *  - getFloatParameterValue
	 *  - getStrParameterValue
	 *
	 * @throw Poco::NotFoundException if the name is not found
	 * @throw Poco::DataFormatException if the parameter format does not fit
	 */
    template<typename T> T getParameterValue(std::string paramName);

    /**
     * Set the value of the parameter given by its name
     *
     * Check the parameter type and call one of:
     *  - setIntParameterValue
     *  - setFloatParameterValue
     *  - setStrParameterValue
     *
     * @throw Poco::NotFoundException if the name is not found
     * @throw Poco::DataFormatException if the parameter format does not fit
     */
    template<typename T> void setParameterValue(std::string paramName, T value);

protected:
    /**
     * Set the internal name of the module
     *
     * This method has to be called by the constructor
     * before setCustomName()
     * @throw Poco::ExistsException If internalName is already in use
     * @throw Poco::SyntaxException If internalName contains bad characters
     */
    void setInternalName(std::string internalName);

	/**
	 * Set the custom name of the module
	 *
	 * This method has to be called by the constructor.
	 * If customName is empty, use internalName().
     * @throw Poco::ExistsException If customName is already in use
     * @throw Poco::SyntaxException If customName contains bad characters
	 */
	void setCustomName(std::string customName);

	/**
	 * Notify the managers about the module creation
	 *
	 * Has to be called at the end of the constructors implementation
	 * in order to be registered in the ModuleManager and
	 * in the Dispatcher
	 */
	void notifyCreation();

	/**
	 * Set the inPorts list size
	 */
	void setInPortCount(size_t cnt) { inPorts.resize(cnt, NULL); }
	/**
	 * Add input data port
	 *
	 * @param name name of the port
	 * @param description description of the port
	 * @param dataType type of port data
	 * @param index index of the port in the inPorts list. It allows
	 * to use enums to access the ports.
	 */
	void addInPort(
	        std::string name, std::string description,
	        int dataType,
	        size_t index );

    /**
     * Set the outPorts list size
     */
    void setOutPortCount(size_t cnt) { outPorts.resize(cnt, NULL); }
    /**
     * Add output data port
     *
     * @param name name of the port
     * @param description description of the port
     * @param dataType type of port data
     * @param index index of the port in the outPorts list. It allows
     * to use enums to access the ports.
     */
    void addOutPort(
            std::string name, std::string description,
            int dataType,
            size_t index );

    /**
     * Set parameter set size
     *
     * to be called before adding parameters
     */
    void setParameterCount(size_t count)
        { paramSet.resize(count); }

    /**
     * Add a parameter in the parameter set
     *
     * Should be called in the module constructor
     */
    void addParameter(size_t index, std::string name, std::string descr, ParamItem::ParamType datatype);

    virtual long getIntParameterValue(size_t paramIndex)
    {
        poco_bugcheck_msg("getIntParameterValue not implemented for this module");
        throw Poco::BugcheckException();
    }

    virtual double getFloatParameterValue(size_t paramIndex)
    {
        poco_bugcheck_msg("getFloatParameterValue not implemented for this module");
        throw Poco::BugcheckException();
    }

    virtual std::string getStrParameterValue(size_t paramIndex)
    {
        poco_bugcheck_msg("getStrParameterValue not implemented for this module");
        throw Poco::BugcheckException();
    }

    virtual void setIntParameterValue(size_t paramIndex, long value)
        { poco_bugcheck_msg("setIntParameterValue not implemented for this module"); }

    virtual void setFloatParameterValue(size_t paramIndex, double value)
        { poco_bugcheck_msg("setFloatParameterValue not implemented for this module"); }

    virtual void setStrParameterValue(size_t paramIndex, std::string value)
        { poco_bugcheck_msg("setStrParameterValue not implemented for this module"); }

private:
    /// enum to be returned by checkName
    enum NameStatus
    {
        nameOk,
        nameExists,
        nameBadSyntax
    };

    /**
     * Check if the given name is allowed
     *
     *  - verify that the syntax is ok
     *  - verify that the name is not already in use
     */
    NameStatus checkName(std::string newName);

    /**
     * Remove the internal name of the names list
     *
     * To be called by setCustomName in case of failing module creation
     * process due to malformed custom name, or name already in use.
     */
    void freeInternalName();

	/**
	 * Parent module factory
	 *
	 * - to be notified when the module is deleted
	 * - to get the factory parameters (selectors, evtl parameters)
	 */
	ModuleFactory* mParent;

	std::string mInternalName; ///< internal name of the module
	std::string mName; ///< custom name of the module

	std::vector<InPort*> inPorts; ///< list of input data ports
	std::vector<OutPort*> outPorts; ///< list of output data ports

	static std::vector<std::string> names; ///< list of names of all modules
	static Poco::RWLock namesLock; ///< read write lock to access the list of names

	ParameterSet paramSet;

	/**
	 * Retrieve a parameter index from its name
	 *
	 * The main mutex has to be locked before calling this function
	 */
	size_t getParameterIndex(std::string paramName);
};

/// templates implementation
#include "Module.ipp"

#endif /* SRC_MODULE_H_ */
