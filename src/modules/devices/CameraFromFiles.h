/**
 * @file	src/modules/devices/CameraFromFiles.h
 * @date	jan. 2017
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2017 Ph. Renaud-Goud / Opticalp

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

#ifndef SRC_MODULES_DEVICES_CAMERAFROMFILES_H_
#define SRC_MODULES_DEVICES_CAMERAFROMFILES_H_

#ifdef HAVE_OPENCV

#include "core/Module.h"

/**
 * Fake camera device to load images from files.
 * 
 * A camera device is defined as follows:
 *  - generate image on direct runModule
 *  - generate image on input port trig
 */
class CameraFromFiles: public Module
{
public:
    CameraFromFiles(ModuleFactory* parent, std::string customName);
    virtual ~CameraFromFiles() { }

    std::string description()
    {
        return "Fake camera providing images \nissued from the file system. ";
    }

private:
    /**
     * Main logic
     */
    void process(int startCond);

    void reset() { currentImgPath = imgPaths.begin(); Module::reset(); }

    static size_t refCount; ///< reference counter to generate a unique internal name

    enum params
    {
        paramDirectory,
        paramFiles,
        paramForceGrayscale,
        paramCnt
    };

    std::string getStrParameterValue(size_t paramIndex);
    void setStrParameterValue(size_t paramIndex, std::string value);

    Poco::Path imgDir; ///< image files absolute directory
    std::vector< std::string > imgPaths; ///< paths of the images
    std::vector< std::string >::iterator currentImgPath; ///< position of the next image to be generated from imgPaths.

    bool forceGrayscale;

    /// Indexes of the input ports
    enum inPorts
    {
        trigPort,
        inPortCnt
    };

    /// Indexes of the output ports
    enum outPorts
    {
        imgOutPort,
        acqReadyOutPort,
        outPortCnt
    };

    Poco::RWLock dataLock; ///< general lock for any data of this module
};

#endif /* HAVE_OPENCV */
#endif /* SRC_MODULES_DEVICES_CAMERAFROMFILES_H_ */
