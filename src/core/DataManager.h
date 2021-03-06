/**
 * @file	src/core/DataManager.h
 * @date	Feb. 2016
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

#ifndef SRC_DATAMANAGER_H_
#define SRC_DATAMANAGER_H_

#include "DataSource.h"
#include "OutPort.h"

#include "VerboseEntity.h"

#include "Poco/Util/Subsystem.h"
#include "Poco/AutoPtr.h"

#include "Poco/DynamicFactory.h"

#include <map>
#include <set>

using Poco::AutoPtr;

class DataLogger;
class DataProxy;

/**
 * DataManager
 *
 * Manage the data of the data ports using dataItems.
 */
class DataManager: public Poco::Util::Subsystem, public VerboseEntity
{
public:
    /**
     * Constructor
     *
     * Register the DataLoggers in the dynamic factory
     */
    DataManager();
    virtual ~DataManager();

    /**
     * Subsystem name
     *
     * Shall not contain spaces: see setLogger() call in initialize().
     */
    const char * name() const;

    /// @name un/re/initialization methods
    ///@{
    /**
     * Initialize the data loggers
     *
     * If standard data loggers were defined
     */
    void initialize(Poco::Util::Application& app);
    /**
     * Reset all data items to empty
     * Reset all data loggers
     */
    void uninitialize();
    ///@}

//    /**
//     * Called before the Application's command line processing begins.
//     *
//     * Allow command line arguments support.
//     */
//    void defineOptions(Poco::Util::OptionSet & options);

    /**
     * Get the data loggers class names
     *
     * To be used to create new loggers using the factory
     */
    std::map<std::string, std::string> dataLoggerClasses()
        { return loggerClasses; }

    /**
     * Create a new data logger of the given type
     */
    AutoPtr<DataLogger> newDataLogger(std::string className);

    /**
     * Get the data proxies class names
     *
     * To be used to create new proxies using the factory
     */
    std::map<std::string, std::string> dataProxyClasses()
        { return proxyClasses; }

    /**
     * Create a new data proxy of the given type
     */
    AutoPtr<DataProxy> newDataProxy(std::string className);

private:
    Poco::DynamicFactory<DataLogger> loggerFactory;
    // TODO: use unordered map for c++11-able compilers
    std::map<std::string, std::string> loggerClasses;
    /// To be used with loggerClasses and proxyClasses
    typedef std::pair<std::string,std::string> classPair;

    Poco::DynamicFactory<DataProxy> proxyFactory;
    // TODO: use unordered map for c++11-able compilers
    std::map<std::string, std::string> proxyClasses;
};

//
// inlines
//
inline const char * DataManager::name() const
{
    return "DataManager";
}

#endif /* SRC_DATAMANAGER_H_ */
