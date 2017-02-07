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

#include "core/ModuleFactoryBranch.h"
#include "UI/PythonManager.h"

#include "Poco/NumberFormatter.h"
#include "Poco/Path.h"
#include "Poco/StringTokenizer.h"

size_t PythonModule::refCount = 0;

PythonModule::PythonModule(ModuleFactory* parent, std::string customName):
    Module(parent, customName)
{
    setInternalName("PythonModule"
                        + Poco::NumberFormatter::format(refCount));
    setCustomName(customName);
    setLogger("module." + name());

    // the input ports are defined by the parent factory selector.
    std::string sel(static_cast<ModuleFactoryBranch*>(parent)->getSelector());
    if (!sel.empty())
    {
        Poco::StringTokenizer tok(sel, ";", Poco::StringTokenizer::TOK_TRIM);
        size_t tokCnt = tok.count();
        setInPortCount(tokCnt);
        for (size_t ind = 0; ind < tokCnt; ind++)
            addTrigPort(tok[ind], "auto-generated trig port, "
                    "retrieved from parent factory selector parsing", ind);

        poco_information(logger(), Poco::NumberFormatter::format(tokCnt)
            + " trig ports were created for " + name());
    }

    // no output port

    // parameters
    setParameterCount(paramCnt);
    addParameter(paramScriptFilePath, "scriptFilePath",
            "path to the python script to be executed when trigged",
            ParamItem::typeString, "");

    // set parameter defaults
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
