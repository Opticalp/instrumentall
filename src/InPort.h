/**
 * @file	src/InPort.h
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

class OutPort;
class Dispatcher;

/**
 * InPort
 *
 * Parent class for InDataPort and TrigPort
 */
class InPort: public Port
{
public:
    InPort(Module* parent,
            std::string name,
            std::string description,
            int datatype,
            size_t index, bool trig = false);

    /**
     * Constructor to be used to create empty input ports
     *
     * Either empty TrigPort or empty InDataPort
     */
    InPort(OutPort* emptySourcePort,
            std::string name,
            std::string description, bool trig = false);

    virtual ~InPort() { }

    /// Retrieve the source port
    SharedPtr<OutPort*> getSourcePort()
        { return mSourcePort; }

    /**
     * Store that the data has been used and can be released.
     *
     * Release the corresponding locks, record that the data is not new
     * any more
     */
    virtual void release();

    bool isNew() { return !used; }

    /**
     * Convenience function
     *
     * Allow not to check a dynamic_cast result.
     * Faster, simplier.
     */
    bool isTrig() { return isTrigFlag; }

protected:
    /**
     * Set the source port
     *
     * This function should only be called by the dispatcher.
     *
     * If the source port was not the empty port,
     * the port that was previously bound is notified
     */
    void setSourcePort(SharedPtr<OutPort*> port);

    /**
     * Release the source port
     *
     * And replace it by the empty port.
     * If the source port was not the empty port,
     * the port that was previously bound is notified
     */
    void releaseSourcePort();

    /**
     * To be called by the dispatcher
     *
     * when new data is available, just before the push.
     */
    void setNew(bool value = true);

private:
    SharedPtr<OutPort*> mSourcePort;

    bool used;
    bool isTrigFlag;

    friend class Dispatcher;
};

#endif /* SRC_INPORT_H_ */
