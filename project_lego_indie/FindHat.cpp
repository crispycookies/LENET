/**
 * @file FindHat.cpp
 * @brief Class which tries to find a hat feature in a given picture.
 * @author Daniel Giritzer, Tobias Egger
 * @copyright "THE BEER-WARE LICENSE" (Revision 42):
 * <giri@nwrk.biz> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Daniel Giritzer
 */
#include "FindHat.h"

#include <opencv2/imgproc.hpp>
#include <iostream>

#include "ImgShow.h"

bool FindHat::DoWork(cv::Mat& pic) {
    cv::Mat pic_HSV;
    cv::cvtColor(pic, pic_HSV, cv::COLOR_BGR2HSV);
    cv::Mat roiHat = pic_HSV(cv::Rect(0, 0, pic.cols, pic.rows * 0.2));
    cv::Mat hasHat;
    cv::inRange(roiHat, m_LowerColorBound, m_UpperColorBound, hasHat);

    if(m_ShowInfo)
        ImgShow(hasHat, "Has hat", ImgShow::grey, false, true);

    if(cv::countNonZero(hasHat) > 500)
        return true;
    return false;
}
