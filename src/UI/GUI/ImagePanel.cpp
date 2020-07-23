/**
 * @file	src/UI/GUI/ImagePanel.cpp
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

#include "ImagePanel.h"

#ifdef HAVE_WXWIDGETS
#ifdef HAVE_OPENCV

#include "opencv2/imgproc/imgproc.hpp"

wxIMPLEMENT_DYNAMIC_CLASS(ImagePanel, wxPanel)

wxDECLARE_EVENT(RedrawEvent, wxCommandEvent);
wxDEFINE_EVENT(RedrawEvent, wxCommandEvent);

wxBEGIN_EVENT_TABLE(ImagePanel, wxPanel)
// some useful events
 //EVT_MOTION(ImagePanel::mouseMoved)
 EVT_LEFT_DOWN(ImagePanel::mouseDown)
 EVT_LEFT_UP(ImagePanel::mouseReleased)
 /*
 EVT_RIGHT_DOWN(ImagePanel::rightClick)
 EVT_LEAVE_WINDOW(ImagePanel::mouseLeftWindow)
 EVT_KEY_DOWN(ImagePanel::keyPressed)
 EVT_KEY_UP(ImagePanel::keyReleased)
 EVT_MOUSEWHEEL(ImagePanel::mouseWheelMoved)
 */

// catch paint events
EVT_PAINT(ImagePanel::paintEvent)
//Size event
EVT_SIZE(ImagePanel::OnSize)

EVT_COMMAND(wxID_ANY, RedrawEvent, ImagePanel::forceRedraw)

wxEND_EVENT_TABLE()


// some useful events
 /*
 void ImagePanel::mouseWheelMoved(wxMouseEvent& event) {}
 void ImagePanel::rightClick(wxMouseEvent& event) {}
 void ImagePanel::mouseLeftWindow(wxMouseEvent& event) {}
 void ImagePanel::keyPressed(wxKeyEvent& event) {}
 void ImagePanel::keyReleased(wxKeyEvent& event) {}
 */

ImagePanel::ImagePanel()
{
    _width = -1;
    _height = -1;
    _isNewImage = false;
    _captured = false;
    zoomReset();
}

ImagePanel::ImagePanel(wxFrame* parent) :
        wxPanel(parent)
{
    ImagePanel();
}

void ImagePanel::forceRedraw(wxCommandEvent& evt)
{
    Refresh();
}

void ImagePanel::setImage(cv::Mat& imgIn)
{
    _imageLock.writeLock();

    if (imgIn.empty())
    {
        if (_cvImage.empty())
            return;

        _cvImage = cv::Mat();
        _isNewImage = true;
        return;
    }

    cv::Mat image;
    cv::Mat tmpImg; // another temporary image

    switch (imgIn.channels())
    {
    case 1:
        // cvtColor can not handle CV_64F images...
        if (imgIn.type()==CV_64F)
        {
            double min,max;
            cv::minMaxLoc(imgIn,&min,&max);

            imgIn.convertTo(
                    tmpImg,      // output image
                    CV_8U,       // depth
                    255.0/max ); // scale factor
        }
        else
        {
            tmpImg = imgIn;
        }
        cv::cvtColor(tmpImg,image,cv::COLOR_GRAY2RGB);
            break;
    case 3:
        image = imgIn;
        break;
    default:
        throw Poco::DataFormatException("ImagePanel::setImage",
                "unsupported image type");
    }

    if (image.type()!=CV_8UC3)
    {
        double min,max;
        cv::minMaxLoc(image,&min,&max);

        image.convertTo(
                tmpImg,      // output image
                CV_8UC3,       // depth
                255.0/max ); // scale factor
    }
    else
    {
        tmpImg = image.clone();
    }

    cv::cvtColor(tmpImg,_cvImage,cv::COLOR_BGR2RGB);

    _isNewImage = true;

    _imageLock.unlock();

    // do not call refresh directly! since we can be in a worker thread...
    wxCommandEvent* evt = new wxCommandEvent(RedrawEvent,GetId());
    evt->SetEventObject(this);
    QueueEvent( evt );
}

/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */
void ImagePanel::paintEvent(wxPaintEvent & evt)
{
    // depending on your system you may need to look at double-buffered dcs
    wxPaintDC dc(this);
    render(dc);
}

/*
 * Here we do the actual rendering. I put it in a separate
 * method so that it can work no matter what type of DC
 * (e.g. wxPaintDC or wxClientDC) is used.
 */
