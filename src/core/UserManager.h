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
#include "User.h"

#include "Poco/Util/Subsystem.h"

#include <set>
#include <map>

///// File where to find the signatures of the other files
//#define NX_STYLE_SIGN_FILE "access/.signs"

/// File where to store the user credentials in the form `[username]:[description]:[uid]:[digest]\n`
#define NX_STYLE_PWD_FILE "access/.passwd"

///// File where to store group membership `[groupname]:[gid]:[allowAll|denyAll]:[uid1,uid2,...]\n`
//#define NX_STYLE_GRP_FILE "access/.groups"
//
///**
// * Folder where to store the fine grained permissions
// *
// * file name : [GID]-[allow|deny]
// * `[itemname]:[value]\n`
// */
//#define NX_STYLE_TRUSTED_ITEMS_FOLDER "access/"



/**
 * UserManager
 *
 * Manage the users and the user permissions.
 *
 * The safety of this user manager is based on SHA1 robustness
 *
 * To improve the safety, it would be good to write protect the files defined by
 * NX_STYLE_PWD_FILE and NX_STYLE_TRUSTED_ITEMS_FILE
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

    /**
     * Retrieve a user pointer (shared pointer)
     *
     * @param userName user name
     * @param password associated password
     * @param[out] userPtr anonymous user (bad passwd or not-existing user) or given user
     *
     * @return true if user name and password fit.
     */
    bool authenticate(std::string userName, std::string password, UserPtr userPtr);

    /**
     * @return true if the password fits
     *
     *
     */
    bool verifyPasswd(UserPtr userPtr, std::string password);
    bool verifyPasswd(std::string userName, std::string password);

    /**
     * Get the anonymous user
     *
     * To be used for example after successive failing attempts to
     * verifyPasswd
     */
    void disconnectUser(UserPtr hUser);

private:
    /**
     * Parse NX_STYLE_PWD_FILE and load users
     */
    void loadAvailableUsers();

    /**
     * Set userPtr given the user name
     *
     * userPtr is assumed to be disconnected (anonymous)
     */
    void connectUser(std::string userName, UserPtr userPtr);

    std::set<UserPtr> connectedUsers; ///< set of shared ptr users
    std::set<User> availableUsers;
    std::map<std::string,std::string> userDigests;

    User anonymous;
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
