/**
 * @file	src/DataManager.cpp
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

#include "DataManager.h"

#include "OutPort.h"

DataManager::DataManager()
{
    // nothing to do
}

DataManager::~DataManager()
{
    uninitialize(); // should have been already called by the system.

    // dataStore should clean itself nicely
}

void DataManager::initialize(Poco::Util::Application& app)
{
}

void DataManager::uninitialize()
{
}

void DataManager::addOutPort(OutPort* port)
{
    allData.push_back(SharedPtr<DataItem*>(new DataItem*(port->dataItem())));
}

void DataManager::removeOutPort(OutPort* port)
{
    // TODO:
    // - remove the outPort dataItem from the dataStore.
    // - replace it by an empty dataItem
}
