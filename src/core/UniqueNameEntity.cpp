/**
 * @file	src/core/UniqueNameEntity.cpp
 * @date	May 2018
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2018 Ph. Renaud-Goud / Opticalp

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

#include "UniqueNameEntity.h"

std::set<std::string> UniqueNameEntity::names;
Poco::Mutex UniqueNameEntity::namesLock;

void UniqueNameEntity::setName(std::string newName, bool freeOld)
{
    std::string oldName(mName);

    switch (reserveName(newName))
    {
    case nameOk:
        mName = newName;
        return;
    case nameExists:
        throw Poco::ExistsException("setName",
                newName + " already in use");
    case nameBadSyntax:
        throw Poco::SyntaxException("setName",
                "The name should only contain alphanumeric characters "
                "or \"-\", \"_\", \".\"");
    default:
        poco_bugcheck_msg("Impossible case condition");
    }

    if (!mName.empty() && freeOld)
        freeName(oldName);
}

void UniqueNameEntity::freeName(std::string oldName)
{
    if (!oldName.empty())
    {
        Poco::Mutex::ScopedLock lock(namesLock); // recursive mutex, ok with checkName() call

        names.erase(oldName);
    }
}

UniqueNameEntity::NameStatus UniqueNameEntity::reserveName(std::string newName)
{
    Poco::Mutex::ScopedLock lock(namesLock); // recursive mutex, ok with checkName() call

    NameStatus ret = checkName(newName);

    if (ret == nameOk)
        names.insert(newName);

    return ret;
}

#include "Poco/RegularExpression.h"

UniqueNameEntity::NameStatus UniqueNameEntity::checkName(std::string newName)
{
    Poco::Mutex::ScopedLock lock(namesLock); // recursive mutex, ok.

    // check syntax
    Poco::RegularExpression regex("^[0-9a-zA-Z\\._-]+$");
    if (!regex.match(newName))
        return nameBadSyntax;

    // check existence
    if (names.count(newName))
        return nameExists;

    return nameOk;
}

UniqueNameEntity::~UniqueNameEntity()
{
    freeName(mName);
}
