/**
 * Freely inspired from:
 * https://wiki.wxwidgets.org/An_image_panel
 *
 * @file	src/UI/GUI/ImagePanel.h
 * @date	Mar. 2015
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2015 Ph. Renaud-Goud / Opticalp

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

#ifndef SRC_GUI_IMAGEPANEL_H_
#define SRC_GUI_IMAGEPANEL_H_

#ifdef HAVE_WXWIDGETS
#ifdef HAVE_OPENCV

// #include <wx/generic/panelg.h>

#include <wx/wx.h>
#include <wx/sizer.h>

// openCV core
#include "opencv2/core/core.hpp"

#include "Poco/RWLock.h"

/**
 * ImagePanel
 *
 * class to show images in a custom panel
 */
class ImagePanel : public wxPanel
{
public:
    ImagePanel(wxFrame* parent);
    ImagePanel();
    virtual ~ImagePanel() { }

    /// set a new image to be displayed
    void setImage(cv::Mat& imgIn);

    /// reset the zoom
    void zoomReset();

    /// increment the zoom
    void incZoom(float x=0.5, float y=0.5);

    /// decrement the zoom
    void decZoom(float x=0.5, float y=0.5);

private:
    void paintEvent(wxPaintEvent & evt);
    void OnSize(wxSizeEvent& event);
    void render(wxDC& dc);
    void forceRedraw(wxCommandEvent& evt);

    // some useful events
     // void mouseMoved(wxMouseEvent& event);
     void mouseDown(wxMouseEvent& event);
     void mouseReleased(wxMouseEvent& event);
     /*
     void mouseWheelMoved(wxMouseEvent& event);
     void rightClick(wxMouseEvent& event);
     void mouseLeftWindow(wxMouseEvent& event);
     void keyPressed(wxKeyEvent& event);
     void keyReleased(wxKeyEvent& event);
     */

    cv::Mat _cvImage;
    wxBitmap _resized;
    int _width, _height;
    bool _isNewImage;

    float _zoomFactor; ///< 1 for unzoomed (full image).
    float _zoomW,_zoomH; ///< center image position
    bool _zoomChanged;

    bool _captured;
    long _lastX,_lastY;

    Poco::RWLock _imageLock;

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_DYNAMIC_CLASS(ImagePanel);
};

#endif /* HAVE_OPENCV */
#endif /* HAVE_WXWIDGETS */
#endif /* SRC_GUI_IMAGEPANEL_H_ */
