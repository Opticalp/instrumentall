/**
 * @file	src/core/DuplicatedSource.cpp
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

#include "DuplicatedSource.h"

#include "Dispatcher.h"
#include "Poco/Util/Application.h"

DuplicatedSource::DuplicatedSource(DataSource* source):
	DataSource(source), mName("duplicated" + source->name())
{
	std::set<DataTarget*> targets(source->getDataTargets());

	breaker.breakAllTargetsFromSource(source);

	for (std::set<DataTarget*>::iterator it = targets.begin(),
			ite = targets.end(); it != ite; it++)
		Poco::Util::Application::instance()
	                        .getSubsystem<Dispatcher>()
	                        .bind(this, *it);
}

DuplicatedSource::DuplicatedSource(DataSource* source, DataTarget* target):
	DataSource(source), breaker(source, target),
	mName("duplicated" + source->name())
{
	Poco::Util::Application::instance()
	                        .getSubsystem<Dispatcher>()
	                        .bind(this, target);
}

void DuplicatedSource::trigTargets()
{
    Poco::Util::Application::instance()
                        .getSubsystem<Dispatcher>()
                        .setOutputDataReady(this);
}

