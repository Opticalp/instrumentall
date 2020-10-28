/**
 * @file	src/Modules/imageProc/ImageMixer.h
 * @date	Oct 2020
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2020 Ph. Renaud-Goud / Opticalp

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

#ifndef SRC_MODULES_IMAGEPROC_IMAGEMIXER_H_
#define SRC_MODULES_IMAGEPROC_IMAGEMIXER_H_

#ifdef HAVE_OPENCV

#include "core/Module.h"

/**
 * Add 2 input images using scale factors
 *
 * Use either a file or a second input port as second image
 */
class ImageMixer: public Module
{
public:
    ImageMixer(ModuleFactory* parent, std::string customName);

    std::string description()
    {
        return "Add 2 input images using scale factors. \n"
        		"The second image can be an image file. See parameters. " ;
    }

private:
    /**
     * Main logic
     */
    void process(int startCond);
    
    cv::Mat mix(cv::Mat A, cv::Mat B);

    static size_t refCount; ///< reference counter to generate a unique internal name

    enum params
    {
        paramA,
        paramB,
		paramOffset,
		paramFilePath,
        paramCnt
    };
    
    double a,b,offset;

    double getFloatParameterValue(size_t paramIndex);
    void setFloatParameterValue(size_t paramIndex, double value);

    std::string getStrParameterValue(size_t paramIndex);
    void setStrParameterValue(size_t paramIndex, std::string value);

    std::string imagePath;
    cv::Mat fileImage;

    /// Indexes of the input ports
    enum inPorts
    {
        imageAInPort,
        imageBInPort,
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
#endif /* SRC_MODULES_IMAGEPROC_IMAGEMIXER_H_ */
