/**
 * @file	src/core/ParameterizedEntityWithWorkers.h
 * @date	Jan. 2017
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

#ifndef SRC_CORE_PARAMETERIZEDENTITYWITHWORKERS_H_
#define SRC_CORE_PARAMETERIZEDENTITYWITHWORKERS_H_

#include "ParameterizedEntity.h"
#include "ParameterizedWithGetters.h"
#include "ParameterizedWithSetters.h"

/**
 * ParameterizedEntity that is able to handle cancel, reset, ...
 * 
 * Gateway to the parent for the ParameterSetter and the ParameterGetter
 */
class ParameterizedEntityWithWorkers:
        public ParameterizedEntity,
        public ParameterizedWithGetters,
        public ParameterizedWithSetters
{
public:
    ParameterizedEntityWithWorkers(std::string prefixKey):
        ParameterizedEntity(prefixKey),
        ParameterizedWithGetters(this),
        ParameterizedWithSetters(this)
        {   }

    ParameterizedEntityWithWorkers(std::string prefixKey, bool applyParametersFromSettersWhenAllSet):
        ParameterizedEntity(prefixKey),
        ParameterizedWithGetters(this),
        ParameterizedWithSetters(this, applyParametersFromSettersWhenAllSet)
        {   }

    virtual ~ParameterizedEntityWithWorkers() { }

    /**
     * To be used by the parameter setters to cancel the corresponding module
     */
    void cancel() { lazyCancel(); }

    virtual void waitCancelled() = 0;

    /**
     * To be used by the parameter setters to reset the corresponding module
     */
    void reset() { moduleReset(); }

protected:
    virtual void lazyCancel() = 0;
    virtual void moduleReset() = 0;
};

#endif /* SRC_CORE_PARAMETERIZEDENTITYWITHWORKERS_H_ */
