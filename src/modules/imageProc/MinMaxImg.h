/**
 * @file	src/modules/imageProc/MinMaxImg.h
 * @date    Feb. 2019
 * @author	PhRG / Opticalp.fr
 */

/*
 Copyright (c) 2019 Ph. Renaud-Goud / Opticalp

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

#ifndef SRC_MODULES_IMAGEPROC_MINMAXIMG_H_
#define SRC_MODULES_IMAGEPROC_MINMAXIMG_H_

#ifdef HAVE_OPENCV

#include "core/Module.h"

/**
* Combine two input images using element-wise min or max
* 
* Those operations correspond to binary AND and OR if applied to binary images
*/
class MinMaxImg: public Module
{
public:
	MinMaxImg(ModuleFactory* parent, std::string customName);

	std::string description()
	{
		return "Combine two images using elt-wise max or min. ";
	}

private:
    static size_t refCount; ///< reference counter to generate a unique internal name

	/**
	* Main logic
	*/
	void process(int startCond);


    /// Indexes of the input ports
    enum inPorts
    {
        imgAInPort,
        imgBInPort,
        inPortCnt
    };

    /// Indexes of the output ports
    enum outPorts
    {
        imageOutPort,
        outPortCnt
    };

    enum params
	{
		paramCombType,
		paramCnt
	};

//	Poco::Int64 getIntParameterValue(size_t paramIndex);
//	void setIntParameterValue(size_t paramIndex, Poco::Int64 value);
//
//	double getFloatParameterValue(size_t paramIndex);
//	void setFloatParameterValue(size_t paramIndex, double value);

	std::string getStrParameterValue(size_t paramIndex);
	void setStrParameterValue(size_t paramIndex, std::string value);

	/// enum used for imginType parameter values
	enum ImgInTypeValue
	{
	    imgInTypeMin,
	    imgInTypeMax,
	    imginTypeCnt
	};
    ImgInTypeValue imgInType;
};
#endif /* HAVE_OPENCV */
#endif /* SRC_MODULES_IMAGEPROC_MINMAXIMG_H_ */
