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
#include "Poco/Path.h"

#include <set>
#include <map>

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
     *
     * The hUser has to be initialized:
     *  - neither hUser.isNull()
     *  - nor (*hUser) == NULL
     */
    void disconnectUser(UserPtr hUser);

    /**
     * Check if the given user is allowed to execute the given script
     *
     * @param path script path
     * @param content script content. Will permit to compute the digest
     * to verify file access permission
     * @param userPtr user for which the permission is to be checked
     */
    bool isScriptAuthorized(std::string path, std::string& content, UserPtr userPtr);

    /**
     * Check if the given user is allowed to access the given folder
     *
     * check if the parent folders are allowed.
     *
     * @param folderPath folder path
     * @param userPtr user for which the permission is to be checked
     */
    bool isFolderAuthorized(Poco::Path folderPath, UserPtr userPtr);

    /**
     * First instantiate a new user
     *
     * assert hUser.isNull();
     */
    void initUser(UserPtr& hUser);

    bool isAdmin(UserPtr userPtr);

private:
    /**
     * Parse NX_STYLE_PWD_FILE and load users
     */
    void loadAvailableUsers();
    void loadUserPermissions(User user);

    /**
     * Set userPtr given the user name
     *
     * userPtr is assumed to be disconnected (anonymous)
     */
    void connectUser(std::string userName, UserPtr hUser);

    bool isAdmin(User user);

    std::set<UserPtr> connectedUsers; ///< set of shared ptr users
    std::set<User> availableUsers;
    std::map<std::string,std::string> userDigests;

    std::map<User,std::string> userPermissions;

    std::string parseSha1Script(const std::string& permissions, const std::string path);

    const User anonymous;
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
