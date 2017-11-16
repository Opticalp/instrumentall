/**
 * @file	src/core/UserManager.cpp
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

#ifdef MANAGE_USERS

#include "UserManager.h"

#include "Poco/Util/Application.h"

#include "Poco/SHA1Engine.h"
#include "Poco/String.h"
#include "Poco/StringTokenizer.h"
#include "Poco/File.h"
#include "Poco/Path.h"

#include "Poco/NumberFormatter.h"
#include "Poco/NumberParser.h"

#include <fstream>

///// File where to find the signatures of the other files
//#define NX_STYLE_SIGN_FILE "access/.signs"

/// File where to store the user credentials in the form `[username]:[description]:[uid]:[digest]\n`
#define NX_STYLE_PWD_FILE "access/.passwd"

///// File where to store group membership `[groupname]:[gid]:[allowAll|denyAll]:[uid1,uid2,...]\n`
//#define NX_STYLE_GRP_FILE "access/.groups"

/**
 * Folder where to store the fine grained permissions
 *
 * file name : .UID-<UID>
 */
#define NX_STYLE_TRUSTED_ITEMS_FOLDER "access/"

#define USER_ADMIN_UID 1

UserManager::UserManager():
    VerboseEntity(name()),
    anonymous()
{

}

UserManager::~UserManager()
{
    // uninitialize(); // should have been already called by the system.
}

void UserManager::initialize(Poco::Util::Application& app)
{
    setLogger(name());

    // TODO: check that the user permission files exist and are readable
    // TODO: evtl: parse and load into memory
    loadAvailableUsers();

    // TODO: should not be able to init if users remain in connectedUsers?
}

void UserManager::uninitialize()
{
    poco_information(logger(),"User manager uninitializing");

    for (std::set<UserPtr>::iterator it = connectedUsers.begin(),
            ite = connectedUsers.end(); it != ite; it++)
        disconnectUser(*it);

    // TODO: clear available users, userDigests and userPermissions
    // TODO: should thread-protect this using mutexes.

    poco_information(logger(),"User manager uninitialized");
}

void UserManager::loadAvailableUsers()
{
    Poco::Util::Application& app = Poco::Util::Application::instance();

    Poco::Path tmpPath(NX_STYLE_PWD_FILE, Poco::Path::PATH_UNIX);
    Poco::Path pwdPath(app.config().getString("application.dir"));
    pwdPath.append(tmpPath);

    pwdPath.makeAbsolute();

    if (!pwdPath.isFile()
                 || !Poco::File(pwdPath).exists()
                 || !Poco::File(pwdPath).canRead() )
     {
         Poco::FileException e("loadAvailableUsers",
                 "unable to read the passwd file: "
                 + pwdPath.toString());
         poco_error(logger(),e.displayText());
         throw e;
     }

     // parse the file content
     std::ifstream ifs(pwdPath.toString().c_str());

     std::string line;
     while (std::getline(ifs, line))
     {
         Poco::StringTokenizer tok(line, ":");
         if (tok.count() != 4) // name, uid, description, digest
         {
             Poco::SyntaxException e("loadAvailableUsers",
                 "passwd file syntax error in: \n"
                 + line);
             poco_error(logger(),e.displayText());
             throw e;
         }

         userDigests.insert(std::pair<std::string,std::string>(tok[0],tok[3]));
         User newUser(tok[0], Poco::NumberParser::parse64(tok[1]), tok[2]);
         availableUsers.insert(newUser);

         if (!isAdmin(newUser)) // admin has all permissions
             loadUserPermissions(newUser);
     }

     poco_information(logger(),
             Poco::NumberFormatter::format(availableUsers.size())
         + " users available. (+ anonymous)");

     if (!availableUsers.size())
         poco_warning(logger(),"loadAvailableUsers: "
                 "no user is defined in the passwd file. No Admin. ");
}

