/**
 * @file	src/core/User.h
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

#ifndef SRC_CORE_USER_H_
#define SRC_CORE_USER_H_

#ifdef MANAGE_USERS

struct User
{
    std::string name;
    std::string description;
//    std::string group;
    size_t uid;
//    size_t gid;

    User(std::string n, size_t id, std::string desc):
        name(n), uid(id), description(desc) {   }

    User():
        name("anonymous"), uid(0), description("") {   }

    friend bool operator < (const User& l, const User& r) { return (l.uid < r.uid); }
    friend bool operator == (const User& l, const User& r) { return (l.uid == r.uid); }
};

#include "Poco/SharedPtr.h"

typedef Poco::SharedPtr<User const *> UserPtr;

#endif /* MANAGE_USERS */
#endif /* SRC_CORE_USER_H_ */
