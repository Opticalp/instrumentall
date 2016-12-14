/**
 * @file	src/core/DepPoco.cpp
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


// since 1.4.0, Poco defines its version in Version.h
#ifdef POCO_VERSION_H
#include "Poco/Version.h"
#endif

#if ( POCO_VERSION >= 0x01030602 )
#include "Poco/Environment.h"
#endif
#include "Poco/NumberFormatter.h"

#include "DepPoco.h"

std::string DepPoco::name()
{
    return "POCO";
}

std::string DepPoco::description()
{
    return "\"POrtable COmponents\" are modern, "
            "powerful open source C++ class libraries "
            "for building network- and internet-based applications "
            "that run on desktop, server, mobile and embedded systems.";

    // TODO: describe components here?
}

std::string DepPoco::URL()
{
    return "http://pocoproject.org/";
}

std::string DepPoco::license()
{
    return "Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH. "
           "and Contributors. \n"
           "SPDX-License-Identifier: BSL-1.0";
}

std::string DepPoco::buildTimeVersion()
{
    return parseVersion(POCO_VERSION);
}

std::string DepPoco::runTimeVersion()
{
// Poco::Environment::libraryVersion introduced in 1.3.6p2
#if ( POCO_VERSION >= 0x01030602 )
    return parseVersion(Poco::Environment::libraryVersion());
#else
    return "Poco build time version < 1.3.6p2. "
           "Unable to get the runtime version..."; 
#endif
}

std::string DepPoco::parseVersion(Poco::UInt32 ver)
{
    std::string pocoV;

    int patch = (ver >> 8) & 0xFF;
    int minor = (ver >> 16) & 0xFF;
    int major = (ver >> 24) & 0xFF;

    pocoV = Poco::NumberFormatter::format(major) + "."
          + Poco::NumberFormatter::format(minor) + "."
          + Poco::NumberFormatter::format(patch) ;

    switch (ver & 0xF0)
    {
    case 0xA0:
        pocoV = pocoV + "(alpha" + Poco::NumberFormatter::format(ver & 0x0F) + ")";
        break;
    case 0xB0:
        pocoV = pocoV + "(beta" + Poco::NumberFormatter::format(ver & 0x0F) + ")";
        break;
    case 0xD0:
        pocoV = pocoV + "(dev" + Poco::NumberFormatter::format(ver & 0x0F) + ")";
        break;
    case 0x00:
        if ((ver & 0x0F) == 0 )
            pocoV = pocoV + "(stable)";
        // else: unknown
        break;
    default:
        // unknown
        break;
    }

    return pocoV;
}
