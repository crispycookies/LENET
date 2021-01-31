/**
 * @file FindRightArm.cpp
 * @brief Class which tries to find a right arm feature in a given picture.
 * @author Daniel Giritzer, Tobias Egger
 * @copyright "THE BEER-WARE LICENSE" (Revision 42):
 * <giri@nwrk.biz> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Daniel Giritzer
 */

#include "FindRightArm.h"

#include <opencv2/imgproc.hpp>
#include <iostream>

bool FindRightArm::DoWork(cv::Mat& pic) {
   /* cv::Mat pic_HSV;
    cv::cvtColor(pic, pic_HSV, cv::COLOR_BGR2HSV);
    cv::Mat roiRightHand = pic_HSV(cv::Rect(pic.cols - pic.cols * 0.3, pic.rows/ 2, pic.cols * 0.3, pic.rows/2));
    cv::Mat hasRHand;
    cv::inRange(roiRightHand, m_LowerColorBound, m_UpperColorBound, hasRHand);
    if(cv::countNonZero(hasRHand))
        return true; */
    return false;
}

/*
    cv::Mat roiRightArm = pic(cv::Rect(pic.cols * 0.85, pic.rows * 0.3, pic.cols - pic.cols * 0.85, (pic.rows - pic.rows * 0.3) - (pic.rows - pic.rows * 0.4)));
    cv::Mat hasRArm;
    cv::inRange(roiRightArm, cv::Scalar(6, 80, 63), cv::Scalar(19, 255, 153), hasRArm);
    if(cv::countNonZero(hasRArm))
        std::cout << "Has RArm" << std::endl;*/