/**
 * @file FindBodyPrint.cpp
 * @brief Class which tries to find a body print feature in a given picture.
 * @author Daniel Giritzer, Tobias Egger
 * @copyright "THE BEER-WARE LICENSE" (Revision 42):
 * <giri@nwrk.biz> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Daniel Giritzer
 */

#include "FindBodyPrint.h"

#include <opencv2/imgproc.hpp>
#include <iostream>

#include "ImgShow.h"

bool FindBodyPrint::DoWork(cv::Mat& pic) {
    cv::Mat pic_HSV;
    cv::cvtColor(pic, pic_HSV, cv::COLOR_BGR2HSV);
    cv::Mat roiCenter = pic_HSV(cv::Rect(pic.cols * 0.38, pic.rows * 0.42, (pic.cols - pic.cols * 0.38) - (pic.cols - pic.cols * 0.62), (pic.rows - pic.rows * 0.42) - (pic.rows - pic.rows * 0.58)));
    cv::Mat hasBodyPrint;
    cv::inRange(roiCenter, m_LowerColorBound, m_UpperColorBound, hasBodyPrint);

    if(m_ShowInfo)
        ImgShow(hasBodyPrint, "Has body print", ImgShow::grey, false, true);

    if(cv::countNonZero(hasBodyPrint))
        return true;
    return false;
}
