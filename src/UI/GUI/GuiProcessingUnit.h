/**
 * @file	src/UI/GUI/GuiProcessingUnit.h
 * @date	Apr. 2016
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

#ifndef SRC_GUIPROCESSINGUNIT_H_
#define SRC_GUIPROCESSINGUNIT_H_

#ifdef HAVE_WXWIDGETS

#include "core/VerboseEntity.h"

#include "UI/python/PythonScriptRunner.h"

#include "Poco/SharedPtr.h"
#include "Poco/Path.h"

#include "Poco/Thread.h"

#ifdef HAVE_OPENCV
#  include "opencv2/opencv.hpp"
#endif

class TopFrame;
class ImagePanel;


/**
 * GuiProcessingUnit
 *
 * Handle all the processing behind the GUI
 */
class GuiProcessingUnit: private VerboseEntity
{
public:
    GuiProcessingUnit(TopFrame* parent);
    virtual ~GuiProcessingUnit();

//    void registerImagePanel(ImagePanel* imgPnl)
//        { imagePanel = imgPnl; }

    void setPyGuiScript(Poco::Path scriptPath)
    {  guiScript = scriptPath; }

    /// run python script
    void runPyScript(Poco::Path scriptPath, bool repeat = false);

    /// run GUI script once
    void runPyScript();

    /// run GUI script in loop
    void runLoopPyScript();

    /**
     * Launch guiscript if autostart
     */
    void init();

    /// emergency stop (cancelAll)
    void stop();

    bool isScriptRunning();

#ifdef HAVE_OPENCV
    void showImage(cv::Mat img);
#endif

    std::string getStatusBarTxt(int field);
    void setStatusBarTxt(std::string txt, int field);

    std::string getTextCtrlTxt();
    void setTextCtrlTxt(std::string txt);

private:
    TopFrame* topFrame;

    Poco::Path guiScript;

    PythonScriptRunner pyRunner;

    Poco::Thread pyThread; ///< thread for the pyRunner
};

#endif /* HAVE_WXWIDGETS */
#endif /* SRC_GUIPROCESSINGUNIT_H_ */
