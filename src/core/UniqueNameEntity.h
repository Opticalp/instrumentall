/**
 * @file	src/core/UniqueNameEntity.h
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

#ifndef SRC_CORE_UNIQUENAMEENTITY_H_
#define SRC_CORE_UNIQUENAMEENTITY_H_

#include "Poco/Mutex.h"

#include <string>
#include <set>

/**
 * Insure that a name is unique
 * 
 * Manage the description also.
 */
class UniqueNameEntity
{
public:
    UniqueNameEntity() { }
    virtual ~UniqueNameEntity() { }

    std::string name() { return mName; }

    /**
     * @return full description of the entity; functionalities, usage
     */
    virtual std::string description() = 0;

protected:
    /// enum to be returned by checkName
    enum NameStatus
    {
        nameOk,
        nameExists,
        nameBadSyntax
    };

    /**
     * Set a new name for the entity
     */
    void setName(std::string newName, bool freeOld = true);

    /**
     * Remove an old name from the used name list.
     */
    void freeName(std::string oldName);

    /**
     * Add a name in the used name list
     */
    NameStatus reserveName(std::string newName);


    /**
     * Check if the given name is allowed
     *
     *  - verify that the syntax is ok
     *  - verify that the name is not already in use
     */
    NameStatus checkName(std::string newName);

private:
    std::string mName;

    static std::set<std::string> names; ///< list of names of all modules
    static Poco::Mutex namesLock; ///< recuresive mutex to access the name list
};

#endif /* SRC_CORE_UNIQUENAMEENTITY_H_ */
