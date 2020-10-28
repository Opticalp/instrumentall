/**
 * @file	src/dataLoggers/SaveImageLogger.h
 * @date	Jan. 2017
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2017 Ph. Renaud-Goud / Opticalp

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

#ifndef SRC_DATALOGGERS_SAVEIMAGELOGGER_H_
#define SRC_DATALOGGERS_SAVEIMAGELOGGER_H_

#ifdef HAVE_OPENCV

#include "core/DataLogger.h"

/**
 * SaveImageLogger
 * 
 * DataLogger to save images when hit.
 * The file name increments after each image.
 * @see parameters description
 */
class SaveImageLogger: public DataLogger
{
public:
    SaveImageLogger();
    virtual ~SaveImageLogger() { }

    std::string description() { return classDescription(); }

    static std::string classDescription()
        { return "Save images using incremental file name. "
                "See parameters description"; }

    void log();

private:
    static size_t refCount;

    std::set<int> supportedInputDataType();

    enum params
    {
        paramDirectory,
        paramPrefix,
        paramDigits,
        paramExtension,
        paramNextIndex,
		paramNormalize,
        paramCnt
    };

    Poco::Int64 nextIndex; ///< storage for the next index
    int digits; ///< storage for digit count
    Poco::Path directory; ///< file storage directory
    std::string prefix; ///< file name prefix
    std::string extension; ///< image file extension

    enum normalization
	{
    	normNone,
		normMin,
		normMax,
		normDef = 4, // default value
	};

	int normalize; ///< image normalization method

    Poco::Int64 getIntParameterValue(size_t paramIndex);
    void setIntParameterValue(size_t paramIndex, Poco::Int64 value);

    void setStrParameterValue(size_t paramIndex, std::string value);
    std::string getStrParameterValue(size_t paramIndex);
};

#endif /* HAVE_OPENCV */
#endif /* SRC_DATALOGGERS_SAVEIMAGELOGGER_H_ */
