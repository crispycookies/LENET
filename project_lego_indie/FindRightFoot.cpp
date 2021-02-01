/**
 * @file FindRightFoot.cpp
 * @brief Class which tries to find a right foot feature in a given picture.
 * @author Daniel Giritzer, Tobias Egger
 * @copyright "THE BEER-WARE LICENSE" (Revision 42):
 * <giri@nwrk.biz> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Daniel Giritzer
 */

#include "FindRightFoot.h"

#include <opencv2/imgproc.hpp>
#include <iostream>

#include "ImgShow.h"

bool FindRightFoot::DoWork(cv::Mat& pic) {
    cv::Mat pic_HSV;
    cv::cvtColor(pic, pic_HSV, cv::COLOR_BGR2HSV);
    cv::Mat roiRightFoot = pic_HSV(cv::Rect(pic.cols * 0.6, pic.rows * 0.8, pic.cols - pic.cols * 0.6, pic.rows-pic.rows * 0.8));
    cv::Mat hasRFoot;
    cv::inRange(roiRightFoot, m_LowerColorBound, m_UpperColorBound, hasRFoot);

    if(m_ShowInfo)
        ImgShow(hasRFoot, "Has right foot", ImgShow::grey, false, true);

    if(cv::countNonZero(hasRFoot))
        return true;
    return false;
}