void UserManager::loadUserPermissions(User user)
{
    Poco::Util::Application& app = Poco::Util::Application::instance();


    Poco::Path tmpPath(NX_STYLE_TRUSTED_ITEMS_FOLDER
            + Poco::format(".UID-%z", user.uid), Poco::Path::PATH_UNIX);
    Poco::Path permPath(app.config().getString("application.dir"));
    permPath.append(tmpPath);

    permPath.makeAbsolute();

    if (!permPath.isFile()
                 || !Poco::File(permPath).exists()
                 || !Poco::File(permPath).canRead() )
     {
         poco_error(logger(), "loadUserPermissions: "
                 "unable to read the permissions file: "
                 + permPath.toString() );
         userPermissions.insert(std::pair<User, std::string>(user, ""));
         return;
     }

     // transfer the file content into a string
     std::ifstream ifs(permPath.toString().c_str());

     ifs.seekg(0, std::ios::end);
     std::string buf;
     buf.reserve(ifs.tellg());
     ifs.seekg(0, std::ios::beg);

     buf.assign((std::istreambuf_iterator<char>(ifs)),
                 std::istreambuf_iterator<char>());

     userPermissions.insert(std::pair<User, std::string>(user, buf));
     poco_information(logger(), user.name + " permissions: \n" + buf);
}

void UserManager::initUser(UserPtr& hUser)
{
    poco_information(logger(), "initializing user");
    poco_assert(hUser.isNull());

    hUser = new (User const *)(&anonymous); // new pointer on anonymous
}

void UserManager::disconnectUser(UserPtr hUser)
{
    if (hUser.isNull() || (*hUser) == NULL)
        poco_bugcheck_msg("The hUser should be instanciated before disconnection");

    connectedUsers.erase(hUser);
    *hUser = &anonymous;
}

void UserManager::connectUser(std::string userName, UserPtr hUser)
{
    if (hUser.isNull() || (*hUser) == NULL)
        poco_bugcheck_msg("The hUser should be instanciated before connection");

    for (std::set<User>::iterator it = availableUsers.begin(),
            ite = availableUsers.end(); it != ite; it++)
    {
        if (it->name.compare(userName) == 0)
        {
            //*hUser = const_cast<User*>(&(*it));
            *hUser = &(*it);

            if (*it == anonymous)
                return;

            connectedUsers.insert(hUser);
                return;
        }
    }

    poco_bugcheck_msg("The user to be connected was not found...");
}

bool UserManager::authenticate(std::string userName, std::string password,
        UserPtr userPtr)
{
    disconnectUser(userPtr);
    if (!verifyPasswd(userName, password))
        return false;

    connectUser(userName, userPtr);
    return true;
}

bool UserManager::verifyPasswd(UserPtr userPtr, std::string password)
{
    if (!userPtr.isNull() && (*userPtr))
        return verifyPasswd((*userPtr)->name, password);
    else
        return false;
}

bool UserManager::verifyPasswd(std::string userName, std::string password)
{
    std::map<std::string, std::string>::iterator it = userDigests.find(userName);
    if (it == userDigests.end())
        return false;

    Poco::SHA1Engine sha1;
    sha1.update(password);
    const Poco::DigestEngine::Digest& digest = sha1.digest();
    std::string digestString(Poco::DigestEngine::digestToHex(digest));

    return (Poco::icompare(digestString, it->second) == 0);
}

bool UserManager::isAdmin(UserPtr userPtr)
{
    if (userPtr.isNull() || ((*userPtr) == NULL))
        poco_bugcheck_msg("The hUser should be instanciated before call to isAdmin(hUser)");

    return isAdmin(**userPtr);
}

bool UserManager::isAdmin(User user)
{
    return (user.uid == USER_ADMIN_UID);
}

bool UserManager::isScriptAuthorized(std::string path, std::string& content,
        UserPtr userPtr)
{
    if (isAdmin(userPtr))
        return true;

    // to be implemented

    return true;
}

bool UserManager::isFolderAuthorized(Poco::Path folderPath, UserPtr userPtr)
{
    if (isAdmin(userPtr))
        return true;

    // to be implemented

    return true;
}

#endif /* MANAGE_USERS */
