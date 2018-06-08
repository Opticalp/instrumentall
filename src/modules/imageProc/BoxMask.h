/**
 * @file	src/modules/imageProc/BoxMask.h
 * @date    Jun. 2017
 * @author	PhRG / KG / AI
 */

/*
 Copyright (c) 2017 Ph. Renaud-Goud / Opticalp and contributors

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

#ifndef SRC_MODULES_IMAGEPROC_BOXMASK_H_
#define SRC_MODULES_IMAGEPROC_BOXMASK_H_

#ifdef HAVE_OPENCV

#include "core/Module.h"

/**
* Create a mask to apply on an image for a threshold in or out the mask
* 
* The parameters are center, size, angle, values and types. 
* if the sample position is detected automatically
*/
class BoxMask: public Module
{
public:
	BoxMask(ModuleFactory* parent, std::string customName);

	std::string description()
	{
		return "Create a mask with an image. \n"
			"Apply the rectangle or ellispe mask with invalue inside and outvalue outside \n "
			"return an image masked. three posibilities : ref, min or max";
	}

private:
    static size_t refCount; ///< reference counter to generate a unique internal name

	/**
	* Main logic
	*/
	void process(int startCond);

	/**
	 * Draw a rectangle on the maskOut image
	 *
	 * The rectangle parameters (size, position, rotation angle, fill value)
	 * are taken from the module parameters
	 */
	void buildMaskRectangle(cv::Mat& maskOut);

    /**
     * Draw an ellipse on the maskOut image
     *
     * The ellipse parameters (size, position, rotation angle, fill value)
     * are taken from the module parameters
     */
	void buildMaskEllipse(cv::Mat& maskOut);

    /// Indexes of the input ports
    enum inPorts
    {
        imageInPort,
        inPortCnt
    };

    /// Indexes of the output ports
    enum outPorts
    {
        maskPort,
        outPortCnt
    };

    enum params
	{
		paramImgInType,
		paramInValue,
		paramOutValue,
		paramBoxType,
		paramBoxWidth,
		paramBoxHeight,
		paramBoxAngle,
		paramBoxXCenter,
		paramBoxYCenter,
		paramCnt
	};

	Poco::Int64 getIntParameterValue(size_t paramIndex);
	void setIntParameterValue(size_t paramIndex, Poco::Int64 value);

	// angle
	double getFloatParameterValue(size_t paramIndex);
	void setFloatParameterValue(size_t paramIndex, double value);

	std::string getStrParameterValue(size_t paramIndex);
	void setStrParameterValue(size_t paramIndex, std::string value);

	Poco::Int64 imgWidth, imgHeight; //size input image
	Poco::Int64 inValue, outValue;
	Poco::Int64 boxWidth, boxHeight;
	Poco::Int64 boxXcenter, boxYcenter;
	double boxAngle;

	/// enum used for imginType parameter values
	enum ImgInTypeValue
	{
	    imgInTypeRef,
	    imgInTypeMin,
	    imgInTypeMax,
	    imginTypeCnt
	};
    ImgInTypeValue imgInType;

    /// enum used for boxType parameter possible values
    enum BoxTypeValue
    {
        boxTypeRect,
        boxTypeEllipse,
        boxTypeCnt
    };
    BoxTypeValue boxType;
};
#endif /* HAVE_OPENCV */
#endif /* SRC_MODULES_IMAGEPROC_BOXMASK_H_ */
