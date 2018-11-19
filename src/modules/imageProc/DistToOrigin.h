/**
 * @file	src/Modules/imageProc/DistToOrigin.h
 * @date	Aug 2018
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2018 Ph. Renaud-Goud / Opticalp

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

#ifndef SRC_MODULES_IMAGEPROC_DISTTOORIGIN_H_
#define SRC_MODULES_IMAGEPROC_DISTTOORIGIN_H_

#ifdef HAVE_OPENCV

#include "core/Module.h"

/**
 * Display centering informations on the input image 
 *
 * according to the delta x and delta y input values 
 * to the output image (color)
 */
class DistToOrigin: public Module
{
public:
    DistToOrigin(ModuleFactory* parent, std::string customName);

    std::string description()
    {
        return "Display centering information on the image";
    }

private:
    /**
     * Main logic
     */
    void process(int startCond);

    static size_t refCount; ///< reference counter to generate a unique internal name

    enum params
    {
        paramXpos,
        paramYpos,
		paramRatio,
        paramColorMode, // gray, red, green, blue
        paramColorLevel, // luminance
        paramCnt
    };

    Poco::Int64 getIntParameterValue(size_t paramIndex);
    void setIntParameterValue(size_t paramIndex, Poco::Int64 value);

	double getFloatParameterValue(size_t paramIndex);
	void setFloatParameterValue(size_t paramIndex, double value);

	std::string getStrParameterValue(size_t paramIndex);
    void setStrParameterValue(size_t paramIndex, std::string value);

    int xPos, yPos;
	double ratio;
    std::string title;

    enum colorMode
    {
        colorGray,
        colorRed,
        colorGreen,
        colorBlue,
        colorModeCnt
    };

    colorMode color;
    int colorLevel;

    /// Indexes of the input ports
    enum inPorts
    {
        imageInPort,
        deltaXPort,
        deltaYPort,
        inPortCnt
    };

    /// Indexes of the output ports
    enum outPorts
    {
        imageOutPort,
        outPortCnt
    };
};

#endif /* HAVE_OPENCV */
#endif /* SRC_MODULES_IMAGEPROC_DISTTOORIGIN_H_ */
