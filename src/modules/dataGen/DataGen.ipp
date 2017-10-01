/**
 * @file	src/modules/dataGen/DataGen.ipp
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


#include "DataGen.h"

template<typename T>
inline std::vector<T> DataGen::fillOutIntVect(int dataType)
{
    std::vector<T> retList;

    switch (DataItem::noContainerDataType(dataType))
    {
    case DataItem::typeInt32:
    case DataItem::typeUInt32:
    case DataItem::typeInt64:
    case DataItem::typeUInt64:
        while (!iQueue.empty())
        {
            retList.push_back(static_cast<T>(iQueue.front()));
            iQueue.pop();
        }
        break;
    default:
        poco_bugcheck_msg("DataGen::sendData >> unexpected data type");
        throw Poco::BugcheckException();
    }

    return retList;
}

template<typename T>
inline std::vector<T> DataGen::fillOutFloatVect(int dataType)
{
    std::vector<T> retList;

    switch (DataItem::noContainerDataType(dataType))
    {
    case DataItem::typeFloat:
    case DataItem::typeDblFloat:
        while (!fQueue.empty())
        {
            retList.push_back(static_cast<T>(fQueue.front()));
            fQueue.pop();
        }
        break;
    default:
        poco_bugcheck_msg("DataGen::sendData >> unexpected data type");
        throw Poco::BugcheckException();
    }

    return retList;
}

inline std::vector<std::string> DataGen::fillOutStrVect()
{
    std::vector<std::string> retList;

    while (!sQueue.empty())
    {
        retList.push_back(sQueue.front());
        sQueue.pop();
    }

    return retList;
}

#ifdef HAVE_OPENCV
inline cv::Mat DataGen::fillOutCvMat(Poco::Int64 seed)
{
//    #if CV_MAJOR_VERSION == 2
//    // do opencv 2 code
//    #elif CV_MAJOR_VERSION == 3
//    // do opencv 3 code
//    #endif

    // make a 640x1024 OpenCV matrix.
    cv::Mat M(640, 1024, CV_8U, CV_RGB(0,0,0));
    cv::putText(M, "InstrumentAll -- test pattern --",
            cv::Point(30, 60),
            cv::FONT_HERSHEY_PLAIN, 2.0,
            CV_RGB(seed, seed, seed), 2, CV_AA);

//    // create a window for display.
//     cv::namedWindow( "test it", CV_WINDOW_AUTOSIZE );
//     // Show our image inside it.
//     cv::imshow( "test it", M );
//
//     //poco_information(logger(),"Waiting for key stroke...");
//     cv::waitKey(1000);
//     //poco_information(logger(),"Got it!");
//
//     cv::destroyWindow("test it");

    return M;
}

inline std::vector<cv::Mat> DataGen::fillOutCvMatVect()
{
    std::vector<cv::Mat> retList;

    while (!iQueue.empty())
    {
        retList.push_back(fillOutCvMat(iQueue.front()));
        iQueue.pop();
    }

    return retList;
}
#endif
