/**
 * @file	src/Dispatcher.h
 * @date	Feb. 2016
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2016 Ph. Renaud-Goud / Opticalp

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

#ifndef SRC_DISPATCHER_H_
#define SRC_DISPATCHER_H_

#include "Poco/Util/Subsystem.h"

/**
 * Dispatcher
 *
 * Manage the bindings between modules.
 * Every interaction between @ref Module should pass by the dispatcher.
 */
class Dispatcher: public Poco::Util::Subsystem
{
public:
    Dispatcher();
    virtual ~Dispatcher();

    /**
      * Subsystem name
      *
      * Shall not contain spaces: see setLogger() call in initialize().
      */
     const char * name() const { return "Dispatcher"; }

     /// @name un/re/initialization methods
     ///@{
     /**
      *
      */
     void initialize(Poco::Util::Application& app);
     // void reinitialize(Application & app); // not needed. By default: uninit, then init.
     /**
      * Reset workflow.
      *
      */
     void uninitialize();
     ///@}

private:
     // flow bindings (from source port to multiple target ports)

     // counter flow bindings (from target port to source port)
};

#endif /* SRC_DISPATCHER_H_ */
