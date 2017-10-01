/**
 * @file	src/modules/extern/PythonModule.h
 * @date	Feb. 2017
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

#ifndef SRC_MODULES_EXTERN_PYTHONMODULE_H_
#define SRC_MODULES_EXTERN_PYTHONMODULE_H_

#ifdef HAVE_PYTHON27

#include "core/Module.h"

/**
 * Python Module
 *
 * When all the input ports are trigged, the python script is launched.
 * The input ports are released when the script returns.
 *
 * There is no output port. DataGen modules should be used instead.
 *
 * The count of input ports is determined using the factory selector.
 * This selector is a semi-colon-separated list of trig port names.
 */
class PythonModule: public Module
{
public:
    PythonModule(ModuleFactory* parent, std::string customName);

    std::string description();

private:
    static size_t refCount; ///< reference counter to generate a unique internal name

    void process(int startCond);

    enum params
    {
        paramScriptFilePath,
        paramCnt
    };

    std::string getStrParameterValue(size_t paramIndex);
    void setStrParameterValue(size_t paramIndex, std::string value);

    std::string scriptPath;
};

#endif /* HAVE_PYTHON27 */
#endif /* SRC_MODULES_EXTERN_PYTHONMODULE_H_ */
