/**
 * @file	src/Dependency.h
 * @date	nov. 2015
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

#ifndef SRC_DEPENDENCY_H_
#define SRC_DEPENDENCY_H_

#include <string>

/**
 * Dependency
 *
 * Base class to be used to describe external dependencies (libraries)
 */
class Dependency
{
public:
    Dependency() { }
    virtual ~Dependency() { }

    virtual std::string name()=0;
    virtual std::string description()=0;
    virtual std::string URL()=0;
    virtual std::string license()=0;

    /**
     * Version of the library as it was at build time
     *
     * Usually got from the headers or any cmake-related feature
     */
    virtual std::string buildTimeVersion()=0;

    /**
     * Version of the library as it is at run time
     *
     * It provides a mean to verify that the run time version
     * is the same than the build time version.
     *
     * This function should call a function in the external library
     * and not rely on headers (!)
     */
    virtual std::string runTimeVersion()=0;
};

#endif /* SRC_DEPENDENCY_H_ */
