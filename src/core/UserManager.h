/**
 * @file	src/core/UserManager.h
 * @date	Nov. 2017
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

#ifndef SRC_USERMANAGER_H_
#define SRC_USERMANAGER_H_

#ifdef MANAGE_USERS

#include "VerboseEntity.h"

#include "Poco/Util/Subsystem.h"

#include <set>

/**
 * UserManager
 *
 * Manage the users and the user permissions.
 */
class UserManager: public Poco::Util::Subsystem, public VerboseEntity
{
public:
    /**
     * Constructor
     *
     */
    UserManager();
    virtual ~UserManager();

    /**
     * Subsystem name
     *
     * Shall not contain spaces: see setLogger() call in initialize().
     */
    const char * name() const;

    /// @name un/re/initialization methods
    ///@{
    void initialize(Poco::Util::Application& app);
    void uninitialize();
    ///@}

//    /**
//     * Called before the Application's command line processing begins.
//     *
//     * Allow command line arguments support.
//     */
//    void defineOptions(Poco::Util::OptionSet & options);


private:
    // std::set<UserPtr> connectedUsers; ///< set of shared ptr users
};

//
// inlines
//
inline const char * UserManager::name() const
{
    return "UserManager";
}

#endif /* MANAGE_USERS */
#endif /* SRC_USERMANAGER_H_ */
