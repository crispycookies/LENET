/**
 * @file FindLeftFoot.cpp
 * @brief Class which tries to find a left foot feature in a given picture.
 * @author Daniel Giritzer, Tobias Egger
 * @copyright "THE BEER-WARE LICENSE" (Revision 42):
 * <giri@nwrk.biz> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Daniel Giritzer
 */
#include "FindLeftFoot.h"

#include <opencv2/imgproc.hpp>
#include <iostream>

bool FindLeftFoot::DoWork(cv::Mat& pic) {
    cv::Mat pic_HSV;
    cv::cvtColor(pic, pic_HSV, cv::COLOR_BGR2HSV);
    cv::Mat roiLeftFoot = pic_HSV(cv::Rect(0, pic.rows * 0.8, pic.cols * 0.4, pic.rows-pic.rows * 0.8));
    cv::Mat hasLFoot;
    cv::inRange(roiLeftFoot, m_LowerColorBound, m_UpperColorBound, hasLFoot);
    if(cv::countNonZero(hasLFoot))
        return true;
    return false;
}