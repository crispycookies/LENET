/**
 * @file FindRightHand.cpp
 * @brief Class which tries to find a left hand feature in a given picture.
 * @author Daniel Giritzer, Tobias Egger
 * @copyright "THE BEER-WARE LICENSE" (Revision 42):
 * <giri@nwrk.biz> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Daniel Giritzer
 */

#include "FindRightHand.h"

#include <opencv2/imgproc.hpp>
#include <iostream>

bool FindRightHand::DoWork(cv::Mat& pic) {
    cv::Mat pic_HSV;
    cv::cvtColor(pic, pic_HSV, cv::COLOR_BGR2HSV);
    cv::Mat roiRightHand = pic_HSV(cv::Rect(pic.cols - pic.cols * 0.3, pic.rows/ 2, pic.cols * 0.3, pic.rows/2));
    cv::Mat hasRHand;
    cv::inRange(roiRightHand, m_LowerColorBound, m_UpperColorBound, hasRHand);
    if(cv::countNonZero(hasRHand))
        return true;
    return false;
}