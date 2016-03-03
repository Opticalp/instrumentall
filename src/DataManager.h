/**
 * @file	src/DataManager.h
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

#include "DataItem.h"

#include "VerboseEntity.h"

#include "Poco/Util/Subsystem.h"
#include "Poco/SharedPtr.h"

using Poco::SharedPtr;

class DataLogger;

/**
 * DataManager
 *
 * Manage the data of the data ports using dataItems.
 */
class DataManager: public Poco::Util::Subsystem, public VerboseEntity
{
public:
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
     * Add the DataItem of a new output port
     *
     * append its DataItem to the dataStore
     * This function is called by the OutPort constructor
     */
    void addOutPort(OutPort* port);

    /**
     * Remove the DataItem of a deleted output port
     *
     * remove its DataItem from the dataStore
     * This function is called by the OutPort destructor
     */
    void removeOutPort(OutPort* port);

    /**
     * To be called when new data is available
     *
     * Launch the log actions
     *
     * @param self caller
     */
    void newData(DataItem* self);

private:
    std::vector< SharedPtr<DataItem*> > allData; ///< data corresponding to each OutPort

    DataItem emptyDataItem;

    // TODO: any volatile data storage here that is used by the UI
    // to push data (one shot) into a given InPort

    // TODO: all about the data loggers:
    // - a list of used/available data loggers
    // - a link from each data to its data logger(s)
};

//
// inlines
//
inline const char * DataManager::name() const
{
    return "DataManager";
}

#endif /* SRC_DATAMANAGER_H_ */
