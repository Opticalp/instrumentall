/**
 * @file	src/Module.cpp
 * @date	dec. 2015
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2015 Ph. Renaud-Goud / Opticalp

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

#include "Module.h"
#include "ModuleFactory.h"
#include "ModuleManager.h"

#include "InPort.h"
#include "OutPort.h"

#include "Poco/NumberFormatter.h"

#include <typeinfo>
POCO_IMPLEMENT_EXCEPTION( ModuleException, Poco::Exception, "Module error")

std::vector<std::string> Module::names;
Poco::RWLock Module::namesLock;

void Module::notifyCreation()
{
    // if not EmptyModule, add this to module manager
    if (mParent)
        Poco::Util::Application::instance().getSubsystem<ModuleManager>().addModule(this);
}

Module::~Module()
{
    // notify parent factory
    if (mParent)
    {
        mParent->removeChildModule(this);

        // notify module manager
        Poco::Util::Application::instance().getSubsystem<ModuleManager>().removeModule(this);
    }

    // delete ports
    for (std::vector<InPort*>::iterator it=inPorts.begin(), ite=inPorts.end();
            it!=ite; it++)
        delete *it;

    for (std::vector<OutPort*>::iterator it=outPorts.begin(), ite=outPorts.end();
            it!=ite; it++)
        delete *it;
}

#include "Poco/Exception.h"

void Module::setInternalName(std::string internalName)
{
    namesLock.writeLock();

    switch (checkName(internalName))
    {
    case nameOk:
        mInternalName = internalName;
        names.push_back(mInternalName);
        namesLock.unlock();
        return;
    case nameExists:
        namesLock.unlock();
        throw Poco::ExistsException("setInternalName",
                internalName + " already in use");
    case nameBadSyntax:
        namesLock.unlock();
        throw Poco::SyntaxException("setInternalName",
                "The name should only contain alphanumeric characters "
                "or \"-\", \"_\", \".\"");
    }
}

void Module::setCustomName(std::string customName)
{
    if (customName.empty() || customName.compare(internalName())==0)
    {
        mName = internalName();
        return;
    }

    namesLock.writeLock();

    switch (checkName(customName))
    {
    case nameOk:
        mName = customName;
        names.push_back(mName);
        namesLock.unlock();
        return;
    case nameExists:
        freeInternalName();
        namesLock.unlock();
        throw Poco::ExistsException("setCustomName",
                customName + " already in use");
    case nameBadSyntax:
        freeInternalName();
        namesLock.unlock();
        throw Poco::SyntaxException("setCustomName",
                "The name should only contain alphanumeric characters "
                "or \"-\", \"_\", \".\"");
    }
}

ModuleFactory* Module::parent()
{
    if (mParent)
        return mParent;
    else
        throw ModuleException("parent",
                "This module has no valid parent factory");
}

void Module::addInPort(std::string name, std::string description,
        DataItem::DataTypeEnum dataType, size_t index)
{
    if (index<inPorts.size())
        inPorts[index] = new InPort(this, name, description, dataType, index);
    else
        poco_bugcheck_msg(("addInPort: wrong index "
                + Poco::NumberFormatter::format(index)).c_str());
}

void Module::addOutPort(std::string name, std::string description,
        DataItem::DataTypeEnum dataType, size_t index)
{
    if (index<outPorts.size())
        outPorts[index] = new OutPort(this, name, description, dataType, index);
    else
        poco_bugcheck_msg(("addOutPort: wrong index "
                + Poco::NumberFormatter::format(index)).c_str());
}

#include "Poco/RegularExpression.h"

Module::NameStatus Module::checkName(std::string newName)
{
    // check syntax
    Poco::RegularExpression regex("^[0-9a-zA-Z\\._-]+$");
    if (!regex.match(newName))
        return nameBadSyntax;

    // check existance
    for (std::vector<std::string>::iterator it = names.begin(),
            ite = names.end(); it != ite; it++)
        if (it->compare(newName)==0)
            return nameExists;

    return nameOk;
}

void Module::freeInternalName()
{
    // verify that this is a module creation process
    if (!name().empty())
        return;

    for (std::vector<std::string>::reverse_iterator it = names.rbegin(),
            ite = names.rend(); it != ite; it++ )
    {
        if (it->compare(internalName())==0)
        {
            names.erase((it+1).base());
            return;
        }
    }
}

void Module::addParameter(size_t index, std::string name, std::string descr, ParamItem::ParamType datatype)
{
    try
    {
        paramSet.at(index).name = name;
        paramSet[index].descr = descr;
        paramSet[index].datatype = datatype;
    }
    catch (std::out_of_range& )
    {
        poco_bugcheck_msg("addParameter: incorrect index. "
                "Please check your module constructor");
    }
}

void Module::getParameterSet(ParameterSet* pSet)
{
    // mainMutex.lock();
    pSet->clear();
    pSet->reserve(paramSet.size());
    for (ParameterSet::iterator it = paramSet.begin(),
            ite = paramSet.end(); it != ite; it++)
    {
        pSet->push_back(ParamItem());
        pSet->back().name = it->name;
        pSet->back().descr = it->descr;
        pSet->back().datatype = it->datatype;
    }
    // mainMutex.unlock();
}

size_t Module::getParameterIndex(std::string paramName)
{
    size_t length = paramSet.size();

    for (size_t index = 0; index < length; index++)
        if (paramSet[index].name.compare(paramName) == 0)
            return index;

    throw Poco::NotFoundException("getParameterIndex", "parameter name not found");
}

ParamItem::ParamType Module::getParameterType(std::string paramName)
{
    // TODO: mainMutex scoped lock
    return paramSet[getParameterIndex(paramName)].datatype;
}
