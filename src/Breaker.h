/**
 * @file	src/Breaker.h
 * @date	jul. 2016
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

#ifndef SRC_BREAKER_H_
#define SRC_BREAKER_H_

#include <map>

class DataSource;
class DataTarget;

/**
 * Breaker
 *
 * Temporary data flow breaker
 */
class Breaker
{
public:
	Breaker() { }
	virtual ~Breaker() { releaseBreaks(); }

	/**
	 * Catch all the source to target connections
	 * from the given source
	 */
	void breakAllTargetsFromSource(DataSource* source);

	/**
	 * Catch only the source to target connection
	 * for the given target
	 */
	void breakSourceToTarget(DataTarget* target);

	/**
	 * Release the breaks on the edges recorded in Breaker::edges
	 */
	void releaseBreaks();

private:
	typedef std::pair<DataTarget*, DataSource*> Edge;

	std::map<DataTarget*, DataSource*> edges; ///< broken edges
};

#endif /* SRC_BREAKER_H_ */
