/**
 * @file	src/modules/extern/PythonModule.cpp
 * @date	Feb. 2017
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

#ifdef HAVE_PYTHON27

#include "PythonModule.h"

#include "UI/PythonManager.h"

#include "Poco/NumberFormatter.h"
#include "Poco/Path.h"

size_t PythonModule::refCount = 0;

PythonModule::PythonModule(ModuleFactory* parent, std::string customName):
    Module(parent, customName)
{
    setInternalName("PythonModule"
                        + Poco::NumberFormatter::format(refCount));
    setCustomName(customName);
    setLogger("module." + name());

    // the input ports are defined by setParameterValue

    // no output port

    // parameters
    setParameterCount(paramCnt);
    addParameter(paramInPortCount, "inPortCount",
            "Set the input port count. "
            "The value can not be changed if non-zero. ",
            ParamItem::typeInteger, "0");
    addParameter(paramScriptFilePath, "scriptFilePath",
            "path to the python script to be executed when trigged",
            ParamItem::typeString, "");

    // set parameter defaults
    setIntParameterValue(paramInPortCount, getIntParameterDefaultValue(paramInPortCount));
    setStrParameterValue(paramScriptFilePath, getStrParameterDefaultValue(paramScriptFilePath));

    notifyCreation();

    // if nothing failed
    refCount++;
}

std::string PythonModule::description()
{
    return "Execute the given python script when all the input ports "
            "are trigged. You should consider using a dataGen module "
            "if you need to ouput data. The input ports are locked  "
            "as long as the python script is executing. ";
}

void PythonModule::process(int startCond)
{
    if (scriptPath.size())
        Poco::Util::Application::instance().getSubsystem<PythonManager>().runScript(scriptPath);
    else
        throw Poco::RuntimeException("process", "The script file path is not defined");
}

Poco::Int64 PythonModule::getIntParameterValue(size_t paramIndex)
{
    poco_assert(paramIndex == paramInPortCount);

    return getInPortCount();
}

void PythonModule::setIntParameterValue(size_t paramIndex, Poco::Int64 value)
{
    poco_assert(paramIndex == paramInPortCount);

    if (getInPortCount())
        throw Poco::RuntimeException("setParameterValue",
                "impossible to set inPortCount parameter: "
                "input ports are already present");

    if (value == 0)
        return;

    if (value < 0)
        throw Poco::RangeException("setParameterValue",
                "inPortCount has to be positive");

    setInPortCount(value);
    for (Poco::Int64 ind = 0; ind < value; ind++)
        addTrigPort("trig" + Poco::NumberFormatter::format(ind),
                "Auto-generated trig port. "
                "The count of trig ports is defined "
                "by the inPortCount parameter", ind);
}

std::string PythonModule::getStrParameterValue(size_t paramIndex)
{
    poco_assert(paramIndex == paramScriptFilePath);

    return scriptPath;
}

void PythonModule::setStrParameterValue(size_t paramIndex, std::string value)
{
    poco_assert(paramIndex == paramScriptFilePath);

    if (value.empty())
        scriptPath.clear();
    else
        scriptPath = Poco::Path(value).absolute().toString();
}

#endif /* HAVE_PYTHON27 */
