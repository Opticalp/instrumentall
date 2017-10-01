/**
 * @file	src/core/VerboseEntity.h
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

#ifndef SRC_VERBOSEENTITY_H_
#define SRC_VERBOSEENTITY_H_

#include "Poco/Logger.h"

/**
 * VerboseEntity
 *
 * Base class to add logging ability to any entity: subsystem, module, etc.
 */
class VerboseEntity
{
public:
    /**
	 * Temporary constructor
	 *
	 * The logger is first set to startup.<name>
	 * It should then be changed using setLogger()
	 * to a new logger e.g. inheriting from a parent
	 * logger.
	 *
	 * @param name name of the entity. Should be unique,
	 * ascii-friendly and should not contain spaces.
	 */
	VerboseEntity(std::string name)
	{
		setLogger("startup." + name);
	}

	/**
	 * Fully qualified constructor
	 *
	 * No additional call to setLogger is needed.
	 *
	 * @param logger logger to be used for this entity.
	 */
	VerboseEntity(Poco::Logger& logger)
	{
		setLogger(logger);
	}

    /**
     * Standard constructor
     *
     * Used in abstract base classes.
     * Should not be used in implementations.
     * Use VerboseEntity(Poco::Logger& logger) instead
     */
	VerboseEntity(): pLogger(NULL) { }

	/**
	 * Default destructor
	 *
	 * Nothing to be done
	 */
	virtual ~VerboseEntity() { }

	/**
	 * Change logger priority/level
	 */
	void setVerbosity(int priority)
	{
		logger().setLevel(priority);
	}

	/**
	 * Get logger priority/level
	 */
	int getVerbosity()
	{
		return logger().getLevel();
	}

protected:
    /**
     * Set the local logger
     */
    void setLogger(Poco::Logger& logger)
    {
    	pLogger = &logger;
        // poco_information(this->logger(),
        //     this->logger().name() + " logger is now available");
    }

    /**
     * Set the local logger
     */
    void setLogger(std::string loggerName)
    {
        setLogger(Poco::Logger::get(loggerName));
    }

    /**
     * Get the local logger
     */
    Poco::Logger& logger()
    {
    	poco_check_ptr(pLogger);
    	return *pLogger;
    }

private:
    /// local logger
    Poco::Logger* pLogger;
};

#endif /* SRC_VERBOSEENTITY_H_ */
