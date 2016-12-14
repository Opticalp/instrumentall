/**
 * @file	src/core/InPort.h
 * @date	Apr. 2016
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

#ifndef SRC_INPORT_H_
#define SRC_INPORT_H_

#include "Port.h"
#include "SeqTarget.h"

#include "Poco/Mutex.h"
#include "Poco/SharedPtr.h"

using Poco::SharedPtr;

class Dispatcher;

/**
 * InPort
 *
 * Parent class for InDataPort and TrigPort
 */
class InPort: public Port, public SeqTarget
{
public:
    InPort(Module* parent,
            std::string name,
            std::string description,
            size_t index, bool trig = false);

    /**
     * Constructor to be used to create empty input ports
     *
     * Either empty TrigPort or empty InDataPort
     */
    InPort(std::string name, std::string description, bool trig = false);

    virtual ~InPort() { }

    /**
     * Implementation of DataTarget::name
     *
     * as explicit inheritance from Port::name
     */
    std::string name() { return Port::name(); }

    /**
     * Implementation of DataTarget::description
     *
     * as explicit inheritance from Port::description
     */
    std::string description() { return Port::description(); }

    /**
     * Convenience function
     *
     * Allow not to check a dynamic_cast result.
     * Faster, simplier.
     */
    bool isTrig() { return isTrigFlag; }

private:
    void runTarget();

	void targetCancel();
	void targetWaitCancelled();
	void targetReset();

    bool isTrigFlag;
};

#endif /* SRC_INPORT_H_ */
