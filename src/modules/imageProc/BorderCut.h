/**
 * @file	src/modules/imageProc/BorderCut.h
 * @date    Feb. 2019
 * @author	PhRG - opticalp.fr
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

#ifndef SRC_MODULES_IMAGEPROC_BORDERCUT_H_
#define SRC_MODULES_IMAGEPROC_BORDERCUT_H_

#ifdef HAVE_OPENCV

#include "core/Module.h"

/**
 * Detect the borders and cut the image
 * 
 * Use the image sum on cols or rows
 * Retrieve the next item diff
 * as soon as the diff is smaller than the threshold, starting from 
 * the border, we assume that we are outside the image. 
 */
class BorderCut: public Module
{
public:
    BorderCut(ModuleFactory* parent, std::string customName);

    std::string description()
    {
        return "Detect the borders and crop the image to remove borders";
    }

private:
    /**
     * Main logic
     */
    void process(int startCond);
    
    /**
     * Check if a border is present
     * 
     * @param inVec vector to be analyzed
     * @return 0 if no edge is detected. Positive value if an border 
     *         is detected in the crescent direction of the indexes.
     *         The absolute value gives the distance to the outer image edge. 
     */
    int hasEdge(cv::Mat inVec);

    static size_t refCount; ///< reference counter to generate a unique internal name

    enum params
    {
        paramThreshold,
        paramPadding,
        paramMinEdgeLength,
        paramCnt
    };

    Poco::Int64 getIntParameterValue(size_t paramIndex);
    void setIntParameterValue(size_t paramIndex, Poco::Int64 value);

    double getFloatParameterValue(size_t paramIndex);
    void setFloatParameterValue(size_t paramIndex, double value);

    Poco::Int64 minEdgeLen, padding;
    double thres;

    /// Indexes of the input ports
    enum inPorts
    {
        imageInPort,
        inPortCnt
    };

    /// Indexes of the output ports
    enum outPorts
    {
        imageOutPort,
        // TODO: add output(s) to indicate which border(s) was(were) cut
        outPortCnt
    };

};

#endif /* HAVE_OPENCV */
#endif /* SRC_MODULES_IMAGEPROC_BORDERCUT_H_ */
