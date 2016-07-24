/**
 * @file	src/DataPocoLogger.h
 * @date	Mar 2016
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

#ifndef SRC_DATAPOCOLOGGER_H_
#define SRC_DATAPOCOLOGGER_H_

#include "DataLogger.h"

#include "Poco/Logger.h"

/**
 * DataPocoLogger
 *
 * Very simple data logger that outputs data into a Poco::Logger
 */
class DataPocoLogger: public DataLogger
{
public:
    DataPocoLogger();

    virtual ~DataPocoLogger() { }

    std::string name() { return mName; }

    static std::string description()
        { return "Output data into a Poco::Logger"; }

    void log();

private:
    static size_t refCount;
    std::string mName;

    bool isSupportedInputDataType(int datatype);

    std::set<int> supportedInputDataType();

    /**
     * Set the local logger
     */
    void setRecLogger(std::string loggerName)
    {
        pLogger = &Poco::Logger::get(loggerName);
    }

    /**
     * Get the local logger
     */
    Poco::Logger& recLogger()
    {
        poco_check_ptr(pLogger);
        return *pLogger;
    }

    Poco::Logger* pLogger;

    /**
     * Log a vector
     *
     * called by runTask()
     */
    void logVectorValue(DataItem::DataTypeEnum dataType);
};

#endif /* SRC_DATAPOCOLOGGER_H_ */
