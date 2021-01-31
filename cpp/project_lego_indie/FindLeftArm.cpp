/**
 * @file FindLeftArm.cpp
 * @brief Class which tries to find a left arm feature in a given picture.
 * @author Daniel Giritzer, Tobias Egger
 * @copyright "THE BEER-WARE LICENSE" (Revision 42):
 * <giri@nwrk.biz> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Daniel Giritzer
 */

#include "FindLeftArm.h"

#include <opencv2/imgproc.hpp>
#include <iostream>

bool FindLeftArm::DoWork(cv::Mat& pic) {
   /* cv::Mat pic_HSV;
    cv::cvtColor(pic, pic_HSV, cv::COLOR_BGR2HSV);
    cv::Mat roiLeftHand = pic_HSV(cv::Rect(0, pic.rows/ 2, pic.cols * 0.3, pic.rows/2));
    cv::Mat hasLHand;
    cv::inRange(roiLeftHand, m_LowerColorBound, m_UpperColorBound, hasLHand);
    if(cv::countNonZero(hasLHand))
        return true; */
    return false;
}

    /*cv::Mat roiLeftArm = pic(cv::Rect(0, pic.rows * 0.3, pic.cols * 0.15, (pic.rows - pic.rows * 0.3) - (pic.rows - pic.rows * 0.4)));
    cv::Mat hasLArm;
    cv::inRange(roiLeftArm, cv::Scalar(6, 80, 63), cv::Scalar(19, 255, 153), hasLArm);
    if(cv::countNonZero(hasLArm))
        std::cout << "Has LArm" << std::endl; */