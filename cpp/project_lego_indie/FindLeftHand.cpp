/**
 * @file FindLeftHand.cpp
 * @brief Class which tries to find a left hand feature in a given picture.
 * @author Daniel Giritzer, Tobias Egger
 * @copyright "THE BEER-WARE LICENSE" (Revision 42):
 * <giri@nwrk.biz> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Daniel Giritzer
 */

#include "FindLeftHand.h"

#include <opencv2/imgproc.hpp>
#include <iostream>

bool FindLeftHand::DoWork(cv::Mat& pic) {
    cv::Mat pic_HSV;
    cv::cvtColor(pic, pic_HSV, cv::COLOR_BGR2HSV);
    cv::Mat roiLeftHand = pic_HSV(cv::Rect(0, pic.rows/ 2, pic.cols * 0.3, pic.rows/2));
    cv::Mat hasLHand;
    cv::inRange(roiLeftHand, m_LowerColorBound, m_UpperColorBound, hasLHand);
    if(cv::countNonZero(hasLHand))
        return true;
    return false;
}