void ImagePanel::render(wxDC&  dc)
{
    if (_cvImage.empty())
        return;

    _imageLock.readLock();

    int neww, newh;
    dc.GetSize( &neww, &newh );

    if( neww != _width || newh != _height || _isNewImage || _zoomChanged )
    {
    	if (_isNewImage)
    		zoomReset();

        wxImage image(_cvImage.cols,_cvImage.rows,_cvImage.data,true);

        float ratio;
        bool filled = true;

        // check which is the limiting dimension: w or h
        if (float(_cvImage.cols)/float(neww) < float(_cvImage.rows)/float(newh))
        {
        	// w is limiting
        	ratio = _zoomFactor * float(newh)/float(_cvImage.rows);
        	if (float(_cvImage.cols)*ratio < neww)
        		filled = false;
        }
        else
        {
        	// h is limiting
        	ratio = _zoomFactor * float(neww)/float(_cvImage.cols);
        	if (float(_cvImage.rows)*ratio < newh)
        		filled = false;
        }


    	float newCenterW = _zoomW * neww;
    	float newCenterH = _zoomH * newh;

        if (filled)
        {
            _resized = wxBitmap(
        			image
						.Size(  wxSize(neww/ratio , newh/ratio) ,
								wxPoint( -(float(_cvImage.cols)/2 - newCenterW/ratio),
										 -(float(_cvImage.rows)/2 - newCenterH/ratio) ) )
						.Scale( neww, newh /*, wxIMAGE_QUALITY_HIGH*/ )
								);

        }
		else
		{
        	_resized = wxBitmap(
        			image
						.Scale( _cvImage.cols*ratio,_cvImage.rows*ratio /*, wxIMAGE_QUALITY_HIGH*/ )
						.Size(  wxSize(neww , newh) ,
								wxPoint( -(float(_cvImage.cols)/2*ratio - newCenterW),
										 -(float(_cvImage.rows)/2*ratio - newCenterH) ) ,
								0, 0, 0   )
								);
		}


        // _resized = wxBitmap( image.Scale( neww, newh /*, wxIMAGE_QUALITY_HIGH*/ ) );

        _width = neww;
        _height = newh;
        dc.DrawBitmap( _resized, 0, 0, false );
    }
    else
    {
        dc.DrawBitmap( _resized, 0, 0, false );
    }

    _zoomChanged = false;
    _isNewImage = false;

    _imageLock.unlock();
}

/*
 * Here we call refresh to tell the panel to draw itself again.
 * So when the user resizes the image panel the image should be resized too.
 */
void ImagePanel::OnSize(wxSizeEvent& event){
    Refresh();
    //skip the event.
    event.Skip();
}

void ImagePanel::zoomReset()
{
	_zoomFactor = 1;
	_zoomW = 0.5;
	_zoomH = 0.5;
	_zoomChanged = true;
}

void ImagePanel::incZoom(float x, float y)
{
	float newZoom = _zoomFactor*2;

//	// limit to 4:1
//	if (_width*_width*newZoom/_cvImage.cols/_cvImage.cols > 4
//			&& _height*_height*newZoom/_cvImage.rows/_cvImage.rows > 4)
//		return;

	_zoomW = newZoom/_zoomFactor*(_zoomW-x)+x;
	_zoomH = newZoom/_zoomFactor*(_zoomH-y)+y;

	_zoomFactor = newZoom;
	_zoomChanged = true;
}

void ImagePanel::decZoom(float x, float y)
{
	float newZoom = _zoomFactor/2;

	if (newZoom <1)
	{
		zoomReset();
		return;
	}

	_zoomW = newZoom/_zoomFactor*(_zoomW-x)+x;
	_zoomH = newZoom/_zoomFactor*(_zoomH-y)+y;

	_zoomFactor = newZoom;
	_zoomChanged = true;
}

//void ImagePanel::mouseMoved(wxMouseEvent& event)
//{
//	if (_captured)
//	{
//		long x,y;
//		event.GetPosition(&x,&y);
//
//		if ((_lastX-x) || (_lastY-y))
//		{
//			_zoomChanged = true;
//			_zoomW += float(x-_lastX)/_width;
//			_zoomH += float(y-_lastY)/_height;
//			_lastX=x;
//			_lastY=y;
//			Refresh();
//		}
//
//	}
//}


void ImagePanel::mouseDown(wxMouseEvent& event)
{
	event.GetPosition(&_lastX,&_lastY);
	_captured = true;
}

void ImagePanel::mouseReleased(wxMouseEvent& event)
{
	long x,y;
	event.GetPosition(&x,&y);

	if ((_lastX-x) || (_lastY-y))
	{
		_zoomChanged = true;
		_zoomW += float(x-_lastX)/_width;
		_zoomH += float(y-_lastY)/_height;
	}

	_captured = false;

	Refresh();
}

#endif /* HAVE_OPENCV */
#endif /* HAVE_WXWIDGETS */

