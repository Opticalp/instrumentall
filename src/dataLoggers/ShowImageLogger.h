/**
 * @file	src/dataLoggers/ShowImageLogger.h
 * @date	Apr. 2016
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

#ifndef SRC_SHOWIMAGELOGGER_H_
#define SRC_SHOWIMAGELOGGER_H_

#ifdef HAVE_OPENCV

#include "core/DataLogger.h"

/**
 * ShowImageLogger
 *
 * Data logger that displays an image
 */
class ShowImageLogger: public DataLogger
{
public:
    ShowImageLogger();
    virtual ~ShowImageLogger() { }

    std::string description() { return classDescription(); }

    static std::string classDescription()
        { return "Display an image"; }

    void log();

private:
    static size_t refCount;
	static size_t winCount;

    bool isSupportedInputDataType(int datatype);

    std::set<int> supportedInputDataType();

    enum params
    {
        paramImagePanel,
        paramCnt
    };

    Poco::Int64 imagePanelIndex; ///< storage for the next index

    Poco::Int64 getIntParameterValue(size_t paramIndex);
    void setIntParameterValue(size_t paramIndex, Poco::Int64 value);
};

#endif /* HAVE_OPENCV */
#endif /* SRC_SHOWIMAGELOGGER_H_ */
