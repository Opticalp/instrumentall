/**
 * @file	src/ParameterizedEntity.h
 * @date	June 2016
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

#ifndef SRC_PARAMETERIZEDENTITY_H_
#define SRC_PARAMETERIZEDENTITY_H_

#include "VerboseEntity.h"

#include "ParameterSet.h"

#include "Poco/Util/Application.h" // layered configuration

/**
 * ParameterizedEntity
 *
 * Base class to handle parameters in an entity.
 *
 * Primarily used with modules.
 */
class ParameterizedEntity: public virtual VerboseEntity
{
public:
	/**
	 * Constructor
	 *
	 * @param keyPrefix prefix key to be used in the config files
	 * If a temporary prefixKey is defined at construction, the
	 * right prefix key shall be set in order to use the conf file
	 * entries
	 *
	 * @see setPrefixKey
	 */
	ParameterizedEntity(std::string prefixKey):
		confPrefixKey(prefixKey)
	{
	}

	virtual ~ParameterizedEntity()
	{
	}

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

    /**
     * Expire output data
     */
    virtual void expireOutData() = 0;

protected:
	/**
	 * Change the prefix key
	 */
	void setPrefixKey(std::string prefixKey)
	{	confPrefixKey = prefixKey; }

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

    /**
     * Add a parameter in the parameter set
     *
     * Should be called in the module constructor.
     * Specify a default value as a string,
     * as could be read in a config file
     */
    void addParameter(size_t index,
            std::string name, std::string descr,
            ParamItem::ParamType datatype,
            std::string hardCodedValue);

    /**
     * Retrieve the default value for the given parameter
     *
     * - Check if the parameter has an entry in the configuration:
     *      module.<name>.<paramName>
     * - if not found, return the hard-coded value that was defined at creation,
     * - if not found...
     * @throw Poco::NotFoundException
     */
    long getIntParameterDefaultValue(size_t index);
    double getFloatParameterDefaultValue(size_t index);
    std::string getStrParameterDefaultValue(size_t index);

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
	ParameterizedEntity();
	std::string confPrefixKey;

	ParameterSet paramSet;

	/**
	 * Retrieve a parameter index from its name
	 *
	 * The main mutex has to be locked before calling this function
	 */
	size_t getParameterIndex(std::string paramName);

	/**
	 * Table of hard-coded values
	 *
	 * @see addParameter
	 */
	std::map<size_t, std::string> hardCodedValues;

	/**
	 * Retrieve the raw default value as a string
	 */
	std::string getParameterDefaultValue(size_t index);

    /**
     * Convenience function to get the application config
     *
     * simple forwarder to `Poco::Util::Application::instance().config()`
     */
    Poco::Util::LayeredConfiguration& appConf()
        { return Poco::Util::Application::instance().config(); }

};

/// templates implementation
#include "ParameterizedEntity.ipp"

#endif /* SRC_PARAMETERIZEDENTITY_H_ */
