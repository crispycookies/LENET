/**
 * @file FindHead.cpp
 * @brief Class which tries to find a head feature in a given picture.
 * @author Daniel Giritzer, Tobias Egger
 * @copyright "THE BEER-WARE LICENSE" (Revision 42):
 * <giri@nwrk.biz> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Daniel Giritzer
 */

#include "FindHead.h"

#include <opencv2/imgproc.hpp>
#include <iostream>

#include "ImgShow.h"

bool FindHead::DoWork(cv::Mat& pic) {
    cv::Mat pic_HSV;
    cv::cvtColor(pic, pic_HSV, cv::COLOR_BGR2HSV);
    cv::Mat roiHead = pic_HSV(cv::Rect(0, 0, pic.cols, pic.rows * 0.3));
    cv::Mat hasHead;
    cv::inRange(roiHead, m_LowerColorBound, m_UpperColorBound, hasHead);

    if(m_ShowInfo)
        ImgShow(hasHead, "Has head", ImgShow::grey, false, true);

    if(cv::countNonZero(hasHead))
        return true;
    return false;
}