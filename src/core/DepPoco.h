/**
 * @file	src/core/DepPoco.h
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

#ifndef SRC_DEPPOCO_H_
#define SRC_DEPPOCO_H_

#include "Poco/Types.h"

#include "Dependency.h"

/**
 * DepPoco
 *
 * Description of Poco external dependency.
 * Only implement parent abstract methods.
 */
class DepPoco: public Dependency
{
public:
    DepPoco() { }

    std::string name();
    std::string description();
    std::string URL();
    std::string license();
    std::string buildTimeVersion();
    std::string runTimeVersion();

private:
    std::string parseVersion(Poco::UInt32 ver);
};

#endif /* SRC_DEPPOCO_H_ */
