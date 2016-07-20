/**
 * @file	src/DataTarget.cpp
 * @date	Jul. 2016
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

#include "DataTarget.h"

#include "DataAttributeIn.h"

DataTarget::DataTarget():
	dataSource(NULL),
	used(true)
{

}

DataTarget::~DataTarget()
{
	releaseDataSource();
}

DataSource* DataTarget::getDataSource()
{
	Poco::ScopedLock<Poco::FastMutex> lock(sourceLock);

	return dataSource;
}

void DataTarget::setDataSource(DataSource* source)
{
    if (!isSupportedDataType(source->dataType()))
        throw Poco::DataFormatException("setDataSource",
                "The target does not support this data type: "
        		+ DataItem::dataTypeStr(source->dataType()));

    Poco::ScopedLock<Poco::FastMutex> lock(sourceLock);

	if (dataSource)
		dataSource->removeDataTarget(this);

	dataSource = source;

	if (dataSource)
		dataSource->addDataTarget(this);
}

void DataTarget::releaseDataSource()
{
	setDataSource(NULL);
}

void DataTarget::setNew(bool value)
{
    if (value)
    {
        getDataSource()->readDataLock();
        used = false;
        newDataUnlock();
    }
    else
    {
    	// no need to newDataLock() since the only caller is InPort::release(),
    	// then, new data can not be written
    	used = true;
    }
}

void DataTarget::readDataAttribute(DataAttributeIn* pAttr)
{
	*pAttr = DataAttributeIn(
		getDataSource()->getDataAttribute(), this);
}

void DataTarget::release()
{
    setNew(false);
    getDataSource()->unlockData();
}
